/*********************************************************************************
  *Copyright(C),Lwy
  *FileName:  json_parser.cpp
  *Author:  lwy
  *Version:  1.3
  *Date:  2022-03-23
  *Description:  此文件实现Parser类
**********************************************************************************/

#include <cassert>
#include <cctype>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include "json_exception.h"
#include "json_parser.h"

namespace lwy {

    namespace json {

        // 直接读取判断并跳过第一个字符
        inline void expect(const char * &c, char ch) {
            assert(*c == ch);
            ++c;
        }

        // 构造中完成解析工作, 存入val
        Parser::Parser(Value &val, const std::string &content)
                : val_(val), cur_(content.c_str()) {
            val_.set_type(json::Null);
            parse_whitespace();
            parse_value();
            parse_whitespace();
            if(*cur_ != '\0'){
                val_.set_type(json::Null);
                throw(Exception("parse root not singular"));
            }
        }

        // 解析路由函数
        void Parser::parse_value() {
            switch (*cur_) {
                case 'n' : parse_literal("null", json::Null);  return;
                case 't' : parse_literal("true", json::True);  return;
                case 'f' : parse_literal("false", json::False); return;
                case '\"': parse_string(); return;
                case '[' : parse_array();  return;
                case '{' : parse_object(); return;
                default  : parse_number(); return;
                case '\0': throw(Exception("parse expect value"));
            }
        }

        // 解析空白符号
        void Parser::parse_whitespace() noexcept {
            while (*cur_ == ' ' || *cur_ == '\t' || *cur_ == '\n' || *cur_ == '\r')
                ++cur_;
        }

        // 解析字面值, 包括ture, false, null
        void Parser::parse_literal(const char *literal, json::type t) {
            // 将cur_对应的待解析串与literal对比
            expect(cur_, literal[0]);
            size_t i;
            for (i = 0; literal[i + 1]; ++i) {
                if (cur_[i] != literal[i + 1])
                    throw (Exception("parse invalid value"));
            }
            cur_ += i;
            val_.set_type(t);
        }

        // 解析数字
        // number = [ "-" ] int [ frac ] [ exp ]
        // int = "0" / digit1-9 *digit
        // frac = "." 1*digit
        // exp = ("e" / "E") ["-" / "+"] 1*digit
        void Parser::parse_number() {
            const char *p = cur_;
            if (*p == '-') ++p;
            if (*p == '0') ++p;
            else {
                if(!isdigit(*p)) throw (Exception("parse invalid value"));
                while(isdigit(*++p)) ;
            }
            if (*p == '.') {
                if(!isdigit(*++p)) throw (Exception("parse invalid value"));
                while(isdigit(*++p)) ;
            }
            if (*p == 'e' || *p == 'E') {
                ++p;
                if(*p == '+' || *p == '-') ++p;
                if(!isdigit(*p)) throw (Exception("parse invalid value"));
                while(isdigit(*++p)) ;
            }
            errno = 0;
            double v = strtod(cur_, nullptr);
            if (errno == ERANGE && (v == HUGE_VAL || v == -HUGE_VAL))
                throw (Exception("parse number too big"));
            val_.set_number(v);
            cur_ = p;
        }

        // 解析字符串
        void Parser::parse_string() {
            std::string s;
            parse_string_raw(s);
            val_.set_string(s);
        }

        // 解析原始字符串, 抽出来的公共部分, 方便复用，tmp用来接收结果
        void Parser::parse_string_raw(std::string &tmp) {
            expect(cur_, '\"');
            const char *p = cur_;
            unsigned u = 0, u2 = 0;
            while (*p != '\"') {
                if(*p == '\0')
                    throw(Exception("parse miss quotation mark"));
                if(*p == '\\' && ++p){
                    switch(*p++){
                        case '\"': tmp += '\"'; break;
                        case '\\': tmp += '\\'; break;
                        case '/' : tmp += '/' ; break;
                        case 'b' : tmp += '\b'; break;
                        case 'f' : tmp += '\f'; break;
                        case 'n' : tmp += '\n'; break;
                        case 'r' : tmp += '\r'; break;
                        case 't' : tmp += '\t'; break;
                        case 'u' :
                            parse_hex4(p, u);
                            if (u >= 0xD800 && u <= 0xDBFF) {
                                if (*p++ != '\\')
                                    throw(Exception("parse invalid unicode surrogate"));
                                if (*p++ != 'u')
                                    throw(Exception("parse invalid unicode surrogate"));
                                parse_hex4(p, u2);
                                if (u2 < 0xDC00 || u2 > 0xDFFF)
                                    throw(Exception("parse invalid unicode surrogate"));
                                u = (((u - 0xD800) << 10) | (u2 - 0xDC00)) + 0x10000;
                            }
                            parse_encode_utf8(tmp, u);
                            break;
                        default	 : throw (Exception("parse invalid string escape"));
                    }
                }
                else if ((unsigned char) *p < 0x20) {
                    throw (Exception("parse invalid string char"));
                }
                else tmp += *p++;
            }
            cur_ = ++p;
        }

