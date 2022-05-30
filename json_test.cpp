//
// Created by 15629 on 2022/5/28.
//

#include "gtest/gtest.h"

#include <string>
#include <cstring>
#include "json.h"

using namespace lwy;

class TestJson {
public:
    static void TestLiteral(const std::string& actual, json::type expect);
    static void TestNumber(const std::string& actual, double expect);
    static void TestString(const std::string& actual, const std::string& expect);
    static void TestError(const std::string& actual, const std::string& error);
    static void TestRoundTrip(const std::string& actual);
    static void TestEqual(const std::string& json1, const std::string& json2, int equality);

    static void TestParseLiteral();
    static void TestParseNumber();
    static void TestParseString();
    static void TestParseArray();
	static void TestParseObject();

	static void TestParseExpectValue();
	static void TestParseInvalidValue();
	static void TestParseRootNotSingular();
	static void TestParseNumberTooBig();
	static void TestParseMissingQuotationMark();
	static void TestParseInvalidStringEscape();
	static void TestParseInvalidStringChar();
	static void TestParseInvalidUnicodeHex();
	static void TestParseInvalidUnicodeSurrogate();
	static void TestParseMissCommaOrSquareBracket();
	static void TestParseMissKey();
	static void TestParseMissColon();
	static void TestParseMissCommaOrCurlyBracket();

    static void TestStringifyNumber();
    static void TestStringifyString();
    static void TestStringifyArray();
    static void TestStringifyObject();
    
    static void TestEqual();
    static void TestCopy();
    static void TestMove();
    static void TestSwap();

    static void TestAccessNull();
    static void TestAccessBoolean();
    static void TestAccessNumber();
    static void TestAccessString();
    static void TestAccessArray();
    static void TestAccessObject();
};


