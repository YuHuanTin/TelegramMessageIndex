//
// Created by YuHuanTin on 2023/11/26.
//

#ifndef TELEGRAMMESSAGEINDEX_TELEGRAMCLIENTCORE_H
#define TELEGRAMMESSAGEINDEX_TELEGRAMCLIENTCORE_H

#include <map>
#include <memory>
#include <fstream>
#include <functional>
#include <string_view>


#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>
#include "../Config/ProgramConfig.h"


class TelegramClientCore {
private:
    using Object = td::td_api::object_ptr<td::td_api::Object>;

    std::unique_ptr<td::ClientManager> client_manager_;
    std::int32_t                        client_id_{0};
    bool                                are_authorized_ = false;
    bool                                need_restart_   = false;
    std::uint64_t                       current_query_id_{0};
    std::uint64_t                       authentication_query_id_{0};

    td::td_api::object_ptr<td::td_api::AuthorizationState> authorization_state_;
    std::map<std::uint64_t, std::function<void(Object)>>   handlers_;

    std::map<std::int64_t, td::td_api::object_ptr<td::td_api::user>> users_;
    std::map<std::int64_t, std::string> chat_title_;

    /**
     * user defined
     */

    std::shared_ptr<ProgramConfig> configServicePtr;
public:

    explicit TelegramClientCore(std::shared_ptr<ProgramConfig> ProgramConfig_);

    void set_proxy_s5(std::string_view Proxy_Host, int32_t Proxy_Port);

    void loop();

    [[nodiscard]] std::string get_user_name(std::int64_t user_id) const;

    [[nodiscard]] std::string get_chat_title(std::int64_t chat_id) const;

    void send_query(td::td_api::object_ptr<td::td_api::Function> f, std::function<void(Object)> handler);

    void update();

private:

    void restart();

    void process_response(td::ClientManager::Response response);

    void process_update(td::td_api::object_ptr<td::td_api::Object> update);

    auto create_authentication_query_handler();

    void on_authorization_state_update();

    void check_authentication_error(Object object);

    std::uint64_t next_query_id();
};


#endif //TELEGRAMMESSAGEINDEX_TELEGRAMCLIENTCORE_H
