//
// Created by AFETT on 2024/8/18.
//

#pragma once


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

    TdClientCore &td_client_;

public:
    explicit ObjectManager(TdClientCore &TdClient);;

    std::string GetUserName(UserId User_id);

    std::string GetChatTitle(ChatId Chat_id);

    void ProcessObject(Ptr_Object Message);
};

