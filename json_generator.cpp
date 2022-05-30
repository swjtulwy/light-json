/*********************************************************************************
  *Copyright(C),Lwy
  *FileName:  json_generator.cpp
  *Author:  lwy
  *Version:  1.3
  *Date:  2022-03-23
  *Description:  此文件实现Generator类
**********************************************************************************/

#include "json_generator.h"

namespace lwy {

    namespace json {

        Generator::Generator(const Value &val, std::string &result) : res_(result) {
            res_.clear();
            stringify_value(val);
        }

        // 递归输出为JSON串
        void Generator::stringify_value(const Value &v) {
            switch (v.get_type()) {
                case json::Null: res_ += "null";  break;
                case json::True: res_ += "true";  break;
                case json::False: res_ += "false"; break;
                case json::Number: {
                    // case语句里不能定义局部变量，要用代码块表示
                    char buffer[32] = {0};
                    // %g 自动选择合适的表示法, 小数点后17位
                    sprintf(buffer, "%.17g", v.get_number());
                    res_ += buffer;
                }
                    break;
                case json::String:
                    stringify_string(v.get_string());
                    break;
                case json::Array:
                    res_ += '[';
                    for (size_t i = 0; i < v.get_array_size(); ++i) {
                        if (i > 0) res_ += ',';
                        stringify_value(v.get_array_element(i));
                    }
                    res_ += ']';
                    break;
                case json::Object:
                    res_ += '{';
                    for (size_t i = 0; i < v.get_object_size(); ++i) {
                        if (i > 0) res_ += ',';
                        stringify_string(v.get_object_key(i));
                        res_ += ':';
                        stringify_value(v.get_object_value(i));
                    }
                    res_ += '}';
                    break;
            }
        }
        void Generator::stringify_string(const std::string &str) {
            res_ += '\"';
            for(auto it = str.begin(); it < str.end(); ++it) {
                unsigned char ch = *it;
                // 遇到如下转义字符和特殊字符也要原样输出
                switch (ch) {
                    case '\"': res_ += "\\\""; break;
                    case '\\': res_ += "\\\\"; break;
                    case '\b': res_ += "\\b";  break;
                    case '\f': res_ += "\\f";  break;
                    case '\n': res_ += "\\n";  break;
                    case '\r': res_ += "\\r";  break;
                    case '\t': res_ += "\\t";  break;
                    default:
                        // ASCII 字符集中的可打印字符（在十六进制代码 0x20 (32) 和 0x7e (126)之间）
                        // 第0～32(0x20)号及第127号(共34个)是控制字符或通讯专用字符
                        if (ch < 0x20 || ch == 0x7f) {
                            char buffer[7] = {0};
                            sprintf(buffer, "\\u%04X", ch);
                            res_ += buffer;
                        } else
                            res_ += *it;
                }
            }
            res_ += '\"';
        }

    }
}
