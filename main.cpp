
#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <memory>
#include <td/telegram/td_api.hpp>

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <print>
#include <cassert>
#include <fstream>
#include <thread>
#include <algorithm>


// overloaded
namespace detail {
    template<class... Fs>
    struct overload;

    template<class F>
    struct overload<F> : public F {
        explicit overload(F f) : F(f) {
        }
    };

    template<class F, class... Fs>
    struct overload<F, Fs...>
            : public overload<F>, public overload<Fs...> {
        overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...) {
        }

        using overload<F>::operator();
        using overload<Fs...>::operator();
    };
}  // namespace detail

template<class... F>
auto overloaded(F... f) {
    return detail::overload<F...>(f...);
}

namespace td_api = td::td_api;

class TdExample {
public:
    TdExample() {
        td::ClientManager::execute(td_api::make_object<td_api::setLogVerbosityLevel>(2));
        client_manager_ = std::make_unique<td::ClientManager>();
        client_id_ = client_manager_->create_client_id();

        // 设置代理并且不需要判断是否失败，因为失败也处理不了
        auto addProxy = td_api::make_object<td_api::addProxy>();
        addProxy->server_ = "127.0.0.1";
        addProxy->port_ = 11223;
        addProxy->enable_ = true;
        addProxy->type_ = td_api::make_object<td_api::proxyTypeSocks5>();
        send_query(std::move(addProxy), {});


        send_query(td_api::make_object<td_api::getOption>("version"), {});

        ptr_fs = std::make_unique<std::fstream>("./log.txt",
                                                std::ios_base::out | std::ios_base::binary | std::ios_base::app);
    }

