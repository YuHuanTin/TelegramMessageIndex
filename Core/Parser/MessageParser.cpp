//
// Created by YuHuanTin on 2024/1/23.
//

#include "MessageParser.h"


#include <print>

MessageParser::MessageParser(TelegramClientCore *Core, td::tl::unique_ptr<td::td_api::message> Message)
        : core_(Core), message_(std::move(Message)) {

    // parse sender name/id
    switch (message_->sender_id_->get_id()) {
        case td::td_api::messageSenderUser::ID: {
            auto user = td::td_api::move_object_as<td::td_api::messageSenderUser>(message_->sender_id_);
            sender_name_ = Core->get_user_name(user->user_id_);
            sender_id_   = user->user_id_;
            break;
        }
        case td::td_api::messageSenderChat::ID: {
            auto chat    = td::td_api::move_object_as<td::td_api::messageSenderChat>(message_->sender_id_);
            sender_name_ = Core->get_chat_title(chat->chat_id_);
            sender_id_   = chat->chat_id_;
            break;
        }
        default:
            break;
    }
}


void MessageParser::parse_content() {
    // parse message content
    switch (message_->content_->get_id()) {
        case td::td_api::messageText::ID: {
            if (parse_text_ == nullptr) {
                break;
            }
            parse_text_(this, td::move_tl_object_as<td::td_api::messageText>(message_->content_));
            break;
        }
        case td::td_api::messagePhoto::ID: {
            if (parse_photo_ == nullptr) {
                break;
            }
            // send downlaod photo request
            auto               photo_content = td::move_tl_object_as<td::td_api::messagePhoto>(message_->content_);
            core_->send_query(td::td_api::make_object<td::td_api::downloadFile>(photo_content->photo_->sizes_.back()->photo_->id_, 32, 0, 0, true),
                              [shared_this   = shared_from_this()](td::td_api::object_ptr<td::td_api::Object> object) {
                                  if (object->get_id() == td::td_api::error::ID) {
                                      std::println("Error: {}", to_string(td::move_tl_object_as<td::td_api::error>(object)));
                                      return;
                                  }
                                  if (object == nullptr) {
                                      std::println("Error: object is nullptr");
                                      return;
                                  }

                                  shared_this->parse_photo_(shared_this.get(), td::move_tl_object_as<td::td_api::file>(object));
                              });
            break;
        }
        default:
            break;
    }
}

std::string MessageParser::get_sender_name() const { return sender_name_; }

std::string MessageParser::get_chat_title() const { return core_->get_chat_title(message_->chat_id_); }

td::td_api::int53 MessageParser::get_sender_id() const { return sender_id_; }

td::td_api::int53 MessageParser::get_chat_id() const { return message_->chat_id_; }





