//
// Created by AFETT on 2024/9/16.
//

#pragma once

#include "StorageManager.h"

inline static StorageManager StorageManager;

template<typename DEFAULT_VALUE_TYPE = std::string>
class ConfigManager {
    std::string_view                  key_{};
    std::optional<DEFAULT_VALUE_TYPE> default_value_{};

public:
    ConfigManager() = delete;

    explicit ConfigManager(const std::string_view Key)
        : key_(Key) {}

    ConfigManager(const std::string_view Key, DEFAULT_VALUE_TYPE &&Default_value)
        : key_(Key), default_value_(Default_value) {}

    template<typename RETURN_TYPE>
    [[nodiscard]] RETURN_TYPE Value() {
        auto value = StorageManager.Read<RETURN_TYPE>(key_);
        if (!value && default_value_.has_value()) {
            // set default value to config
            Set_Value(default_value_.value());
            return default_value_.value();
        }
        return value.value();
    }

    template<typename SET_TYPE>
    void Set_Value(SET_TYPE &&Value) const {
        StorageManager.Write(std::string { key_ }, Value);
    }
};