        // 四位16进制表示
        void Parser::parse_hex4(const char* &p, unsigned &u) {
            u = 0;
            for (int i = 0; i < 4; ++i) {
                char ch = *p++;
                u <<= 4;
                if (isdigit(ch))
                    u |= ch - '0';
                else if (ch >= 'A' && ch <= 'F')
                    u |= ch - ('A' - 10);
                else if (ch >= 'a' && ch <= 'f')
                    u |= ch - ('a' - 10);
                else throw(Exception("parse invalid unicode hex"));
            }
        }

        // 码点范围	       码点位数	字节1	    字节2	    字节3	    字节4
        // U+0000 ~ U+007F	  7	    0xxxxxxx
        // U+0080 ~ U+07FF	  11	110xxxxx	10xxxxxx
        // U+0800 ~ U+FFFF	  16	1110xxxx	10xxxxxx	10xxxxxx
        // U+10000 ~ U+10FFFF 21	11110xxx	10xxxxxx	10xxxxxx	10xxxxxx
        void Parser::parse_encode_utf8(std::string &str, unsigned u) const noexcept {
            if (u <= 0x7F)
                str += static_cast<char> (u & 0xFF);
            else if (u <= 0x7FF) {
                str += static_cast<char> (0xC0 | ((u >> 6) & 0xFF));
                str += static_cast<char> (0x80 | ( u	   & 0x3F));
            }
            else if (u <= 0xFFFF) {
                str += static_cast<char> (0xE0 | ((u >> 12) & 0xFF));
                str += static_cast<char> (0x80 | ((u >>  6) & 0x3F));
                str += static_cast<char> (0x80 | ( u        & 0x3F));
            }
            else {
                assert(u <= 0x10FFFF);
                str += static_cast<char> (0xF0 | ((u >> 18) & 0xFF));
                str += static_cast<char> (0x80 | ((u >> 12) & 0x3F));
                str += static_cast<char> (0x80 | ((u >>  6) & 0x3F));
                str += static_cast<char> (0x80 | ( u        & 0x3F));
            }
        }
        void Parser::parse_array() {
            expect(cur_, '[');
            parse_whitespace();
            std::vector<Value> tmp;
            if (*cur_ == ']') {
                ++cur_;
                val_.set_array(tmp);
                return;
            }
            for (; ;) {
                try {
                    parse_value();
                } catch(Exception&) {
                    val_.set_type(json::Null);
                    throw;
                }
                tmp.push_back(val_);
                parse_whitespace();
                if (*cur_ == ','){
                    ++cur_;
                    parse_whitespace();
                } else if (*cur_ == ']') {
                    ++cur_;
                    val_.set_array(tmp);
                    return;
                } else {
                    val_.set_type(json::Null);
                    throw(Exception("parse miss comma or square bracket"));
                }
            }
        }

        void Parser::parse_object() {
            expect(cur_, '{');
            parse_whitespace();
            std::vector<std::pair<std::string, Value>> tmp;
            std::string key;
            if (*cur_ == '}') {
                ++cur_;
                val_.set_object(tmp);
                return;
            }
            for (; ;) {
                if (*cur_ != '\"')
                    throw(Exception("parse miss key"));
                try {
                    parse_string_raw(key);
                } catch (Exception&) {
                    throw(Exception("parse miss key"));
                }
                parse_whitespace();
                if (*cur_++ != ':')
                    throw(Exception("parse miss colon"));
                parse_whitespace();
                try {
                    parse_value();
                } catch(Exception&) {
                    val_.set_type(json::Null);
                    throw;
                }
                tmp.emplace_back(key, val_);
                val_.set_type(json::Null);
                key.clear();
                parse_whitespace();
                if (*cur_ == ',') {
                    ++cur_;
                    parse_whitespace();
                } else if (*cur_ == '}'){
                    ++cur_;
                    val_.set_object(tmp);
                    return;
                } else {
                    val_.set_type(json::Null);
                    throw(Exception("parse miss comma or curly bracket"));
                }
            }
        }
    }

}

