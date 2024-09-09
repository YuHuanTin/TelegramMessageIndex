#include <iostream>
#include <print>

#include <concurrencpp/concurrencpp.h>
#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include "TelegramCore.h"

#include "Utils/Logger.hpp"

TdClientCoreCo::TdClientCoreCo() {
    td::ClientManager::execute(Utils::Make<td::td_api::setLogVerbosityLevel>(1));
    client_manager_ = std::make_unique<td::ClientManager>();
    client_id_      = client_manager_->create_client_id();

    SendQuery(Utils::Make<td::td_api::getOption>("version"));
}

// todo 改进它
void TdClientCoreCo::Auth() {
    while (need_restart_ || !are_authorized_) {
        if (need_restart_) {
            restart();
        } else if (!are_authorized_) {
            LoopIt(10);
        }
    }
}

concurrencpp::result<TdClientCoreCo::Ptr_Object> TdClientCoreCo::SendQuery(Ptr_Function f) {
    auto query_id = next_query_id();
    LogFormat::LogFormatter<LogFormat::Debug>("->{}, {}", query_id, td::td_api::to_string(f));

    client_manager_->send(client_id_, query_id, std::move(f));

    // submit promise 
    concurrencpp::result_promise<Ptr_Object> promise;

    auto result            = promise.get_result();
    co_handlers_[query_id] = std::move(promise);
    return result;
}

concurrencpp::result<TdClientCoreCo::Ptr_Object> TdClientCoreCo::LoopIt(const double TimeOutSeconds) {
    auto &&[client_id, request_id, object] = this->client_manager_->receive(TimeOutSeconds);
    if (!object) {
        co_return nullptr; // maybe timeout
    }
    LogFormat::LogFormatter<LogFormat::Debug>("<-{}, {}", request_id, td::td_api::to_string(object));
    if (const auto it = co_handlers_.find(request_id);
        it != co_handlers_.end()) {
        it->second.set_result(std::move(object));
        co_handlers_.erase(it);
        co_return nullptr;
    }

    assert(request_id == 0 && "unexpected request_id");
    // 特殊处理 todo 尝试消灭它！
    if (request_id == 0 && object->get_id() == td::td_api::updateAuthorizationState::ID) {
        authorization_state_ = std::move(Utils::MoveAs<td::td_api::updateAuthorizationState>(object)->authorization_state_);
        on_authorization_state_update();
        co_return nullptr;
    }
    co_return std::move(object);
}

void TdClientCoreCo::restart() {
    client_manager_.reset();
    *this = TdClientCoreCo();
}

auto TdClientCoreCo::create_authentication_query_handler() {
    return [this, id = authentication_query_id_](Ptr_Object object) {
        if (id == authentication_query_id_) {
            check_authentication_error(std::move(object));
        }
    };
}

void TdClientCoreCo::on_authorization_state_update() {
    authentication_query_id_++;
    td::td_api::downcast_call(*authorization_state_,
        Utils::overloaded(
            [this](td::td_api::authorizationStateReady &) {
                are_authorized_ = true;
                LogFormat::LogFormatter<LogFormat::Info>("Authorization is completed, now recv control return back main loop");
            },
            [this](td::td_api::authorizationStateLoggingOut &) {
                are_authorized_ = false;
                LogFormat::LogFormatter<LogFormat::Info>("Logging out");
            },
            [this](td::td_api::authorizationStateClosing &) { LogFormat::LogFormatter<LogFormat::Info>("Closing"); },
            [this](td::td_api::authorizationStateClosed &) {
                are_authorized_ = false;
                need_restart_   = true;
                LogFormat::LogFormatter<LogFormat::Info>("Terminated");
            },
            [this](td::td_api::authorizationStateWaitPhoneNumber &) -> concurrencpp::result<void> {
                LogFormat::LogFormatter<LogFormat::Info>("Enter phone number: ");
                std::string phone_number;
                std::cin >> phone_number;

                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::setAuthenticationPhoneNumber>(phone_number, nullptr)));
            },
            [this](td::td_api::authorizationStateWaitEmailAddress &) -> concurrencpp::result<void> {
                LogFormat::LogFormatter<LogFormat::Info>("Enter email address: ");
                std::string email_address;
                std::cin >> email_address;
                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::setAuthenticationEmailAddress>(email_address)));
            },
            [this](td::td_api::authorizationStateWaitEmailCode &)-> concurrencpp::result<void> {
                LogFormat::LogFormatter<LogFormat::Info>("Enter email authentication code: ");
                std::string code;
                std::cin >> code;
                create_authentication_query_handler()(
                    co_await SendQuery(Utils::Make<td::td_api::checkAuthenticationEmailCode>(Utils::Make<td::td_api::emailAddressAuthenticationCode>(code))));
            },
            [this](td::td_api::authorizationStateWaitCode &)-> concurrencpp::result<void> {
                LogFormat::LogFormatter<LogFormat::Info>("Enter authentication code: ");
                std::string code;
                std::cin >> code;

                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::checkAuthenticationCode>(code)));
            },
            [this](td::td_api::authorizationStateWaitRegistration &) -> concurrencpp::result<void> {
                std::string first_name;
                std::string last_name;
                LogFormat::LogFormatter<LogFormat::Info>("Enter your first name: ");
                std::cin >> first_name;
                LogFormat::LogFormatter<LogFormat::Info>("Enter your last name: ");
                std::cin >> last_name;

                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::registerUser>(first_name, last_name, false)));
            },
            [this](td::td_api::authorizationStateWaitPassword &) -> concurrencpp::result<void> {
                LogFormat::LogFormatter<LogFormat::Info>("Enter authentication password: ");
                std::string password;
                std::getline(std::cin, password);

                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::checkAuthenticationPassword>(password)));
            },
            [this](td::td_api::authorizationStateWaitOtherDeviceConfirmation &state) {
                LogFormat::LogFormatter<LogFormat::Info>("Confirm this login link on another device: {}", state.link_);
            },
            [this](td::td_api::authorizationStateWaitTdlibParameters &)-> concurrencpp::result<void> {
                auto request                   = Utils::Make<td::td_api::setTdlibParameters>();
                request->database_directory_   = "tdlib";
                request->use_message_database_ = true;
                request->use_secret_chats_     = true;
                request->api_id_               = 94575;
                request->api_hash_             = "a3406de8d171bb422bb6ddf3bbd800e2";
                request->system_language_code_ = "en";
                request->device_model_         = "Desktop";
                request->application_version_  = "1.0";
                create_authentication_query_handler()(co_await SendQuery(std::move(request)));
            }));
}

void TdClientCoreCo::check_authentication_error(Ptr_Object object) {
    if (object->get_id() == td::td_api::error::ID) {
        const auto error = Utils::MoveAs<td::td_api::error>(object);
        LogFormat::LogFormatter<LogFormat::Debug>("Error: {}", td::td_api::to_string(error));
        on_authorization_state_update();
    }
}

std::uint64_t TdClientCoreCo::next_query_id() {
    return ++current_query_id_;
}
