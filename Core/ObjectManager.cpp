//
// Created by AFETT on 2024/8/18.
//

#include "ObjectManager.h"
ObjectManager::ObjectManager(TdClientCore &TdClient): td_client_(TdClient) {}

std::string ObjectManager::GetUserName(const UserId User_id) {
    const auto it = users_.find(User_id);
    if (it == users_.end()) {
        return "unknown user";
    }
    return it->second->first_name_ + " " + it->second->last_name_;
}

std::string ObjectManager::GetChatTitle(const ChatId Chat_id) {
    const auto it = chat_titles_.find(Chat_id);
    if (it == chat_titles_.end()) {
        return "unknown chat";
    }
    return it->second;
}

void ObjectManager::ProcessObject(Ptr_Object Message) {
    td::td_api::downcast_call(
        *Message, Utils::overloaded(
            [this](td::td_api::updateNewChat &Update_new_chat) {
                chat_titles_[Update_new_chat.chat_->id_] = Update_new_chat.chat_->title_;
            },
            [this](td::td_api::updateChatTitle &Update_chat_title) {
                chat_titles_[Update_chat_title.chat_id_] = Update_chat_title.title_;
            },
            [this](td::td_api::updateUser &Update_user) {
                const auto user_id = Update_user.user_->id_;
                users_[user_id]    = std::move(Update_user.user_);
            },
            [this](td::td_api::updateChatFolders &Update_chat_folders) {
                std::ranges::for_each(Update_chat_folders.chat_folders_, [this](auto &folder) {
                    folder_titles_[folder->id_] = folder->title_;
                });
            },
            [this](td::td_api::updateNewMessage &Update_new_message) -> concurrencpp::null_result {
                // move to ptr to avoid destroy early
                auto newObj = Utils::Make<td::td_api::updateNewMessage>(std::move(Update_new_message));

                auto setSenderName = [this, &newObj] {
                    switch (newObj->message_->sender_id_->get_id()) {
                        case td::td_api::messageSenderChat::ID: {
                            auto chat = Utils::MoveAs<td::td_api::messageSenderChat>(newObj->message_->sender_id_);
                            return this->chat_titles_.contains(chat->chat_id_) ? this->chat_titles_.at(chat->chat_id_) : "unknow_chat_title";
                        }
                        case td::td_api::messageSenderUser::ID: {
                            auto user = Utils::MoveAs<td::td_api::messageSenderUser>(newObj->message_->sender_id_);
                            return this->users_.contains(user->user_id_)
                                       ? this->users_.at(user->user_id_)->first_name_ + " " + this->users_.at(user->user_id_)->last_name_
                                       : "unknow_user";
                        }
                        default: break;
                    }
                    return std::string { "unknow_user_chat" };
                };
                auto downloadFileInner = [this](FileId File_id) -> concurrencpp::lazy_result<Ptr_File> {
                    auto object = co_await this->td_client_.SendQuery(Utils::Make<td::td_api::downloadFile>(File_id, 32, 0, 0, true));
                    co_return Utils::MoveAs<td::td_api::file>(object);
                };
                auto backupFileInner = [&setSenderName](const std::string &FilePath) {
                    try {
                        const std::filesystem::path file_path(FilePath);
                        const std::string           new_file_name = setSenderName() + "_" + file_path.filename().string();
                        std::filesystem::rename(file_path, file_path.parent_path() / new_file_name);
                    } catch (std::exception &Exception) {
                        std::println("error: {}", Exception.what());
                    }
                };
                auto normallyBackUp = [&downloadFileInner, &backupFileInner](const FileId File_id) -> concurrencpp::lazy_result<void> {
                    auto file = co_await downloadFileInner(File_id);
                    backupFileInner(file->local_->path_);
                };


                std::println("{}", to_string(newObj->message_));
                switch (newObj->message_->content_->get_id()) {
                    case td::td_api::messageAnimation::ID: {
                        co_await normallyBackUp(Utils::MoveAs<td::td_api::messageAnimation>(newObj->message_->content_)->animation_->animation_->id_);
                        break;
                    }
                    case td::td_api::messageAudio::ID: {}
                    case td::td_api::messagePaidMedia::ID: {}
                    case td::td_api::messagePhoto::ID: {}
                    case td::td_api::messageVideo::ID: {
                        co_await normallyBackUp(Utils::MoveAs<td::td_api::messageVideo>(newObj->message_->content_)->video_->video_->id_);
                        break;
                    }
                    case td::td_api::messageVideoNote::ID: {}
                    case td::td_api::messageVoiceNote::ID: {}
                    case td::td_api::messageExpiredPhoto::ID: {}
                    case td::td_api::messageExpiredVideo::ID: {}
                    case td::td_api::messageExpiredVideoNote::ID: {}
                    case td::td_api::messageExpiredVoiceNote::ID: {}
                    case td::td_api::messageUnsupported::ID: {}
                    default: break;
                }
                co_return;
            },
            [](td::td_api::updateSuggestedActions &Update_suggested_actions) {
#ifdef _DEBUG_LOG
                std::println("SuggestedActions: {}", to_string(Update_suggested_actions));
#endif
            },
            [](td::td_api::proxy &Proxy) {
#ifdef _DEBUG_LOG
                std::println("Successful setup Proxy: {}", to_string(Proxy));
#endif
            },
            [](td::td_api::updateConnectionState &Connection_state) {
#ifdef _DEBUG_LOG
                std::println("ConnectionState: {}", to_string(Connection_state));
#endif
            },
            [](auto &update) {
#ifdef _DEBUG_LOG
                std::println("Unexpected update: {}", nameof::nameof_full_type<decltype(update)>());
#endif
            }
        )
    );
}