    void loop() {
        while (true) {
            if (need_restart_) {
                restart();
            } else if (!are_authorized_) {
                process_response(client_manager_->receive(10));
            } else {
                std::println("Enter action [q] quit [u] check for updates and request results "
                             "[c] show chats [m <chat_id> <text>] send message [me] show self [l] logout: ");

                std::string line;
                std::getline(std::cin, line);
                std::istringstream ss(line);
                std::string action;
                if (!(ss >> action)) {
                    continue;
                }
                if (action == "q") {
                    return;
                }
                if (action == "u") {
                    std::println("Requesting updates...");
                    while (true) {
                        auto response = client_manager_->receive(0);
                        if (response.object) {
                            process_response(std::move(response));
                        } else {
                            break;
                        }
                    }
                } else if (action == "close") {
                    std::println("Closing...");
                    send_query(td_api::make_object<td_api::close>(), {});
                } else if (action == "me") {
                    send_query(td_api::make_object<td_api::getMe>(), [this](Object object) {
                        std::println("Self: {}", to_string(object));
                    });
                } else if (action == "l") {
                    std::println("Logging out...");
                    send_query(td_api::make_object<td_api::logOut>(), {});
                } else if (action == "m") {
                    std::int64_t chat_id;
                    ss >> chat_id;
                    ss.get();
                    std::string text;
                    std::getline(ss, text);

                    std::println("Sending message to chat {}...", chat_id);
                    auto send_message = td_api::make_object<td_api::sendMessage>();
                    send_message->chat_id_ = chat_id;
                    auto message_content = td_api::make_object<td_api::inputMessageText>();
                    message_content->text_ = td_api::make_object<td_api::formattedText>();
                    message_content->text_->text_ = std::move(text);
                    send_message->input_message_content_ = std::move(message_content);

                    send_query(std::move(send_message), {});
                } else if (action == "c") {
                    std::println("Loading chat list...");
                    send_query(td_api::make_object<td_api::getChats>(nullptr, 20), [this](Object object) {
                        if (object->get_id() == td_api::error::ID) {
                            return;
                        }
                        auto chats = td::move_tl_object_as<td_api::chats>(object);
                        for (auto chat_id: chats->chat_ids_) {
                            std::println("[chat_id: {}] [title: {}]", chat_id, chat_title_[chat_id]);
                        }
                    });
                } else if (action == "test") {
                    std::println("Test:");

                    td_api::int53 chat_id = 0;
                    std::cin >> chat_id;
                    td_api::int64 lastMessageID = 0;

                    std::map<td_api::int53, td_api::int32> mapRecvMessages;

                    while (true) {
                        get_history_message(chat_id, lastMessageID, mapRecvMessages);
                        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    }
                }
            }
        }
    }

private:
    void get_history_message(
            td_api::int53 Chat_id,
            td_api::int64 &LastMessageID,
            std::map<td_api::int53, td_api::int32> &mapRecvMessages
    ) {
        auto obj_get_chat_history = td_api::make_object<td_api::getChatHistory>(
                Chat_id,
                LastMessageID,
                0,
                100,
                false
        );

        bool wait = true;
        send_query(std::move(obj_get_chat_history), [this, &LastMessageID, &wait, &mapRecvMessages](Object object) {
            // if recv error return
            if (object->get_id() == td_api::error::ID) {
                std::println("Error: {}", to_string(object));
                wait = false;
                return;
            }


            // if recv empty return
            auto messages = td::move_tl_object_as<td_api::messages>(object);
            if (messages->total_count_ == 0) {
                wait = false;
                return;
            }


            // print message count
            std::println("Got {} messages", messages->total_count_);


            // set last message id
            LastMessageID = messages->messages_.back()->id_;


            // collect message and save to ss
            // and save id_ and data_ to map
            std::stringstream ss;
            for (auto iterator = messages->messages_.begin(); iterator != messages->messages_.end(); ++iterator) {
                auto message = td::move_tl_object_as<td_api::message>(*iterator);

                // void collect repeat message
                if (mapRecvMessages.find(message->id_) != mapRecvMessages.end()) {
                    continue;
                }


                // get sender id and name
                std::string sender_name;
                td_api::int53 sender_id = 0;
                switch (message->sender_id_->get_id()) {
                    case td_api::messageSenderUser::ID: {
                        auto user = td_api::move_object_as<td_api::messageSenderUser>(message->sender_id_);
                        sender_name = get_user_name(user->user_id_);
                        sender_id = user->user_id_;
                        break;
                    }
                    case td_api::messageSenderChat::ID: {
                        auto chat = td_api::move_object_as<td_api::messageSenderChat>(message->sender_id_);
                        sender_name = get_chat_title(chat->chat_id_);
                        sender_id = chat->chat_id_;
                        break;
                    }
                }


                // get message text
                std::string message_text;
                switch (message->content_->get_id()) {
                    case td_api::messageText::ID: {
                        auto message_content = td_api::move_object_as<td_api::messageText>(message->content_);

                        // find and remove message_content's "\r\n"
                        size_t current_pos = 0;
                        while ((current_pos = message_content->text_->text_.find('\n', current_pos)) != std::string::npos) {
                            message_content->text_->text_.erase(current_pos, 1);
                        }

                        message_text = message_content->text_->text_;
                    }
                }


                // collect message
                ss << std::format("message id: {} timestamp: {} from: [{:<16}] {} \t: {}\r\n",
                                  message->id_,
                                  message->date_,
                                  sender_id,
                                  sender_name,
                                  message_text);
                mapRecvMessages[message->id_] = message->date_;
            }


            // write message to file
            if (ptr_fs->is_open()) {
                ptr_fs->write(ss.str().c_str(), ss.str().length());
                ptr_fs->flush();
            } else {
                ptr_fs->open("./log.txt", std::ios_base::app | std::ios_base::binary | std::ios_base::out);
            }


            // remove old message from map
            if (mapRecvMessages.size() > 5000) {
                std::vector<std::pair<td_api::int53, td_api::int32>> pairs;
                for (auto it = mapRecvMessages.begin(); it != mapRecvMessages.end(); ++it)
                    pairs.emplace_back(*it);

                std::sort(pairs.begin(), pairs.end(), [](auto a, auto b) {
                    return a.second < b.second;
                });

                pairs.erase(pairs.begin(), pairs.begin() + 2000);

                mapRecvMessages.clear();
                for (auto it = pairs.begin(); it != pairs.end(); ++it)
                    mapRecvMessages[it->first] = it->second;
            }


            // cancel wait
            wait = false;
        });

        while (wait) {
            auto response = client_manager_->receive(0);
            if (response.object) {
                process_response(std::move(response));
            } else {
                break;
            }
        }
    }

private:
    /// custom

    std::unique_ptr<std::fstream> ptr_fs;

