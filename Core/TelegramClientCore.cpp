//
// Created by YuHuanTin on 2024/3/21.
//


#include "TelegramClientCore.h"


void TelegramClientCore::inner_init() {
    // 创建客户端
    client_manager_ = std::make_unique<td::ClientManager>();
    client_id_      = client_manager_->create_client_id();

    // 发送查询
    send_query(td::td_api::make_object<td::td_api::getOption>("version"), {});
}

TelegramClientCore::TelegramClientCore(std::shared_ptr<ProgramConfig> ProgramConfig_, std::unique_ptr<FunctionRegister> RegisterdFunc)
    : configServicePtr_(std::move(ProgramConfig_)),
      registerdFunctions_(std::move(RegisterdFunc)) {
    this->inner_init();
}

TelegramClientCore::TelegramClientCore(std::shared_ptr<ProgramConfig> ProgramConfig_, std::unique_ptr<FunctionRegister> RegisterdFunc, int LogLevel)
    : configServicePtr_(std::move(ProgramConfig_)),
      registerdFunctions_(std::move(RegisterdFunc)),
      logLevelOption_(true, LogLevel) {
    assert(LogLevel >= 0 && LogLevel <= 1024 && "log level must be between 0 and 1024");

    // 设置日志等级
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(LogLevel));

    this->inner_init();
}

void TelegramClientCore::loop() {
    while (true) {
        if (need_restart_) {
            restart();
        } else if (!are_authorized_) {
            process_response(client_manager_->receive(10));
        } else {
            this->registerdFunctions_->displayCommands();

            std::string line;
            std::getline(std::cin, line);
            std::istringstream ss(line);
            std::string        action;
            if (!(ss >> action)) {
                continue;
            }

            if (action == "q") {
                return;
            }
            if (auto it = this->registerdFunctions_->mapFuncStr_.find(action);
                it != this->registerdFunctions_->mapFuncStr_.end()) {
                this->registerdFunctions_->mapFunc_[it->second](this);
            } else {
                std::println("unknown action: {}", action);
            }
        }
    }
}

std::queue<td::td_api::object_ptr<td::td_api::message> > &TelegramClientCore::get_messages() {
    return messages_;
}

