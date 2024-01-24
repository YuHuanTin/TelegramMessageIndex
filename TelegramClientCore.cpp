//
// Created by YuHuanTin on 2023/11/26.
//

#include "TelegramClientCore.h"
#include "Functions.h"

#include <print>
#include <iostream>
#include <sstream>


namespace td_api = td::td_api;

TelegramClientCore::TelegramClientCore() {
    td::ClientManager::execute(td_api::make_object<td_api::setLogVerbosityLevel>(2));
    client_manager_ = std::make_unique<td::ClientManager>();
    client_id_ = client_manager_->create_client_id();

    send_query(td_api::make_object<td_api::getOption>("version"), {});
}

void TelegramClientCore::set_proxy_s5(std::string_view Proxy_Host, int32_t Proxy_Port) {
    // 设置代理并且不需要判断是否失败，因为失败也处理不了
    auto obj_add_proxy = td_api::make_object<td_api::addProxy>();
    obj_add_proxy->server_ = Proxy_Host;
    obj_add_proxy->port_ = Proxy_Port;
    obj_add_proxy->enable_ = true;
    obj_add_proxy->type_ = td_api::make_object<td_api::proxyTypeSocks5>();

    send_query(std::move(obj_add_proxy), {});
}

void TelegramClientCore::loop() {
    while (true) {
        if (need_restart_) {
            restart();
        } else if (!are_authorized_) {
            process_response(client_manager_->receive(10));
        } else {
            std::println("[q]: quit\n"
                         "[u]: check for updates and request results\n"
                         "[c]: show chats\n"
                         "[m <chat_id> <text>]: send message\n"
                         "[me]: show self\n"
                         "[l]: logout\n"
                         "[history]: get history message");

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

                update();
            } else if (action == "close") {
                std::println("Closing...");
                send_query(td_api::make_object<td_api::close>(), {});
            } else if (action == "me") {
                send_query(td_api::make_object<td_api::getMe>(), [](Object object) {
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
                message_content->text_        = td_api::make_object<td_api::formattedText>();
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
            } else if (action == "history") {
                Functions::func_history(this);
            }
        }
    }
}

void TelegramClientCore::restart() {
    client_manager_.reset();
    *this = TelegramClientCore();
}

std::string TelegramClientCore::get_user_name(std::int64_t user_id) const {
    auto it = users_.find(user_id);
    if (it == users_.end()) {
        return "unknown user";
    }
    return it->second->first_name_ + " " + it->second->last_name_;
}

std::string TelegramClientCore::get_chat_title(std::int64_t chat_id) const {
    auto it = chat_title_.find(chat_id);
    if (it == chat_title_.end()) {
        return "unknown chat";
    }
    return it->second;
}

void TelegramClientCore::send_query(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler) {
    auto query_id = next_query_id();
    if (handler) {
        handlers_.emplace(query_id, std::move(handler));
    }
    client_manager_->send(client_id_, query_id, std::move(f));
}

void TelegramClientCore::update() {
    while (true) {
        auto response = client_manager_->receive(0);
        if (!response.object)
            break;
        process_response(std::move(response));
    }
}

void TelegramClientCore::process_response(td::ClientManager::Response response) {
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

void TelegramClientCore::process_update(td::td_api::object_ptr<td::td_api::Object> update) {
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
            auto user_id    = update_user->user_->id_;
            users_[user_id] = std::move(update_user->user_);
            break;
        }
        case td_api::updateNewMessage::ID: {
            auto &&update_ref = td_api::move_object_as<td_api::updateNewMessage>(update);

            // 处理新消息
            Functions::parse_update_message(this, td_api::move_object_as<td::td_api::message>(update_ref->message_));

            break;
        }
        default:
            break;
    }
}

auto TelegramClientCore::create_authentication_query_handler() {
    return [this, id = authentication_query_id_](Object object) {
        if (id == authentication_query_id_) {
            check_authentication_error(std::move(object));
        }
    };
}

