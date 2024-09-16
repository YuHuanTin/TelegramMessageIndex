//
// Created by AFETT on 2024/9/16.
//

#pragma once

template<typename ReturnType>
std::string ReadFile(const std::string &FilePath, const bool CreateIfNotExist = true) {
    std::fstream fs(FilePath, std::ios_base::in | std::ios_base::binary);
    //不存在则创建
    if (!fs.is_open() && CreateIfNotExist)
        fs.open(FilePath, std::ios_base::out | std::ios_base::binary);
    //还读取不了则返回空
    if (!fs.is_open())
        throw std::runtime_error("can't open file, maybe not exist or permission denied");
    //获取文件大小
    fs.seekg(0, std::ios_base::end);
    const auto fileSize = fs.tellg();
    fs.seekg(0, std::ios_base::beg);
    //初始化返回容器
    ReturnType content;
    content.resize(fileSize);
    //读取内容
    fs.read(std::bit_cast<char *>(content.data()), fileSize);
    fs.close();
    return content;
}

template<typename InputType>
bool WriteFile(const std::string &FilePath, InputType &&content) {
    std::fstream fs(FilePath, std::ios_base::out | std::ios_base::binary);
    if (!fs.is_open())
        return false;

    fs.write(std::bit_cast<const char *>(content.data()), content.size());
    fs.close();
    return true;
}

class StorageManager {
    std::string    file_name_ { REGISTER::STRING_POOL::normal_config_file_name };
    nlohmann::json configJson_ = {};

public:
    StorageManager() {
        LoadConfigJsonFromFile();
    }

    void LoadConfigJsonFromFile() {
        auto context = ReadFile<std::string>(file_name_);
        if (context.empty())
            return;
        configJson_ = nlohmann::json::parse(context);
    }

    void SaveConfigJsonToFile() const {
        WriteFile(file_name_, configJson_.dump(4));
    }

    template<typename T>
    std::optional<T> Read(const std::string_view Key) {
        if (configJson_.contains(Key)) {
            return configJson_.at(Key).get<T>();
        }
        return std::nullopt;
    }

    template<typename T>
    void Write(const std::string &Key, T &&Value) {
        configJson_[Key] = Value;
    }

    ~StorageManager() {
        SaveConfigJsonToFile();
    }
};
