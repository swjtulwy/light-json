/*********************************************************************************
  *Copyright(C),Lwy
  *FileName:  json.h
  *Author:  lwy
  *Version:  1.3
  *Date:  2022-03-23
  *Description:  此文件声明存储表示JSON各种值的Value类, 采用union来存储Json结点，节省内存空间
  *Function List:
  * Value类主要成员函数功能:
     1. void parse(const std::string &content);
            解析content字符串,结果放入本类的数据成员union中
     2. void stringify(std::string &content) const noexcept;
            将本类的数据成员序列化为字符串
     4. void set_xxx() noexcept;
            在本类数据成员中设置对应结点类型的值
     5. void get_xxx() const noexcept;
            获取本类数据成员的值
     6. void xxx_array_element(...) ...;
            对数组结点类型做相关操作: 入队,出队,插入,删除
     7. xxx_object_xxx(...) ...;
            对Json对象类型做相关操作,键值对存储结构采用pair动态数组实现
     8. void init(const Value &rhs) noexcept;
            为数据成员申请内存
     9. void free() noexcept;
            释放数据成员的内存
**********************************************************************************/

#ifndef JSON_JSON_VALUE_H
#define JSON_JSON_VALUE_H

#include <string>
#include <vector>
#include <utility>
#include "json.h"

namespace lwy {

    namespace json {

        class Value final{
        public:
            void parse(const std::string &content);
            void stringify(std::string &content) const noexcept;

            int get_type() const noexcept;
            void set_type(type t) noexcept;

            double get_number() const noexcept;
            void set_number(double d) noexcept;

            const std::string& get_string() const noexcept;
            void set_string(const std::string &str) noexcept;

            size_t get_array_size() const noexcept;
            const Value& get_array_element(size_t index) const noexcept;
            void set_array(const std::vector<Value> &arr) noexcept;
            void pushback_array_element(const Value& val) noexcept;
            void popback_array_element() noexcept;
            void insert_array_element(const Value &val, size_t index) noexcept;
            void erase_array_element(size_t index, size_t count) noexcept;
            void clear_array() noexcept;

            size_t get_object_size() const noexcept;
            const std::string& get_object_key(size_t index) const noexcept;
            size_t get_object_key_length(size_t index) const noexcept;
            const Value& get_object_value(size_t index) const noexcept;
            void set_object_value(const std::string &key, const Value &val) noexcept;
            void set_object(const std::vector<std::pair<std::string, Value>> &obj) noexcept;
            size_t find_object_index(const std::string &key) const noexcept;
            void remove_object_value(size_t index) noexcept;
            void clear_object() noexcept;

            Value() noexcept : num_(0) {}
            Value(const Value &rhs) noexcept : num_(0) { init(rhs); }
            Value& operator=(const Value &rhs) noexcept;
            ~Value() noexcept;

        private:
            void init(const Value &rhs) noexcept;
            // 手动析构结点的union中非基本类型的成员
            void free() noexcept;

            json::type type_ = json::Null;
            union {
                double num_;
                std::string str_;
                std::vector<Value> arr_;
                std::vector<std::pair<std::string, Value>> obj_;
            };

            friend bool operator==(const Value &lhs, const Value &rhs) noexcept;
        };
        bool operator==(const Value &lhs, const Value &rhs) noexcept;
        bool operator!=(const Value &lhs, const Value &rhs) noexcept;
    }

}

#endif //JSON_JSON_VALUE_H
