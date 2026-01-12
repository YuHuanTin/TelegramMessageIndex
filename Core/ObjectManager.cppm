
module;

#include "td/telegram/td_api.h"


export module ObjectManager;

import std;
import TelegramCore;
import TdUtils;
import Logger;

export class ObjectManager {
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


    std::string _GetUserName(UserId User_id);

    std::string _GetChatTitle(ChatId Chat_id);

    EagerRetType<Ptr_File> _DownloadFile(const FileId File_id);

    void _MoveLocalFile(std::string &localPath, const std::string &senderName);

public:
    explicit ObjectManager(TdClientCore &TdClient);

    std::string GetSenderName(Utils::TdPtr<td::td_api::MessageSender> senderId);

    EagerRetType<> DownloadResource(Utils::TdPtr<td::td_api::MessageSender> sender, Utils::TdPtr<td::td_api::messageAnimation> animation);
    EagerRetType<> DownloadResource(Utils::TdPtr<td::td_api::MessageSender> sender, Utils::TdPtr<td::td_api::messagePhoto> photo);
    EagerRetType<> DownloadResource(Utils::TdPtr<td::td_api::MessageSender> sender, Utils::TdPtr<td::td_api::messageVideo> video);

    void ProcessObject(Ptr_Object Message);
};