    /// custom

    using Object = td_api::object_ptr<td_api::Object>;
    std::unique_ptr<td::ClientManager> client_manager_;
    std::int32_t client_id_{0};

    td_api::object_ptr<td_api::AuthorizationState> authorization_state_;
    bool are_authorized_{false};
    bool need_restart_{false};
    std::uint64_t current_query_id_{0};
    std::uint64_t authentication_query_id_{0};

    std::map<std::uint64_t, std::function<void(Object)>> handlers_;

    std::map<std::int64_t, td_api::object_ptr<td_api::user>> users_;

    std::map<std::int64_t, std::string> chat_title_;

    void restart() {
        client_manager_.reset();
        *this = TdExample();
    }

    void send_query(td_api::object_ptr<td_api::Function> f, std::function<void(Object)> handler) {
        auto query_id = next_query_id();
        if (handler) {
            handlers_.emplace(query_id, std::move(handler));
        }
        client_manager_->send(client_id_, query_id, std::move(f));
    }

    void process_response(td::ClientManager::Response response) {
        if (!response.object) {
            return;
        }

        if (response.request_id == 0) {
            return process_update(std::move(response.object));
        }
        auto it = handlers_.find(response.request_id);
        if (it != handlers_.end()) {
            it->second(std::move(response.object));
            handlers_.erase(it);
        }
    }

    [[nodiscard]] std::string get_user_name(std::int64_t user_id) const {
        auto it = users_.find(user_id);
        if (it == users_.end()) {
            return "unknown user";
        }
        return it->second->first_name_ + " " + it->second->last_name_;
    }

    [[nodiscard]] std::string get_chat_title(std::int64_t chat_id) const {
        auto it = chat_title_.find(chat_id);
        if (it == chat_title_.end()) {
            return "unknown chat";
        }
        return it->second;
    }

    void process_update(td_api::object_ptr<td_api::Object> update) {
        switch (update->get_id()) {
            case td_api::updateAuthorizationState::ID: {
                auto update_authorization_state = td_api::move_object_as<td_api::updateAuthorizationState>(update);
                authorization_state_ = std::move(update_authorization_state->authorization_state_);
                on_authorization_state_update();
                break;
            }
            case td_api::updateNewChat::ID: {
                auto update_new_chat = td_api::move_object_as<td_api::updateNewChat>(update);
                chat_title_[update_new_chat->chat_->id_] = update_new_chat->chat_->title_;
                break;
            }
            case td_api::updateChatTitle::ID: {
                auto update_chat_title = td_api::move_object_as<td_api::updateChatTitle>(update);
                chat_title_[update_chat_title->chat_id_] = update_chat_title->title_;
                break;
            }
            case td_api::updateUser::ID: {
                auto update_user = td_api::move_object_as<td_api::updateUser>(update);
                auto user_id = update_user->user_->id_;
                users_[user_id] = std::move(update_user->user_);
                break;
            }
            case td_api::updateNewMessage::ID: {
                auto update_new_message = td_api::move_object_as<td_api::updateNewMessage>(update);
                auto chat_id = update_new_message->message_->chat_id_;
                std::string sender_name;

                switch (update_new_message->message_->sender_id_->get_id()) {
                    case td_api::messageSenderUser::ID: {
                        auto user = td_api::move_object_as<td_api::messageSenderUser>(
                                update_new_message->message_->sender_id_);
                        sender_name = get_user_name(user->user_id_);
                        break;
                    }
                    case td_api::messageSenderChat::ID: {
                        auto chat = td_api::move_object_as<td_api::messageSenderChat>(
                                update_new_message->message_->sender_id_);
                        sender_name = get_chat_title(chat->chat_id_);
                        break;
                    }
                    default:
                        // 处理未知的消息发送者类型
                        break;
                }

                std::string text;
                if (update_new_message->message_->content_->get_id() == td_api::messageText::ID) {
                    text = static_cast<td_api::messageText &>(*update_new_message->message_->content_).text_->text_;
                }

                std::println("Receive message: [chat_id:{}][from:{}][{}]", chat_id, sender_name, text);

                break;
            }
            default:
                break;
        }
    }

