/*********************************************************************************
  *Copyright(C),Lwy
  *FileName:  json_exception.h
  *Author:  lwy
  *Version:  1.3
  *Date:  2022-03-23
  *Description:  此文件声明Exception异常类, 此类封装继承了logic_error
**********************************************************************************/

#ifndef JSON_JSON_EXCEPTION_H
#define JSON_JSON_EXCEPTION_H

#include <string>
#include <stdexcept>

namespace lwy {
    namespace json{
        class Exception final : public std::logic_error{
        public:
            explicit Exception(const std::string &errMsg) : logic_error(errMsg) { }
        };
    }
}

#endif //JSON_JSON_EXCEPTION_H
