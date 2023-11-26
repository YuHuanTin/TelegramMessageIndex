//
// Created by YuHuanTin on 2023/11/26.
//

#include <print>
#include <iostream>
#include <thread>
#include <sstream>
#include "Functions.h"

namespace td_api = td::td_api;

void open_log_file(std::unique_ptr<std::fstream> &PtrLog) {
    PtrLog = std::make_unique<std::fstream>("./log.txt", std::ios::out | std::ios::app | std::ios::binary);
}

void Functions::get_history_messages(TelegramClientCore *Core) {
    std::println("history:");


    /// get chat id
    td_api::int53 chat_id = 0;
    std::cin >> chat_id;


    /// collect last message id
    td_api::int64 lastMessageID = 0;


    /// open log file
    open_log_file(Core->ptr_fs);


    while (true) {
        auto obj_get_chat_history = td_api::make_object<td_api::getChatHistory>(
                chat_id,
                lastMessageID,
                0,
                100,
                false
        );

        bool wait = true;

        Core->send_query(std::move(obj_get_chat_history), [Core, &lastMessageID, &wait](TelegramClientCore::Object object) {
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
            lastMessageID = messages->messages_.back()->id_;


            // collect message and save to ss
            // and save id_ and data_ to map
            std::stringstream ss;
            for (auto iterator = messages->messages_.begin(); iterator != messages->messages_.end(); ++iterator) {
                auto message = td::move_tl_object_as<td_api::message>(*iterator);


                // get sender id and name
                std::string sender_name;
                td_api::int53 sender_id = 0;
                switch (message->sender_id_->get_id()) {
                    case td_api::messageSenderUser::ID: {
                        auto user = td_api::move_object_as<td_api::messageSenderUser>(message->sender_id_);
                        sender_name = Core->get_user_name(user->user_id_);
                        sender_id = user->user_id_;
                        break;
                    }
                    case td_api::messageSenderChat::ID: {
                        auto chat = td_api::move_object_as<td_api::messageSenderChat>(message->sender_id_);
                        sender_name = Core->get_chat_title(chat->chat_id_);
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
            }


            // write message to file
            if (!Core->ptr_fs->is_open()) {
                open_log_file(Core->ptr_fs);
            }
            Core->ptr_fs->write(ss.str().c_str(), ss.str().length());
            Core->ptr_fs->flush();


            // cancel wait
            wait = false;
        });

        while (wait) {
            auto response = Core->client_manager_->receive(0);
            if (response.object) {
                Core->process_response(std::move(response));
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
