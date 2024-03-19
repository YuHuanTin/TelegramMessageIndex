//
// Created by YuHuanTin on 2023/11/26.
//

#include "Functions.h"

#include <print>
#include <iostream>
#include <thread>
#include <sstream>
#include <filesystem>

#include "Parser/MessageParser.h"


auto default_text_parser(MessageParser *Parser, td::tl::unique_ptr<td::td_api::messageText> Text) {
    std::println("Receive message: [chat_id:{}, title:{}][from:{}][{}]",
                 Parser->get_chat_id(), Parser->get_chat_title(), Parser->get_sender_name(), Text->text_->text_);
}

auto default_photo_parser(MessageParser *Parser, td::tl::unique_ptr<td::td_api::file> File) {
    std::println("Receive photo message: [chat_id:{}, title:{}][from:{}]",
                 Parser->get_chat_id(), Parser->get_chat_title(), Parser->get_sender_name());
    // rename with send person name
    std::filesystem::path file_path(File->local_->path_);

    std::string str = Parser->get_sender_name();
    std::for_each(str.begin(), str.end(), [](char &c) {
        switch (c) {
            case '\\':
            case '/':
            case ':':
            case '*':
            case '?':
            case '"':
            case '<':
            case '>':
            case '|':
                c = '_';
                break;
            default:
                break;
        }
    });
    try {
        std::filesystem::rename(file_path, file_path.parent_path() /
                                           std::format("{}_{}{}", str, File->id_, file_path.extension().string()));
    } catch (std::exception &Exception) {
        std::println("Rename file error: {}", Exception.what());
    }
}

void Functions::func_history() {
    /// get chat id
    td::td_api::int53 chat_id = 0;
    std::cin >> chat_id;


    /// collect last message id
    td::td_api::int64 lastMessageID = 0;


    /// open log file
//    open_log_file(Core->ptr_fs);


    while (true) {
        auto obj_get_chat_history = td::td_api::make_object<td::td_api::getChatHistory>(
                chat_id,
                lastMessageID,
                0,
                100,
                false
        );

        bool wait = true;

        core_->send_query(std::move(obj_get_chat_history), [this, &lastMessageID, &wait](td::td_api::object_ptr<td::td_api::Object> object) {
            // if recv error return
            if (object->get_id() == td::td_api::error::ID) {
                std::println("Error: {}", to_string(object));
                wait = false;
                return;
            }


            // if recv empty return
            auto      messages = td::move_tl_object_as<td::td_api::messages>(object);
            if (messages->total_count_ == 0) {
                wait = false;
                return;
            }


            // print message count
            std::println("Got {} messages", messages->total_count_);


            // set last message id
            lastMessageID = messages->messages_.back()->id_;


            // collect message and save to ss
            std::stringstream ss;
            for (auto iterator = messages->messages_.begin(); iterator != messages->messages_.end(); ++iterator) {
                auto message = td::move_tl_object_as<td::td_api::message>(*iterator);


                auto messageParser = std::make_shared<MessageParser>(core_, std::move(message));
                messageParser->set_content_text(default_text_parser);
                messageParser->set_content_photo(default_photo_parser);


                // get message text
//                std::string message_text;
//                switch (message->content_->get_id()) {
//                    case td::td_api::messageText::ID: {
//                        auto message_content = td::td_api::move_object_as<td::td_api::messageText>(message->content_);
//
//                        // find and remove message_content's "\r\n"
//                        size_t current_pos = 0;
//                        while ((current_pos = message_content->text_->text_.find('\n', current_pos)) != std::string::npos) {
//                            message_content->text_->text_.erase(current_pos, 1);
//                        }
//
//                        message_text = message_content->text_->text_;
//                    }
//                }


                // collect message
//                ss << std::format("message id: {} timestamp: {} from: [{:<16}] {} \t: {}\r\n",
//                                  message->id_,
//                                  message->date_,
//                                  sender_id,
//                                  sender_name,
//                                  message_text);
            }


            // write message to file
//            if (!Core->ptr_fs->is_open()) {
//                open_log_file(Core->ptr_fs);
//            }
//            Core->ptr_fs->write(ss.str().c_str(), ss.str().length());
//            Core->ptr_fs->flush();


            // cancel wait
            wait = false;
        });

        while (wait) {
            core_->update();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void Functions::func_parse_update_message(td::tl::unique_ptr<td::td_api::message> Message) {
    if (this->message_parser_ != nullptr) {
        this->message_parser_(core_, std::move(Message));
        return;
    }

    // default message parser
    auto messageParser = std::make_shared<MessageParser>(core_, std::move(Message));
    messageParser->set_content_text(default_text_parser);
    messageParser->set_content_photo(default_photo_parser);
    messageParser->parse_content();
}

void Functions::func_spy_picture_by_id() {
    auto vecSpyUserIds = configServicePtr->read_lists(ProgramConfig::spy_picture_by_id_list);

    bool reInput = true;
    if (!vecSpyUserIds.empty()) {
        reInput = false;
        std::println("Current spy user id: ");
        for (auto &i: vecSpyUserIds) {
            std::println("{}", i);
        }

        std::println("Use this list for spy? (y/n)");
        std::string answer;
        std::cin >> answer;
        if (answer != "y" && answer != "Y") {
            reInput = true;
        }
    }

    while (reInput) {
        std::println("please input user id or 'break' to exit input");

        std::string userId;
        std::cin >> userId;
        if (userId == "break") {
            break;
        }
        vecSpyUserIds.push_back(std::move(userId));
    }

    configServicePtr->write_lists(ProgramConfig::spy_picture_by_id_list, vecSpyUserIds);
    configServicePtr->refresh();

    // loop
    auto old_message_parser = this->message_parser_;
    this->message_parser_ = [](TelegramClientCore *Core, td::tl::unique_ptr<td::td_api::message> Message) {
        auto messageParser = std::make_shared<MessageParser>(Core, std::move(Message));
        messageParser->set_content_photo([](MessageParser *Parser, td::tl::unique_ptr<td::td_api::file> File) {
            std::println("Receive photo message: [chat_id:{}, title:{}][from:{}] [sender_id:{}]",
                         Parser->get_chat_id(), Parser->get_chat_title(), Parser->get_sender_name(), Parser->get_sender_id());
            // rename with send person name
            std::filesystem::path file_path(File->local_->path_);

            std::string str = Parser->get_sender_name();
            std::for_each(str.begin(), str.end(), [](char &c) {
                switch (c) {
                    case '\\':
                    case '/':
                    case ':':
                    case '*':
                    case '?':
                    case '"':
                    case '<':
                    case '>':
                    case '|':
                        c = '_';
                        break;
                    default:
                        break;
                }
            });
            try {
                std::filesystem::rename(file_path, file_path.parent_path() /
                                                   std::format("{}_{}{}", str, File->id_, file_path.extension().string()));
            } catch (std::exception &Exception) {
                std::println("Rename file error: {}", Exception.what());
            }
        });
        messageParser->parse_content();
    };

    while (true) {
        core_->update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    this->message_parser_ = old_message_parser;
}

Functions::Functions(TelegramClientCore *Core, std::shared_ptr<ProgramConfig> ProgramConfig_)
        : core_(Core), configServicePtr(std::move(ProgramConfig_)) {}