    auto create_authentication_query_handler() {
        return [this, id = authentication_query_id_](Object object) {
            if (id == authentication_query_id_) {
                check_authentication_error(std::move(object));
            }
        };
    }

    void on_authorization_state_update() {
        authentication_query_id_++;
        td_api::downcast_call(*authorization_state_,
                              overloaded(
                                      [this](td_api::authorizationStateReady &) {
                                          are_authorized_ = true;
                                          std::println("Authorization is completed");
                                      },
                                      [this](td_api::authorizationStateLoggingOut &) {
                                          are_authorized_ = false;
                                          std::println("Logging out");
                                      },
                                      [this](td_api::authorizationStateClosing &) {
                                          std::println("Closing");
                                      },
                                      [this](td_api::authorizationStateClosed &) {
                                          are_authorized_ = false;
                                          need_restart_ = true;
                                          std::println("Terminated");
                                      },
                                      [this](td_api::authorizationStateWaitPhoneNumber &) {
                                          std::println("Enter phone number: ");
                                          std::string phone_number;
                                          std::cin >> phone_number;
                                          send_query(
                                                  td_api::make_object<td_api::setAuthenticationPhoneNumber>(
                                                          phone_number, nullptr),
                                                  create_authentication_query_handler());
                                      },
                                      [this](td_api::authorizationStateWaitEmailAddress &) {
                                          std::println("Enter email address: ");
                                          std::string email_address;
                                          std::cin >> email_address;
                                          send_query(
                                                  td_api::make_object<td_api::setAuthenticationEmailAddress>(email_address),
                                                  create_authentication_query_handler());
                                      },
                                      [this](td_api::authorizationStateWaitEmailCode &) {
                                          std::println("Enter email authentication code: ");
                                          std::string code;
                                          std::cin >> code;
                                          send_query(td_api::make_object<td_api::checkAuthenticationEmailCode>(
                                                             td_api::make_object<td_api::emailAddressAuthenticationCode>(code)),
                                                     create_authentication_query_handler());
                                      },
                                      [this](td_api::authorizationStateWaitCode &) {
                                          std::println("Enter authentication code: ");
                                          std::string code;
                                          std::cin >> code;
                                          send_query(td_api::make_object<td_api::checkAuthenticationCode>(code),
                                                     create_authentication_query_handler());
                                      },
                                      [this](td_api::authorizationStateWaitRegistration &) {
                                          std::string first_name;
                                          std::string last_name;
                                          std::println("Enter your first name: ");
                                          std::cin >> first_name;
                                          std::println("Enter your last name: ");
                                          std::cin >> last_name;
                                          send_query(td_api::make_object<td_api::registerUser>(first_name, last_name),
                                                     create_authentication_query_handler());
                                      },
                                      [this](td_api::authorizationStateWaitPassword &) {
                                          std::println("Enter authentication password: ");
                                          std::string password;
                                          std::getline(std::cin, password);
                                          send_query(td_api::make_object<td_api::checkAuthenticationPassword>(password),
                                                     create_authentication_query_handler());
                                      },
                                      [this](td_api::authorizationStateWaitOtherDeviceConfirmation &state) {
                                          std::println("Confirm this login link on another device: {}", state.link_);
                                      },
                                      [this](td_api::authorizationStateWaitTdlibParameters &) {
                                          auto request = td_api::make_object<td_api::setTdlibParameters>();
                                          request->database_directory_ = "tdlib";
                                          request->use_message_database_ = true;
                                          request->use_secret_chats_ = true;
                                          request->api_id_ = 14370505;
                                          request->api_hash_ = "7cbc707a62ce714074b6853d72b92da5";
                                          request->system_language_code_ = "en";
                                          request->device_model_ = "Desktop";
                                          request->application_version_ = "1.0";
                                          request->enable_storage_optimizer_ = true;
                                          send_query(std::move(request), create_authentication_query_handler());
                                      }));
    }

    void check_authentication_error(Object object) {
        if (object->get_id() == td_api::error::ID) {
            auto error = td::move_tl_object_as<td_api::error>(object);
            std::println("Error: {}", to_string(error));
            on_authorization_state_update();
        }
    }

    std::uint64_t next_query_id() {
        return ++current_query_id_;
    }
};


int main() {

    setbuf(stdout, nullptr);

    TdExample example;

    example.loop();


}
