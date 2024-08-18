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
#ifdef _DEBUG_LOG
    LogFormat::LogFormatter<LogFormat::Debug>("->{}, {}", query_id, td::td_api::to_string(f));
#endif
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
        LogFormat::LogFormatter<LogFormat::Debug>("may be timeout, client_id: {}, request_id: {}", client_id, request_id);
        co_return nullptr; // maybe timeout
    }
#ifdef _DEBUG_LOG
    LogFormat::LogFormatter<LogFormat::Debug>("<-{}, {}", request_id, td::td_api::to_string(object));
#endif

    if (const auto it = co_handlers_.find(request_id);
        it != co_handlers_.end()) {
        it->second.set_result(std::move(object));
        co_handlers_.erase(it);
        co_return nullptr;
    }

    assert(request_id == 0 && "unexpected request_id");
    // 特殊处理
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
                std::println("Authorization is completed, now recv control return back main loop");
            },
            [this](td::td_api::authorizationStateLoggingOut &) {
                are_authorized_ = false;
                std::println("Logging out");
            },
            [this](td::td_api::authorizationStateClosing &) { std::println("Closing"); },
            [this](td::td_api::authorizationStateClosed &) {
                are_authorized_ = false;
                need_restart_   = true;
                std::println("Terminated");
            },
            [this](td::td_api::authorizationStateWaitPhoneNumber &) -> concurrencpp::result<void> {
                std::println("Enter phone number: ");
                std::string phone_number;
                std::cin >> phone_number;

                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::setAuthenticationPhoneNumber>(phone_number, nullptr)));
            },
            [this](td::td_api::authorizationStateWaitEmailAddress &) -> concurrencpp::result<void> {
                std::println("Enter email address: ");
                std::string email_address;
                std::cin >> email_address;
                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::setAuthenticationEmailAddress>(email_address)));
            },
            [this](td::td_api::authorizationStateWaitEmailCode &)-> concurrencpp::result<void> {
                std::println("Enter email authentication code: ");
                std::string code;
                std::cin >> code;
                create_authentication_query_handler()(
                    co_await SendQuery(Utils::Make<td::td_api::checkAuthenticationEmailCode>(Utils::Make<td::td_api::emailAddressAuthenticationCode>(code))));
            },
            [this](td::td_api::authorizationStateWaitCode &)-> concurrencpp::result<void> {
                std::println("Enter authentication code: ");
                std::string code;
                std::cin >> code;

                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::checkAuthenticationCode>(code)));
            },
            [this](td::td_api::authorizationStateWaitRegistration &) -> concurrencpp::result<void> {
                std::string first_name;
                std::string last_name;
                std::println("Enter your first name: ");
                std::cin >> first_name;
                std::println("Enter your last name: ");
                std::cin >> last_name;

                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::registerUser>(first_name, last_name, false)));
            },
            [this](td::td_api::authorizationStateWaitPassword &) -> concurrencpp::result<void> {
                std::println("Enter authentication password: ");
                std::string password;
                std::getline(std::cin, password);

                create_authentication_query_handler()(co_await SendQuery(Utils::Make<td::td_api::checkAuthenticationPassword>(password)));
            },
            [this](td::td_api::authorizationStateWaitOtherDeviceConfirmation &state) {
                std::println("Confirm this login link on another device: {}", state.link_);
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
        std::println("Error: {}", td::td_api::to_string(error));
        on_authorization_state_update();
    }
}

std::uint64_t TdClientCoreCo::next_query_id() {
    return ++current_query_id_;
}
