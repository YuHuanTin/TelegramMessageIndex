//
// Created by YuHuanTin on 2024/3/21.
//

#include "CoreWarpper.h"

std::unique_ptr<FunctionRegister> CoreWarpper::register_functions(std::shared_ptr<ProgramConfig> ConfigService) {
    auto registerdFunctions = std::make_unique<FunctionRegister>();

    registerdFunctions->mapFunc_[REGISTER::FUNCTION_NAME::FUNCTION_UPDATE]            = [ConfigService](TelegramClientCore *ptr) {
        std::println("Requesting updates...");
        ptr->update();

        Functions functions(ptr, ConfigService);
        while (!ptr->get_messages().empty()) {
            functions.func_parse_update_message(std::move(ptr->get_messages().front()));
            ptr->get_messages().pop();
        }
    };
    registerdFunctions->mapFunc_[REGISTER::FUNCTION_NAME::FUNCTION_CLOSE]             = [](TelegramClientCore *ptr) {
        std::println("Closing...");
        ptr->send_query(td::td_api::make_object<td::td_api::close>(), nullptr);
    };
    registerdFunctions->mapFunc_[REGISTER::FUNCTION_NAME::FUNCTION_ME]                = [](TelegramClientCore *ptr) {
        ptr->send_query(td::td_api::make_object<td::td_api::getMe>(), [](td::td_api::object_ptr<td::td_api::Object> object) {
            std::println("Self: {}", to_string(object));
        });
    };
    registerdFunctions->mapFunc_[REGISTER::FUNCTION_NAME::FUNCTION_LOGOUT]            = [](TelegramClientCore *ptr) {
        std::println("Logging out...");

        ptr->send_query(td::td_api::make_object<td::td_api::logOut>(), {});
    };
    registerdFunctions->mapFunc_[REGISTER::FUNCTION_NAME::FUNCTION_SEND_MESSAGE]      = [](TelegramClientCore *ptr) {
        std::int64_t      chat_id;
        std::stringstream ss;
        ss >> chat_id;
        ss.get();
        std::string text;
        std::getline(ss, text);

        std::println("Sending message to chat {}...", chat_id);
        auto send_message = td::td_api::make_object<td::td_api::sendMessage>();
        send_message->chat_id_ = chat_id;
        auto message_content = td::td_api::make_object<td::td_api::inputMessageText>();
        message_content->text_               = td::td_api::make_object<td::td_api::formattedText>();
        message_content->text_->text_        = std::move(text);
        send_message->input_message_content_ = std::move(message_content);

        ptr->send_query(std::move(send_message), {});
    };
    registerdFunctions->mapFunc_[REGISTER::FUNCTION_NAME::FUNCTION_GET_CHAT_LIST]     = [](TelegramClientCore *ptr) {
        std::println("Loading chat list...");
        ptr->send_query(td::td_api::make_object<td::td_api::getChats>(nullptr, 20), [ptr](td::td_api::object_ptr<td::td_api::Object> object) {
            if (object->get_id() == td::td_api::error::ID) {
                return;
            }
            auto      chats = td::move_tl_object_as<td::td_api::chats>(object);
            for (auto chat_id: chats->chat_ids_) {
                std::println("[chat_id: {}] [title: {}]", chat_id, ptr->get_chat_title(chat_id));
            }
        });
    };
    registerdFunctions->mapFunc_[REGISTER::FUNCTION_NAME::FUNCTION_HISTORY]           = [ConfigService](TelegramClientCore *ptr) {
        Functions functions(ptr, ConfigService);

        functions.func_history();
    };
    registerdFunctions->mapFunc_[REGISTER::FUNCTION_NAME::FUNCTION_SPY_PICTURE_BY_ID] = [ConfigService](TelegramClientCore *ptr) {
        Functions functions(ptr, ConfigService);

        functions.func_spy_picture_by_id();
    };

    return registerdFunctions;
}

CoreWarpper::CoreWarpper(std::shared_ptr<ProgramConfig> ConfigService) {
    this->telegramClientCore_ = std::make_unique<TelegramClientCore>(ConfigService, register_functions(ConfigService));
}

CoreWarpper::CoreWarpper(std::shared_ptr<ProgramConfig> ConfigService, int LogLevel) {
    this->telegramClientCore_ = std::make_unique<TelegramClientCore>(ConfigService, register_functions(ConfigService), LogLevel);
}

void CoreWarpper::set_socks5_proxy(const std::string &Proxy_Host, int32_t Proxy_Port) {
    // 设置代理并且不需要判断是否失败，因为失败也处理不了
    auto obj_add_proxy = td::td_api::make_object<td::td_api::addProxy>();
    obj_add_proxy->server_ = Proxy_Host;
    obj_add_proxy->port_   = Proxy_Port;
    obj_add_proxy->enable_ = true;
    obj_add_proxy->type_   = td::td_api::make_object<td::td_api::proxyTypeSocks5>();

    telegramClientCore_->send_query(std::move(obj_add_proxy), nullptr);
}

void CoreWarpper::loop() {
    telegramClientCore_->loop();
}


