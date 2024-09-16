
#pragma once

#include "Utils/TdUtils.hpp"

class TdClientCore {
    using Ptr_Object             = Utils::TdPtr<td::td_api::Object>;
    using Ptr_Function           = Utils::TdPtr<td::td_api::Function>;
    using Ptr_AuthorizationState = Utils::TdPtr<td::td_api::AuthorizationState>;

    std::unordered_map<uint64_t, concurrencpp::result_promise<Ptr_Object> > co_handlers_;

public:
    explicit TdClientCore();

    void Auth();

    /**
     * 发送请求并且返回 result 对象
     * @param f 发送的请求函数
     * @return concurrencpp::result 对象
     */
    concurrencpp::result<Ptr_Object> SendQuery(Ptr_Function f);

    concurrencpp::result<Ptr_Object> LoopIt(double TimeOutSeconds);

private:
    std::unique_ptr<td::ClientManager> client_manager_;
    std::int32_t                       client_id_ { 0 };

    Ptr_AuthorizationState authorization_state_;
    bool                   are_authorized_ { false };
    bool                   need_restart_ { false };
    std::uint64_t          current_query_id_ { 0 };
    std::uint64_t          authentication_query_id_ { 0 };

    void restart();

    auto create_authentication_query_handler();

    void on_authorization_state_update();

    void check_authentication_error(Ptr_Object object);

    std::uint64_t next_query_id();
};
