/*********************************************************************************
  *Copyright(C),Lwy
  *FileName:  json_generator.h
  *Author:  lwy
  *Version:  1.3
  *Date:  2022-03-23
  *Description:  此文件声明Generator生成器类, 此类用于将一个Json对象的C++值生成为字符串
  *Function List:
  * Generator类主要成员函数功能:
     1. Generator(const Value& val, std::string &result);
            构造函数,将Value值传入,result 接收返回字符串
     2. void stringify_value(const Value &v);
            将v对应的值序列化为字符串, 存入成员变量res_
     3. void stringify_string(const std::string &str);
            处理str对应的字符串,包括将转移字符过滤转化等
**********************************************************************************/

#ifndef JSON_JSON_GENERATOR_H
#define JSON_JSON_GENERATOR_H

#include "json_value.h"

namespace lwy {

    namespace json {

        class Generator final{
        public:
            Generator(const Value& val, std::string &result);
        private:
            void stringify_value(const Value &v);
            void stringify_string(const std::string &str);

            std::string &res_;
        };

    }

}

#endif //JSON_JSON_GENERATOR_H