void TestJson::TestLiteral(const std::string& actual, json::type expect) {
    Json v;
    std::string status;
    v.set_boolean(false);
    v.parse(actual, status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(expect, v.get_type());
}

void TestJson::TestParseLiteral() {
    TestLiteral("null", json::Null);
    TestLiteral("true", json::True);
    TestLiteral("false", json::False);
}

void TestJson::TestNumber(const std::string &actual, double expect) {
    Json v;
    std::string status;
    v.parse(actual, status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(json::Number, v.get_type());
    EXPECT_DOUBLE_EQ(expect, v.get_number());
}

void TestJson::TestParseNumber() {
    TestNumber("0", 0.0);
    TestNumber("-0", 0.0);
    TestNumber("-0.0", 0.0);
    TestNumber("1", 1.0);
    TestNumber("-1", -1.0);
    TestNumber("1.5", 1.5);
    TestNumber("-1.5", -1.5);
    TestNumber("3.1416", 3.1416);
    TestNumber("1E10", 1E10);
    TestNumber("1e10", 1e10);
    TestNumber("1E+10", 1E+10);
    TestNumber("1E-10", 1E-10);
    TestNumber("-1E10", -1E10);
    TestNumber("-1e10", -1e10);
    TestNumber("-1E+10", -1E+10);
    TestNumber("-1E-10", -1E-10);
    TestNumber("1.234E+10", 1.234E+10);
    TestNumber("1.234E-10", 1.234E-10);
    TestNumber("1e-10000", 0.0);
}

void TestJson::TestString(const std::string& actual, const std::string& expect) {
    Json v;
    std::string status;
    v.parse(actual, status);
    EXPECT_EQ(json::String, v.get_type());
    EXPECT_EQ("parse ok", status);
    EXPECT_STREQ(expect.c_str(), v.get_string().c_str());
//    EXPECT_EQ(0,std::memcmp(expect.c_str(), v.get_string().c_str(), v.get_string().size()));
}

void TestJson::TestParseString() {
    TestString("\"\"", "");
    TestString("\"Hello\"", "Hello");

    TestString(R"("Hello\nWorld")", "Hello\nWorld");
    TestString(R"("\" \\ \/ \b \f \n \r \t")", "\" \\ / \b \f \n \r \t");

    TestString(R"("Hello\u0000World")", "Hello\0World");
    TestString(R"("\u0024")", R"($)");         /* Dollar sign U+0024 */
    TestString(R"("\u00A2")", "\xC2\xA2");     /* Cents sign U+00A2 */
    TestString(R"("\u20AC")", "\xE2\x82\xAC"); /* Euro sign U+20AC */
    TestString(R"("\uD834\uDD1E")", "\xF0\x9D\x84\x9E");  /* G clef sign U+1D11E */
    TestString(R"("\ud834\udd1e")", "\xF0\x9D\x84\x9E");  /* G clef sign U+1D11E */
}

void TestJson::TestParseArray() {
    Json v;
    std::string status;
    v.parse("[ ]", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(json::Array, v.get_type());
    EXPECT_EQ(0, v.get_array_size());

    v.parse("[ null , false , true , 123 , \"abc\" ]", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(json::Array, v.get_type());
    EXPECT_EQ(5, v.get_array_size());
    EXPECT_EQ(json::Null,   v.get_array_element(0).get_type());
    EXPECT_EQ(json::False,   v.get_array_element(1).get_type());
    EXPECT_EQ(json::True,   v.get_array_element(2).get_type());
    EXPECT_EQ(json::Number,   v.get_array_element(3).get_type());
    EXPECT_EQ(json::String,   v.get_array_element(4).get_type());
    EXPECT_DOUBLE_EQ(123.0, v.get_array_element(3).get_number());
    EXPECT_STREQ("abc", v.get_array_element(4).get_string().c_str());

    v.parse("[ [ ] , [ 0 ] , [ 0 , 1 ] , [ 0 , 1 , 2 ] ]", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(json::Array, v.get_type());
    EXPECT_EQ(4, v.get_array_size());
    for(int i = 0; i < 4; ++i) {
        lwy::Json a = v.get_array_element(i);
        EXPECT_EQ(json::Array, a.get_type());
        EXPECT_EQ(i, a.get_array_size());
        for(int j = 0; j < i; ++j) {
            lwy::Json e = a.get_array_element(j);
            EXPECT_EQ(json::Number, e.get_type());
            EXPECT_DOUBLE_EQ((double)j, e.get_number());
        }
    }
}

void TestJson::TestParseObject() {
    lwy::Json v;
    std::string status;
    v.parse(" { } ", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(json::Object, v.get_type());
    EXPECT_EQ(0, v.get_object_size());

    v.parse(" { "
            "\"n\" : null , "
            "\"f\" : false , "
            "\"t\" : true , "
            "\"i\" : 123 , "
            "\"s\" : \"abc\", "
            "\"a\" : [ 1, 2, 3 ],"
            "\"o\" : { \"1\" : 1, \"2\" : 2, \"3\" : 3 }"
            " } ", status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(7, v.get_object_size());
    EXPECT_EQ("n", v.get_object_key(0));
    EXPECT_EQ(json::Null, v.get_object_value(0).get_type());
    EXPECT_EQ("f", v.get_object_key(1));
    EXPECT_EQ(json::False, v.get_object_value(1).get_type());
    EXPECT_EQ("t", v.get_object_key(2));
    EXPECT_EQ(json::True, v.get_object_value(2).get_type());
    EXPECT_EQ("i", v.get_object_key(3));
    EXPECT_EQ(json::Number, v.get_object_value(3).get_type());
    EXPECT_DOUBLE_EQ(123.0, v.get_object_value(3).get_number());
    EXPECT_EQ("s", v.get_object_key(4));
    EXPECT_EQ(json::String, v.get_object_value(4).get_type());
    EXPECT_STREQ("abc", v.get_object_value(4).get_string().c_str());
    EXPECT_EQ("a", v.get_object_key(5));
    EXPECT_EQ(json::Array, v.get_object_value(5).get_type());
    EXPECT_EQ(3, v.get_object_value(5).get_array_size());
    for (int i = 0; i < 3; ++i) {
        lwy::Json e = v.get_object_value(5).get_array_element(i);
        EXPECT_EQ(json::Number, e.get_type());
        EXPECT_DOUBLE_EQ(i + 1.0, e.get_number());
    }
    EXPECT_EQ("o", v.get_object_key(6));
    {
        lwy::Json o = v.get_object_value(6);
        EXPECT_EQ(json::Object, o.get_type());
        for(int i = 0; i < 3; ++i) {
            lwy::Json ov = o.get_object_value(i);
            EXPECT_EQ('1' + i, (o.get_object_key(i))[0]);
            EXPECT_EQ(1, o.get_object_key_length(i));
            EXPECT_EQ(json::Number, ov.get_type());
            EXPECT_DOUBLE_EQ(i + 1.0, ov.get_number());
        }
    }
}

void TestJson::TestError(const std::string& actual, const std::string& error) {
    Json v;
    std::string status;
    v.parse(actual, status);
    EXPECT_EQ(error, status);
    EXPECT_EQ(json::Null, v.get_type());
}

// 测试样例为空，期待值
void TestJson::TestParseExpectValue() {
    TestError("", "parse expect value");
    TestError(" ", "parse expect value");
}

void TestJson::TestParseInvalidValue() {
    TestError("nul", "parse invalid value");
    TestError("!?", "parse invalid value");
    TestError("+0", "parse invalid value");
    TestError("+1", "parse invalid value");
    TestError(".123", "parse invalid value");
    TestError("1.", "parse invalid value");
    TestError("INF", "parse invalid value");
    TestError("inf", "parse invalid value");
    TestError("NAN", "parse invalid value");
    TestError("nan", "parse invalid value");
    TestError("[1,]", "parse invalid value");
    TestError("[\"a\", nul]", "parse invalid value");
}

// 测试样例包含不止一个Json结点以外的内容
void TestJson::TestParseRootNotSingular() {
    TestError("null x", "parse root not singular");
    TestError("trued", "parse root not singular");
    TestError("\"dsad\"d", "parse root not singular");

    TestError("0123", "parse root not singular");
    TestError("0x0", "parse root not singular");
    TestError("0x123", "parse root not singular");
}

void TestJson::TestParseNumberTooBig() {
    TestError("1e309", "parse number too big");
    TestError("-1e309", "parse number too big");
}

void TestJson::TestParseMissingQuotationMark() {
    TestError("\"", "parse miss quotation mark");
    TestError("\"abc", "parse miss quotation mark");
}

// 字符串逃逸，中途截断
void TestJson::TestParseInvalidStringEscape() {
    TestError(R"("\v")", "parse invalid string escape");
    TestError(R"("\'")", "parse invalid string escape");
    TestError(R"("\0")", "parse invalid string escape");
    TestError(R"("\x12")", "parse invalid string escape");
}

// 非法字符
void TestJson::TestParseInvalidStringChar() {
    TestError("\"\x01\"", "parse invalid string char");
    TestError("\"\x1F\"", "parse invalid string char");
}

void TestJson::TestParseInvalidUnicodeHex() {
    TestError(R"("\u")", "parse invalid unicode hex");
    TestError(R"("\u0")", "parse invalid unicode hex");
    TestError(R"("\u01")", "parse invalid unicode hex");
    TestError(R"("\u012")", "parse invalid unicode hex");
    TestError(R"("\u/000")", "parse invalid unicode hex");
    TestError(R"("\uG000")", "parse invalid unicode hex");
    TestError(R"("\u0/00")", "parse invalid unicode hex");
    TestError(R"("\u0G00")", "parse invalid unicode hex");
    TestError(R"("\u0/00")", "parse invalid unicode hex");
    TestError(R"("\u00G0")", "parse invalid unicode hex");
    TestError(R"("\u000G")", "parse invalid unicode hex");
    TestError(R"("\u000/")", "parse invalid unicode hex");
    TestError(R"("\u 123")", "parse invalid unicode hex");
}

// Unicode非法代理项
void TestJson::TestParseInvalidUnicodeSurrogate() {
    TestError(R"("\uD800")", "parse invalid unicode surrogate");
    TestError(R"("\uDBFF")", "parse invalid unicode surrogate");
    TestError(R"("\uD800\\")", "parse invalid unicode surrogate");
    TestError(R"("\uD800\uDBFF")", "parse invalid unicode surrogate");
    TestError(R"("\uD800\uE000")", "parse invalid unicode surrogate");
}

// 缺少逗号或方括号
void TestJson::TestParseMissCommaOrSquareBracket() {
    TestError("[1", "parse miss comma or square bracket");
    TestError("[1}", "parse miss comma or square bracket");
    TestError("[1 2", "parse miss comma or square bracket");
    TestError("[[]", "parse miss comma or square bracket");
}

// 缺少键
void TestJson::TestParseMissKey() {
    TestError("{:1,", "parse miss key");
    TestError("{1:1,", "parse miss key");
    TestError("{true:1,", "parse miss key");
    TestError("{false:1,", "parse miss key");
    TestError("{null:1,", "parse miss key");
    TestError("{[]:1,", "parse miss key");
    TestError("{{}:1,", "parse miss key");
    TestError(R"({"a":1,)", "parse miss key");
}

// 缺少冒号
void TestJson::TestParseMissColon() {
    TestError(R"({"a"})", "parse miss colon");
    TestError(R"({"a","b"})", "parse miss colon");
}

// 缺少花括号
void TestJson::TestParseMissCommaOrCurlyBracket() {
    TestError(R"({"a":1)", "parse miss comma or curly bracket");
    TestError(R"({"a":1])", "parse miss comma or curly bracket");
    TestError(R"({"a":1 "b")", "parse miss comma or curly bracket");
    TestError(R"({"a":{})", "parse miss comma or curly bracket");
}

// RoundTrip测试
void TestJson::TestRoundTrip(const std::string &actual) {
    Json v;
    std::string status;
    v.parse(actual, status);
    EXPECT_EQ("parse ok", status);
    v.stringify(status);
    EXPECT_EQ(actual, status);
}

void TestJson::TestStringifyNumber() {
    TestRoundTrip("0");
    TestRoundTrip("-0");
    TestRoundTrip("1");
    TestRoundTrip("-1");
    TestRoundTrip("1.5");
    TestRoundTrip("-1.5");
    TestRoundTrip("3.25");
    TestRoundTrip("1e+20");
    TestRoundTrip("1.234e+20");
    TestRoundTrip("1.234e-20");

    TestRoundTrip("1.0000000000000002"); /* the smallest number > 1 */
    TestRoundTrip("4.9406564584124654e-324"); /* minimum denormal */
    TestRoundTrip("-4.9406564584124654e-324");
    TestRoundTrip("2.2250738585072009e-308");  /* Max subnormal double */
    TestRoundTrip("-2.2250738585072009e-308");
    TestRoundTrip("2.2250738585072014e-308");  /* Min normal positive double */
    TestRoundTrip("-2.2250738585072014e-308");
    TestRoundTrip("1.7976931348623157e+308");  /* Max double */
    TestRoundTrip("-1.7976931348623157e+308");
}

void TestJson::TestStringifyString() {
    TestRoundTrip(R"("")");
    TestRoundTrip(R"("Hello")");
    TestRoundTrip(R"("Hello\nWorld")");
    TestRoundTrip(R"("\" \\ / \b \f \n \r \t")");
    TestRoundTrip(R"("Hello\u0000World")");
}

void TestJson::TestStringifyArray() {
    TestRoundTrip("[]");
    TestRoundTrip("[null,false,true,123,\"abc\",[1,2,3]]");
}

void TestJson::TestStringifyObject() {
    TestRoundTrip("{}");
    TestRoundTrip(R"({"n":null,"f":false,"t":true,"i":123,"s":"abc","a":[1,2,3],"o":{"1":1,"2":2,"3":3}})");
}

void TestJson::TestEqual(const std::string& json1, const std::string& json2, int equality) {
    Json v1, v2;
    std::string status;
    v1.parse(json1, status);
    EXPECT_EQ("parse ok", status);
    v2.parse(json2, status);
    EXPECT_EQ("parse ok", status);
    EXPECT_EQ(equality, int(v1 == v2));
}

// json串相等性测试
void TestJson::TestEqual() {
    TestEqual("true", "true", 1);
    TestEqual("true", "false", 0);
    TestEqual("false", "false", 1);
    TestEqual("null", "null", 1);
    TestEqual("null", "0", 0);
    TestEqual("123", "123", 1);
    TestEqual("123", "456", 0);
    TestEqual("\"abc\"", "\"abc\"", 1);
    TestEqual("\"abc\"", "\"abcd\"", 0);
    TestEqual("[]", "[]", 1);
    TestEqual("[]", "null", 0);
    TestEqual("[1,2,3]", "[1,2,3]", 1);
    TestEqual("[1,2,3]", "[1,2,3,4]", 0);
    TestEqual("[[]]", "[[]]", 1);
    TestEqual("{}", "{}", 1);
    TestEqual("{}", "null", 0);
    TestEqual("{}", "[]", 0);
    TestEqual(R"({"a":1,"b":2})", R"({"a":1,"b":2})", 1);
    TestEqual(R"({"a":1,"b":2})", R"({"b":2,"a":1})", 1);
    TestEqual(R"({"a":1,"b":2})", R"({"a":1,"b":3})", 0);
    TestEqual(R"({"a":1,"b":2})", R"({"a":1,"b":2,"c":3})", 0);
    TestEqual(R"({"a":{"b":{"c":{}}}})", R"({"a":{"b":{"c":{}}}})", 1);
    TestEqual(R"({"a":{"b":{"c":{}}}})", R"({"a":{"b":{"c":[]}}})", 0);
}

void TestJson::TestCopy() {
    Json v1, v2;
    v1.parse(R"({"t":true,"f":false,"n":null,"d":1.5,"a":[1,2,3]})");
    v2 = v1;
    EXPECT_EQ(1, int(v2 == v1));
}

void TestJson::TestMove() {
    Json v1, v2, v3;
    v1.parse(R"({"t":true,"f":false,"n":null,"d":1.5,"a":[1,2,3]})");
    v2 = v1;
    v3 = std::move(v2);
    EXPECT_EQ(json::Null, v2.get_type());
    EXPECT_EQ(1, int(v3 == v1));
}

void TestJson::TestSwap() {
    Json v1, v2;
    v1.set_string("Hello");
    v2.set_string("World!");
    swap(v1, v2);
    EXPECT_EQ("World!", v1.get_string());
    EXPECT_EQ("Hello", v2.get_string());
}

void TestJson::TestAccessNull() {
    Json v;
    v.set_string("a");
    v.set_null();
    EXPECT_EQ(json::Null, v.get_type());
}

void TestJson::TestAccessBoolean() {
    Json v;
    v.set_string("a");
    v.set_boolean(false);
    EXPECT_EQ(json::False, v.get_type());
}

void TestJson::TestAccessNumber() {
    Json v;
    v.set_string("a");
    v.set_number(1234.5);
    EXPECT_DOUBLE_EQ(1234.5, v.get_number());
}

void TestJson::TestAccessString() {
    Json v;
    v.set_string("");
    EXPECT_STREQ("", v.get_string().c_str());
    v.set_string("Hello");
    EXPECT_STREQ("Hello", v.get_string().c_str());
}

void TestJson::TestAccessArray() {
    Json a, e;
    for (size_t j = 0; j < 5; j += 5) {
        a.set_array();
        EXPECT_EQ(0, a.get_array_size());
        for (int i = 0; i < 10; ++i){
            e.set_number(i);
            a.pushback_array_element(e);
        }

        EXPECT_EQ(10, a.get_array_size());
        for (int i = 0; i < 10; ++i)
            EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());
    }

    a.popback_array_element();
    EXPECT_EQ(9, a.get_array_size());
    for (int i = 0; i < 9; ++i)
        EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());

    a.erase_array_element(4, 0);
    EXPECT_EQ(9, a.get_array_size());
    for (int i = 0; i < 9; ++i)
        EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());

    a.erase_array_element(8, 1);
    EXPECT_EQ(8, a.get_array_size());
    for (int i = 0; i < 8; ++i)
        EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());

    a.erase_array_element(0, 2);
    EXPECT_EQ(6, a.get_array_size());
    for (int i = 0; i < 6; ++i)
        EXPECT_EQ(static_cast<double>(i)+2, a.get_array_element(i).get_number());

    for (int i = 0; i < 2; ++i) {
        e.set_number(i);
        a.insert_array_element(e, i);
    }

    EXPECT_EQ(8, a.get_array_size());
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(static_cast<double>(i), a.get_array_element(i).get_number());
    }

    e.set_string("Hello");
    a.pushback_array_element(e);

    a.clear_array();
    EXPECT_EQ(0, a.get_array_size());
}

void TestJson::TestAccessObject() {
    Json o, v;
    for (int j = 0; j <= 5; j += 5) {
        o.set_object();
        EXPECT_EQ(0, o.get_object_size());
        for (int i = 0; i < 10; ++i) {
            std::string key = "a";
            key[0] += i;
            v.set_number(i);
            o.set_object_value(key, v);
        }

        EXPECT_EQ(10, o.get_object_size());
        for (int i = 10; i < 10; ++i) {
            std::string key = "a";
            key[0] += i;
            auto index = o.find_object_index(key);
            EXPECT_EQ(1, static_cast<int>(index >= 0));
            v = o.get_object_value(index);
            EXPECT_DOUBLE_EQ(static_cast<double>(i), v.get_number());
        }
    }

    auto index = o.find_object_index("j");
    EXPECT_EQ(1, static_cast<int>(index >= 0));
    o.remove_object_value(index);
    index = o.find_object_index("j");
    EXPECT_EQ(1, static_cast<int>(index < 0));
    EXPECT_EQ(9, o.get_object_size());

    index = o.find_object_index("a");
    EXPECT_EQ(1, static_cast<int>(index >= 0));
    o.remove_object_value(index);
    index = o.find_object_index("a");
    EXPECT_EQ(1, static_cast<int>(index < 0));
    EXPECT_EQ(8, o.get_object_size());

    for (int i = 0; i < 8; i++) {
        std::string key = "a";
        key[0] += i + 1;
        EXPECT_DOUBLE_EQ((double)i + 1, o.get_object_value(o.find_object_index(key)).get_number());
    }

    v.set_string("Hello");
    o.set_object_value("World", v);

    index = o.find_object_index("World");
    EXPECT_EQ(1, static_cast<int>(index >= 0));
    v = o.get_object_value(index);
    EXPECT_STREQ("Hello", v.get_string().c_str());

    o.clear_object();
    EXPECT_EQ(0, o.get_object_size());
}

TEST(testParse, literal) {
    TestJson::TestParseLiteral();
}

TEST(testParse, number) {
    TestJson::TestParseNumber();
}

TEST(testParse, string) {
    TestJson::TestParseString();
}

TEST(testParse, array) {
    TestJson::TestParseArray();
}

TEST(testParse, object) {
    TestJson::TestParseObject();
}

TEST(testParse, expectValue) {
    TestJson::TestParseExpectValue();
}

TEST(testParse, invalidValue) {
    TestJson::TestParseInvalidValue();
}

TEST(testParse, rootNotSingular) {
    TestJson::TestParseRootNotSingular();
}

TEST(testParse, numberTooBig) {
    TestJson::TestParseNumberTooBig();
}

TEST(testParse, missingQuotationMark) {
    TestJson::TestParseMissingQuotationMark();
}

TEST(testParse, invalidStringChar) {
    TestJson::TestParseInvalidStringChar();
}

TEST(testParse, invalidUnicodeHex) {
    TestJson::TestParseInvalidUnicodeHex();
}

TEST(testParse, invalidStringEscape) {
    TestJson::TestParseInvalidStringEscape();
}

TEST(testParse, invalidUnicodeSurrogate) {
    TestJson::TestParseInvalidUnicodeSurrogate();
}

TEST(testParse, missCommaOrSquareBracket) {
    TestJson::TestParseMissCommaOrSquareBracket();
}

TEST(testParse, missKey) {
    TestJson::TestParseMissKey();
}

TEST(testParse, missColon) {
    TestJson::TestParseMissColon();
}

TEST(testParse, missCommaOrCurlyBracket) {
    TestJson::TestParseMissCommaOrCurlyBracket();
}

TEST(testStringify, literal) {
    TestJson::TestRoundTrip("null");
    TestJson::TestRoundTrip("false");
    TestJson::TestRoundTrip("true");
}

TEST(testStringify, number) {
    TestJson::TestStringifyNumber();
}

TEST(testStringify, string) {
    TestJson::TestStringifyString();
}

TEST(testStringify, array) {
    TestJson::TestStringifyArray();
}

TEST(testStringify, object) {
    TestJson::TestStringifyObject();
}

TEST(testEqual, equal) {
    TestJson::TestEqual();
}

TEST(testCopy, copy) {
    TestJson::TestCopy();
}


TEST(testMove, move) {
    TestJson::TestMove();
}

TEST(testSwap, swap) {
    TestJson::TestSwap();
}

TEST(testAccess, null) {
    TestJson::TestAccessNull();
}

TEST(testAccess, boolean) {
    TestJson::TestAccessBoolean();
}

TEST(testAccess, number) {
    TestJson::TestAccessNumber();
}

TEST(testAccess, string) {
    TestJson::TestAccessString();
}

TEST(testAccess, arrey) {
    TestJson::TestAccessArray();
}

TEST(testAccess, object) {
    TestJson::TestAccessObject();
}


int main() {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
