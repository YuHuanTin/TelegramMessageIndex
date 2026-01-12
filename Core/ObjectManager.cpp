//
// Created by AFETT on 2024/8/18.
//

module;

#include <concurrencpp/concurrencpp.h>
#include "td/telegram/td_api.hpp"

module ObjectManager;

ObjectManager::ObjectManager(TdClientCore &TdClient) : td_client_(TdClient) {}

std::string ObjectManager::_GetUserName(const UserId User_id) {
    const auto it = users_.find(User_id);
    if (it == users_.end()) {
        return "unknown user";
    }
    return it->second->first_name_ + " " + it->second->last_name_;
}

std::string ObjectManager::_GetChatTitle(const ChatId Chat_id) {
    const auto it = chat_titles_.find(Chat_id);
    if (it == chat_titles_.end()) {
        return "unknown chat";
    }
    return it->second;
}

std::string ObjectManager::GetSenderName(Utils::TdPtr<td::td_api::MessageSender> senderId) {
    switch (senderId->get_id()) {
        case td::td_api::messageSenderChat::ID: {
            auto chat = Utils::MoveAs<td::td_api::messageSenderChat>(senderId);
            return _GetChatTitle(chat->chat_id_);
        }
        case td::td_api::messageSenderUser::ID: {
            auto user = Utils::MoveAs<td::td_api::messageSenderUser>(senderId);
            return _GetUserName(user->user_id_);
        }
        default: {
            return std::string { "unknow_user_chat" };
        }
    }
}

void ObjectManager::_MoveLocalFile(std::string &localPath, const std::string &senderName) {
    try {
        const std::filesystem::path file_path(localPath);
        const std::string           new_file_name = senderName + "_" + file_path.filename().string();
        std::filesystem::rename(file_path, file_path.parent_path() / new_file_name);
    } catch (std::exception &Exception) {
        LogFormat::LogFormatter<LogFormat::Error>("error: {}", Exception.what());
    }
}

EagerRetType<ObjectManager::Ptr_File> ObjectManager::_DownloadFile(const FileId File_id) {
    auto object = co_await td_client_.SendQuery(Utils::Make<td::td_api::downloadFile>(File_id, 32, 0, 0, true));
    co_return Utils::MoveAs<td::td_api::file>(object);
}

EagerRetType<> ObjectManager::DownloadResource(Utils::TdPtr<td::td_api::MessageSender> sender, Utils::TdPtr<td::td_api::messageAnimation> animation) {
    auto file = co_await _DownloadFile(animation->animation_->animation_->id_);
    _MoveLocalFile(file->local_->path_, GetSenderName(std::move(sender)));
}
EagerRetType<> ObjectManager::DownloadResource(Utils::TdPtr<td::td_api::MessageSender> sender, Utils::TdPtr<td::td_api::messagePhoto> photo) {
    // find max size photo
    auto maxSizeIndex = 0;
    for (int i = 1; i < photo->photo_->sizes_.size(); ++i) {
        if (photo->photo_->sizes_[i]->width_ * photo->photo_->sizes_[i]->height_
            > photo->photo_->sizes_[maxSizeIndex]->width_ * photo->photo_->sizes_[maxSizeIndex]->height_) {
            maxSizeIndex = i;
        }
    }

    auto file = co_await _DownloadFile(photo->photo_->sizes_.at(maxSizeIndex)->photo_->id_);
    _MoveLocalFile(file->local_->path_, GetSenderName(std::move(sender)));
}
EagerRetType<> ObjectManager::DownloadResource(Utils::TdPtr<td::td_api::MessageSender> sender, Utils::TdPtr<td::td_api::messageVideo> video) {
    auto file = co_await _DownloadFile(video->video_->video_->id_);
    _MoveLocalFile(file->local_->path_, GetSenderName(std::move(sender)));
}

void ObjectManager::ProcessObject(Ptr_Object Message) {
    td::td_api::downcast_call(*Message,
        Utils::overloaded(
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
            // [this](td::td_api::updateChatFolders &Update_chat_folders) {
            //     std::ranges::for_each(Update_chat_folders.chat_folders_, [this](auto &folder) {
            //         folder_titles_[folder->id_] = folder->title_;
            //     });
            // },
            [this](td::td_api::updateNewMessage &Update_new_message) -> EagerNullRetType {
                // move to ptr to avoid destroy early
                auto newObj = Utils::Make<td::td_api::updateNewMessage>(std::move(Update_new_message));
                LogFormat::LogFormatter<LogFormat::Info>("recv UpdateNewMessage: {}", to_string(newObj->message_));

                switch (newObj->message_->content_->get_id()) {
                    case td::td_api::messageAnimation::ID: {
                        co_await DownloadResource(std::move(newObj->message_->sender_id_), Utils::MoveAs<td::td_api::messageAnimation>(newObj->message_->content_));
                        break;
                    }
                    case td::td_api::messageAudio::ID: {
                    }
                    case td::td_api::messagePaidMedia::ID: {
                    }
                    case td::td_api::messagePhoto::ID: {
                        co_await DownloadResource(std::move(newObj->message_->sender_id_), Utils::MoveAs<td::td_api::messagePhoto>(newObj->message_->content_));
                        break;
                    }
                    case td::td_api::messageVideo::ID: {
                        co_await DownloadResource(std::move(newObj->message_->sender_id_), Utils::MoveAs<td::td_api::messageVideo>(newObj->message_->content_));
                        break;
                    }
                    case td::td_api::messageVideoNote::ID: {
                    }
                    case td::td_api::messageVoiceNote::ID: {
                    }
                    case td::td_api::messageExpiredPhoto::ID: {
                    }
                    case td::td_api::messageExpiredVideo::ID: {
                    }
                    case td::td_api::messageExpiredVideoNote::ID: {
                    }
                    case td::td_api::messageExpiredVoiceNote::ID: {
                    }
                    case td::td_api::messageUnsupported::ID: {
                    }
                    default: break;
                }
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
            }));
}