void TelegramClientCore::on_authorization_state_update() {
    authentication_query_id_++;
    switch (authorization_state_->get_id()) {
        case td_api::authorizationStateReady::ID:
            are_authorized_ = true;
            std::println("Authorization is completed");
            break;
        case td_api::authorizationStateLoggingOut::ID:
            are_authorized_ = false;
            std::println("Logging out");
            break;
        case td_api::authorizationStateClosing::ID:
            std::println("Closing");
            break;
        case td_api::authorizationStateClosed::ID:
            are_authorized_ = false;
            need_restart_ = true;
            std::println("Terminated");
            break;
        case td_api::authorizationStateWaitPhoneNumber::ID: {
            std::println("Enter phone number: ");
            std::string phone_number;
            std::cin >> phone_number;
            send_query(
                    td_api::make_object<td_api::setAuthenticationPhoneNumber>(phone_number, nullptr),
                    create_authentication_query_handler());
            break;
        }
        case td_api::authorizationStateWaitEmailAddress::ID: {
            std::println("Enter email address: ");
            std::string email_address;
            std::cin >> email_address;
            send_query(
                    td_api::make_object<td_api::setAuthenticationEmailAddress>(email_address),
                    create_authentication_query_handler());
            break;
        }
        case td_api::authorizationStateWaitEmailCode::ID: {
            std::println("Enter email authentication code: ");
            std::string code;
            std::cin >> code;
            send_query(td_api::make_object<td_api::checkAuthenticationEmailCode>(td_api::make_object<td_api::emailAddressAuthenticationCode>(code)),
                       create_authentication_query_handler());
            break;
        }
        case td_api::authorizationStateWaitCode::ID: {
            std::println("Enter authentication code: ");
            std::string code;
            std::cin >> code;
            send_query(td_api::make_object<td_api::checkAuthenticationCode>(code), create_authentication_query_handler());
            break;
        }
        case td_api::authorizationStateWaitRegistration::ID: {
            std::string first_name;
            std::string last_name;
            std::println("Enter your first name: ");
            std::cin >> first_name;
            std::println("Enter your last name: ");
            std::cin >> last_name;
            send_query(td_api::make_object<td_api::registerUser>(first_name, last_name), create_authentication_query_handler());
            break;
        }
        case td_api::authorizationStateWaitPassword::ID: {
            std::println("Enter authentication password: ");
            std::string password;
            std::getline(std::cin, password);
            send_query(td_api::make_object<td_api::checkAuthenticationPassword>(password), create_authentication_query_handler());
            break;
        }
        case td_api::authorizationStateWaitOtherDeviceConfirmation::ID: {
            auto state = td_api::move_object_as<td_api::authorizationStateWaitOtherDeviceConfirmation>(authorization_state_);
            std::println("Confirm this login link on another device: {}", state->link_);
            break;
        }
        case td_api::authorizationStateWaitTdlibParameters::ID: {
            auto request = td_api::make_object<td_api::setTdlibParameters>();
            request->database_directory_   = "tdlib";
            request->use_message_database_ = true;
            request->use_secret_chats_     = true;
            request->api_id_               = 14370505;
            request->api_hash_             = "7cbc707a62ce714074b6853d72b92da5";
            request->system_language_code_ = "en";
            request->device_model_         = "Desktop";
            request->application_version_  = "1.0";
            request->enable_storage_optimizer_ = true;
            send_query(std::move(request), create_authentication_query_handler());
            break;
        }
        default:
            break;
    }
}

void TelegramClientCore::check_authentication_error(TelegramClientCore::Object object) {
    if (object->get_id() == td_api::error::ID) {
        auto error = td::move_tl_object_as<td_api::error>(object);
        std::println("Error: {}", to_string(error));
        on_authorization_state_update();
    }
}

std::uint64_t TelegramClientCore::next_query_id() {
    return ++current_query_id_;
}


