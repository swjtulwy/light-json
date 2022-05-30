/*********************************************************************************
  *Copyright(C),Lwy
  *FileName:  json_parser.h
  *Author:  lwy
  *Version:  1.3
  *Date:  2022-03-23
  *Description:  此文件声明Json解析器类Parser, 用于解析JSON串中的各种符号
  *Function List:
  * Parser类主要成员函数功能:
     1. Parser(Value &val, const std::string &content);
            构造函数, 传入Value初始化成员变量val_, 将解析的C++对象放入val
     2. void parse_whitespace() noexcept;
            解析空白符号: 空格, 制表符, 换行, 回车, 解析完这类字符会移动当前解析的指针位置cur
     3. void parse_value();
            解析各种类型值的分发入口,通过遇到的第一个字符来判断要解析的值类型
     4. void parse_literal(const char *literal, json::type t);
            解析字面值: Null, True, False
     5. void parse_number();
            解析double类型数字
     6. void parse_string();
            解析普通字符串
     7. void parse_string_raw(std::string &tmp);
            解析原始字符串
     8. void parse_hex4(const char* &p, unsigned &u);
            解析16进制数字
     9. void parse_encode_utf8(std::string &s, unsigned u) const noexcept;
            解析utf8编码字符
     10. void parse_array();
            解析数组类型
     11. void parse_object();
            递归解析JSON对象
**********************************************************************************/

#ifndef JSON_JSON_PARSER_H
#define JSON_JSON_PARSER_H

#include "json.h"
#include "json_value.h"

namespace lwy {

    namespace json {

        class Parser final{
        public:
            Parser(Value &val, const std::string &content);
        private:
            void parse_whitespace() noexcept;
            void parse_value();
            void parse_literal(const char *literal, json::type t);
            void parse_number();
            void parse_string();
            void parse_string_raw(std::string &tmp);
            void parse_hex4(const char* &p, unsigned &u);
            void parse_encode_utf8(std::string &s, unsigned u) const noexcept;
            void parse_array();
            void parse_object();

            Value &val_;
            const char *cur_;
        };

    }

}

#endif //JSON_JSON_PARSER_H
