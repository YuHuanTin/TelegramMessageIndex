//
// Created by YuHuanTin on 2024/1/23.
//

#ifndef TELEGRAMMESSAGEINDEX_MESSAGEPARSER_H
#define TELEGRAMMESSAGEINDEX_MESSAGEPARSER_H

#include "td/telegram/td_api.h"
#include "td/telegram/td_api.hpp"
#include "../TelegramClientCore.h"


class MessageParser : public std::enable_shared_from_this<MessageParser> {
private:
    TelegramClientCore *core_;

    td::tl::unique_ptr<td::td_api::message> message_;

    // message sender
    [[maybe_unused]] td::td_api::int53                                    sender_id_;
    std::string                                                           sender_name_;

    // message content parse function
    std::function<void(td::tl::unique_ptr<td::td_api::messageText> Text)> parse_text_;
    std::function<void(td::tl::unique_ptr<td::td_api::file>)>             parse_photo_;

public:
    explicit MessageParser(TelegramClientCore *Core, td::tl::unique_ptr<td::td_api::message> Message);

    [[nodiscard]] std::string get_sender_name() const;

    [[nodiscard]] std::string get_chat_title() const;

    [[nodiscard]] td::td_api::int53 get_chat_id() const;

    [[maybe_unused]][[nodiscard]] td::td_api::int53 get_sender_id() const;

    template<typename PRED>
    MessageParser &set_content_text(PRED &&Predicate) {
        parse_text_ = std::forward<PRED>(Predicate);
        return *this;
    }

    template<typename PRED>
    MessageParser &set_content_photo(PRED &&Predicate) {
        parse_photo_ = std::forward<PRED>(Predicate);
        return *this;
    }

    void parse_content();
};


#endif //TELEGRAMMESSAGEINDEX_MESSAGEPARSER_H
