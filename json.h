/*********************************************************************************
  *Copyright(C),Lwy
  *FileName:  json.h
  *Author:  lwy
  *Version:  1.3
  *Date:  2022-03-23
  *Description:  此文件声明支持处理的JSON类型、以及顶层Json类的声明，顶层命名空间为lwy
  *              每一个类型都可以用一个Json对象表示，对应于解析树上的一个结点
  *Function List:
  * Json类主要成员函数功能:
     1. void parse(const std::string &content, std::string &status)
            解析content字符串,结果放入成员变量v中，解析状态字符串用status返回
     2. void stringify(std::string &content) const noexcept
            将content对应的json::Value对象序列化成字符串
     3. void set_xxx() noexcept;
            在v中设置对应结点类型的值
     4. void get_xxx() const noexcept;
            获取v中对应结点类型的C++对象
     5. void xxx_array_element(...) ...;
            对数组结点类型做相关操作: 入队,出队,插入,删除
     6. xxx_object_xxx(...) ...;
            对Json对象类型做相关操作,键值对存储结构采用pair动态数组实现
**********************************************************************************/

#ifndef JSON_JSON_H
#define JSON_JSON_H


#include <memory>
#include <string>


namespace lwy {

    namespace json {
        enum type : int{
            Null,
            True,
            False,
            Number,
            String,
            Array,
            Object
        };
        class Value;
    }

    class Json final{
    public:
        void parse(const std::string &content, std::string &status) noexcept;
        void parse(const std::string &content);
        void stringify(std::string &content) const noexcept;

        Json() noexcept;
        ~Json() noexcept;
        Json(const Json &rhs) noexcept;
        Json& operator=(const Json &rhs) noexcept;
        Json(Json &&rhs) noexcept;
        Json& operator=(Json &&rhs) noexcept;
        void swap(Json &rhs) noexcept;

        int get_type() const noexcept;
        void set_null() noexcept;
        void set_boolean(bool b) noexcept;
        Json& operator=(bool b) noexcept { set_boolean(b); return *this; }

        double get_number() const noexcept;
        void set_number(double d) noexcept;
        Json& operator=(double d) noexcept { set_number(d); return *this; }

        const std::string get_string() const noexcept;
        void set_string(const std::string& str) noexcept;
        Json& operator=(const std::string& str) noexcept { set_string(str); return *this; }

        size_t get_array_size() const noexcept;
        Json get_array_element(size_t index) const noexcept;
        void set_array() noexcept;
        void pushback_array_element(const Json& val) noexcept;
        void popback_array_element() noexcept;
        void insert_array_element(const Json &val, size_t index) noexcept;
        void erase_array_element(size_t index, size_t count) noexcept;
        void clear_array() noexcept;

        void set_object() noexcept;
        size_t get_object_size() const noexcept;
        const std::string& get_object_key(size_t index) const noexcept;
        size_t get_object_key_length(size_t index) const noexcept;
        Json get_object_value(size_t index) const noexcept;
        void set_object_value(const std::string &key, const Json &val) noexcept;
        long long find_object_index(const std::string &key) const noexcept;
        void remove_object_value(size_t index) noexcept;
        void clear_object() noexcept;
    private:
        std::unique_ptr<json::Value> v;

        friend bool operator==(const Json &lhs, const Json &rhs) noexcept;
        friend bool operator!=(const Json &lhs, const Json &rhs) noexcept;
    };
    bool operator==(const Json &lhs, const Json &rhs) noexcept;
    bool operator!=(const Json &lhs, const Json &rhs) noexcept;
    void swap(Json &lhs, Json &rhs) noexcept;
}


#endif //JSON_JSON_H