void TelegramClientCore::restart() {
    client_manager_.reset();

    // 检查是否设置了日志等级
    if (logLevelOption_.first) {
        *this = TelegramClientCore(configServicePtr_, std::move(registerdFunctions_), logLevelOption_.second);
    } else {
        *this = TelegramClientCore(configServicePtr_, std::move(registerdFunctions_));
    }
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

void TelegramClientCore::send_query(Function f, std::function<void(Object)> handler) {
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
        case td::td_api::updateAuthorizationState::ID: {
            auto update_authorization_state = td::td_api::move_object_as<td::td_api::updateAuthorizationState>(update);
            authorization_state_            = std::move(update_authorization_state->authorization_state_);
            on_authorization_state_update();
            break;
        }
        case td::td_api::updateNewChat::ID: {
            auto update_new_chat                     = td::td_api::move_object_as<td::td_api::updateNewChat>(update);
            chat_title_[update_new_chat->chat_->id_] = update_new_chat->chat_->title_;
            break;
        }
        case td::td_api::updateChatTitle::ID: {
            auto update_chat_title                   = td::td_api::move_object_as<td::td_api::updateChatTitle>(update);
            chat_title_[update_chat_title->chat_id_] = update_chat_title->title_;
            break;
        }
        case td::td_api::updateUser::ID: {
            auto update_user = td::td_api::move_object_as<td::td_api::updateUser>(update);
            auto user_id     = update_user->user_->id_;
            users_[user_id]  = std::move(update_user->user_);
            break;
        }
        case td::td_api::updateNewMessage::ID: {
            auto &&update_ref = td::td_api::move_object_as<td::td_api::updateNewMessage>(update);

            // 保存新消息到 FIFO 队列中
            this->messages_.push(td::td_api::move_object_as<td::td_api::message>(update_ref->message_));
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
        case td::td_api::authorizationStateReady::ID:
            are_authorized_ = true;
            std::println("Authorization is completed");
            break;
        case td::td_api::authorizationStateLoggingOut::ID:
            are_authorized_ = false;
            std::println("Logging out");
            break;
        case td::td_api::authorizationStateClosing::ID:
            std::println("Closing");
            break;
        case td::td_api::authorizationStateClosed::ID:
            are_authorized_ = false;
            need_restart_ = true;
            std::println("Terminated");
            break;
        case td::td_api::authorizationStateWaitPhoneNumber::ID: {
            /**
             * 这里使用配置项储存最近登录的手机号码
             */

            auto require_use_last_login_phone_number = [](auto &&Last_login_phone_number) {
                std::println("Use last login phone number: {} ?(y/n)", Last_login_phone_number);
                char use_last_login_phone_number;
                std::cin >> use_last_login_phone_number;
                return use_last_login_phone_number == 'y' || use_last_login_phone_number == 'Y';
            };

            auto last_login_phone_number = this->configServicePtr_->read(REGISTER::CONFIG_STRING_NAME::last_login_phone_number);
            if (!last_login_phone_number.empty() && require_use_last_login_phone_number(last_login_phone_number)) {
                send_query(td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(last_login_phone_number, nullptr),
                    create_authentication_query_handler());
                break;
            }
            std::println("Enter phone number: ");
            std::string phone_number;
            std::cin >> phone_number;

            this->configServicePtr_->write(REGISTER::CONFIG_STRING_NAME::last_login_phone_number, phone_number);
            this->configServicePtr_->refresh();

            send_query(td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>(phone_number, nullptr),
                create_authentication_query_handler());
            break;
        }
        case td::td_api::authorizationStateWaitEmailAddress::ID: {
            std::println("Enter email address: ");
            std::string email_address;
            std::cin >> email_address;
            send_query(
                td::td_api::make_object<td::td_api::setAuthenticationEmailAddress>(email_address),
                create_authentication_query_handler());
            break;
        }
        case td::td_api::authorizationStateWaitEmailCode::ID: {
            std::println("Enter email authentication code: ");
            std::string code;
            std::cin >> code;
            send_query(td::td_api::make_object<td::td_api::checkAuthenticationEmailCode>(
                    td::td_api::make_object<td::td_api::emailAddressAuthenticationCode>(code)),
                create_authentication_query_handler());
            break;
        }
        case td::td_api::authorizationStateWaitCode::ID: {
            std::println("Enter authentication code: ");
            std::string code;
            std::cin >> code;
            send_query(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code), create_authentication_query_handler());
            break;
        }
        case td::td_api::authorizationStateWaitRegistration::ID: {
            std::string first_name;
            std::string last_name;
            std::println("Enter your first name: ");
            std::cin >> first_name;
            std::println("Enter your last name: ");
            std::cin >> last_name;
            send_query(td::td_api::make_object<td::td_api::registerUser>(first_name, last_name), create_authentication_query_handler());
            break;
        }
        case td::td_api::authorizationStateWaitPassword::ID: {
            std::println("Enter authentication password: ");
            std::string password;
            std::getline(std::cin, password);
            send_query(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password), create_authentication_query_handler());
            break;
        }
        case td::td_api::authorizationStateWaitOtherDeviceConfirmation::ID: {
            auto state = td::td_api::move_object_as<td::td_api::authorizationStateWaitOtherDeviceConfirmation>(authorization_state_);
            std::println("Confirm this login link on another device: {}", state->link_);
            break;
        }
        case td::td_api::authorizationStateWaitTdlibParameters::ID: {
            auto request                       = td::td_api::make_object<td::td_api::setTdlibParameters>();
            request->database_directory_       = "tdlib";
            request->use_message_database_     = true;
            request->use_secret_chats_         = true;
            request->api_id_                   = 14370505;
            request->api_hash_                 = "7cbc707a62ce714074b6853d72b92da5";
            request->system_language_code_     = "en";
            request->device_model_             = "Desktop";
            request->application_version_      = "1.0";
            request->enable_storage_optimizer_ = true;
            send_query(std::move(request), create_authentication_query_handler());
            break;
        }
        default:
            break;
    }
}

void TelegramClientCore::check_authentication_error(TelegramClientCore::Object object) {
    if (object->get_id() == td::td_api::error::ID) {
        auto error = td::move_tl_object_as<td::td_api::error>(object);
        std::println("Error: {}", to_string(error));
        on_authorization_state_update();
    }
}

std::uint64_t TelegramClientCore::next_query_id() {
    return ++current_query_id_;
}


