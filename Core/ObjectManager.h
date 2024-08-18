//
// Created by AFETT on 2024/8/18.
//

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <print>
#include <algorithm>
#include <filesystem>
#include <unordered_map>

#include <nameof.hpp>
#include <concurrencpp/concurrencpp.h>
#include <td/telegram/td_api.hpp>

#include "TelegramCore.h"
#include "Utils/TdUtils.hpp"

class ObjectManager {
    using Ptr_Object  = Utils::TdPtr<td::td_api::Object>;
    using Ptr_User    = Utils::TdPtr<td::td_api::user>;
    using Ptr_File    = Utils::TdPtr<td::td_api::file>;
    using ChatTitle   = std::string;
    using ChatId      = int64_t;
    using UserId      = int64_t;
    using FolderId    = int32_t;
    using FileId      = int32_t;
    using FolderTitle = std::string;

    std::unordered_map<ChatId, ChatTitle>     chat_titles_;
    std::unordered_map<UserId, Ptr_User>      users_;
    std::unordered_map<FolderId, FolderTitle> folder_titles_;

    TdClientCoreCo &td_client_;

public:
    explicit ObjectManager(TdClientCoreCo &TdClient);;

    std::string GetUserName(const UserId User_id);

    std::string GetChatTitle(const ChatId Chat_id);

    void ProcessObject(Ptr_Object Message);
};


#endif //OBJECTMANAGER_H
