//
// Created by YuHuanTin on 2023/11/26.
//

#include "Functions.h"

#include <print>
#include <iostream>
#include <thread>
#include <sstream>
#include <filesystem>

#include "../Parser/MessageParser.h"
#include "../Utils/ConsoleCtrlCapturer.h"

auto replace_illegal_characters(std::string &str) {
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
}

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
    replace_illegal_characters(str);

    try {
        std::filesystem::rename(file_path, file_path.parent_path() /
                                           std::format("{}_{}{}", str, File->id_, file_path.extension().string()));
    } catch (std::exception &Exception) {
        std::println("Rename file error: {}", Exception.what());
    }
}

Functions::Functions(TelegramClientCore *Core, std::shared_ptr<ProgramConfig> ConfigService)
        : core_(Core), configServicePtr(std::move(ConfigService)) {}

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
    // default message parser
    auto messageParser = std::make_shared<MessageParser>(core_, std::move(Message));
    messageParser->set_content_text(default_text_parser);
    messageParser->set_content_photo(default_photo_parser);
    messageParser->parse_content();
}

void print_spy_user_ids(const std::vector<std::string> &vecSpyUserIds) {
    std::cout << "Current spy user id: \n";
    for (const auto &i: vecSpyUserIds) {
        std::cout << i << '\n';
    }
}

bool confirm_use_list_for_spy() {
    std::cout << "Use this list for spy? (y/n)\n";
    std::string answer;
    std::cin >> answer;
    return (answer == "y" || answer == "Y");
}

void reinput_user_ids(std::vector<std::string> &vecSpyUserIds) {
    while (true) {
        std::cout << "please input user id or 'break' to exit input\n";
        std::string userId;
        std::cin >> userId;
        if (userId == "break") {
            break;
        }
        vecSpyUserIds.push_back(std::move(userId));
    }
}

void Functions::func_spy_picture_by_id() {
    // require spy user ids
    auto vecSpyUserIds = configServicePtr->read_lists(REGISTER::CONFIG_STRING_NAME::spy_picture_by_id_list);
    if (!vecSpyUserIds.empty()) {
        print_spy_user_ids(vecSpyUserIds);

        if (!confirm_use_list_for_spy()) {
            reinput_user_ids(vecSpyUserIds);
        }
    } else {
        reinput_user_ids(vecSpyUserIds);
    }

    // save config for spy user ids
    configServicePtr->write_lists(REGISTER::CONFIG_STRING_NAME::spy_picture_by_id_list, vecSpyUserIds);
    configServicePtr->refresh();


    auto message_progress = [&vecSpyUserIds](TelegramClientCore *Core, td::tl::unique_ptr<td::td_api::message> Message) {
        auto messageParser = std::make_shared<MessageParser>(Core, std::move(Message));
        messageParser->set_content_photo([&vecSpyUserIds](MessageParser *Parser, td::tl::unique_ptr<td::td_api::file> File) {
            // check if user is in spy list
            auto isContain = std::any_of(vecSpyUserIds.begin(), vecSpyUserIds.end(), [&](const std::string &i) {
                if (Parser->get_sender_id() == stoll(i)) {
                    return true;
                }
                return false;
            });
            if (!isContain) {
                return;
            }


            std::println("Receive photo message: [chat_id:{}, title:{}][from:{}] [sender_id:{}]",
                         Parser->get_chat_id(), Parser->get_chat_title(), Parser->get_sender_name(), Parser->get_sender_id());
            // rename with send person name
            std::filesystem::path file_path(File->local_->path_);

            std::string str = Parser->get_sender_name();
            replace_illegal_characters(str);

            try {
                std::filesystem::rename(file_path, file_path.parent_path() /
                                                   std::format("{}_{}{}", str, File->id_, file_path.extension().string()));
            } catch (std::exception &Exception) {
                std::println("Rename file error: {}", Exception.what());
            }
        });
        messageParser->parse_content();
    };

    // register ctrl c capture handler
    auto console_capturer_ptr = std::make_unique<ConsoleCtrlCapturer>();
    while (!console_capturer_ptr->is_capture_ctrl_c()) {
        // update get some messages
        core_->update();


        while (!core_->get_messages().empty()) {
            message_progress(core_, td::move_tl_object_as<td::td_api::message>(core_->get_messages().front()));
            core_->get_messages().pop();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
