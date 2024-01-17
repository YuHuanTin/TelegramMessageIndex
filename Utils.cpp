//
// Created by AFETT on 2024/1/17.
//

#include "Utils.h"


#include <boost/locale.hpp>

namespace Utils {
    namespace CodeConvert {
        std::string utf8_to_gbk(const std::string &str) {
            return boost::locale::conv::from_utf(str, "gbk");
        }

    }
}