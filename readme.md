# light-json : 轻量级 JSON 解析库

## 项目描述
- 采用基于 C++11 的测试驱动开发模式, 单元测试框架为 GoogleTest
- 符合标准的 JSON 解析器和生成器
- 手写的递归下降解析器
- 仅支持 UTF-8 JSON 文本
- 仅支持以 double 存储 JSON number 类型
## 使用说明
引入 json.h 头文件, 声明 Json 类后即可使用 Json 类下的 JSON 串相关操作
``` 
Json v; 
v.parse(" xxxx ");
std::string str = v.get_string();
```
## 技术介绍

### 项目文件说明
json.cpp 封装实现了对 JSON 串的所有操作,也是对外开放的接口   
json_parser.cpp 实现对 JSON 串的解析工作  
json_generator.cpp 实现对 JSON 串的序列化(将 JSON 的 C++ 对象表示转化为字符串)  
json_value.cpp 实现 JSON 串在 C++ 中表示的数据结构以及相关的操作  

### Unicode 知识
U+0000 至 U+FFFF 这组 Unicode 字符称为基本多文种平面（basic multilingual plane, BMP），还有另外 16 个平面。那么 BMP 以外的字符，JSON 会使用代理对（surrogate pair）表示 \uXXXX\uYYYY。在 BMP 中，保留了 2048 个代理码点。如果第一个码点是 U+D800 至 U+DBFF，我们便知道它的代码对的高代理项（high surrogate），之后应该伴随一个 U+DC00 至 U+DFFF 的低代理项（low surrogate）。然后，我们用下列公式把代理对 (H, L) 变换成真实的码点:
``` 
codepoint = 0x10000 + (H − 0xD800) × 0x400 + (L − 0xDC00)
```
举个例子，高音谱号字符 𝄞 → U+1D11E 不是 BMP 之内的字符。在 JSON 中可写成转义序列 \uD834\uDD1E，我们解析第一个 \uD834 得到码点 U+D834，我们发现它是 U+D800 至 U+DBFF 内的码点，所以它是高代理项。然后我们解析下一个转义序列 \uDD1E 得到码点 U+DD1E，它在 U+DC00 至 U+DFFF 之内，是合法的低代理项。我们计算其码点：
```
H = 0xD834, L = 0xDD1E
codepoint = 0x10000 + (H − 0xD800) × 0x400 + (L − 0xDC00)
= 0x10000 + (0xD834 - 0xD800) × 0x400 + (0xDD1E − 0xDC00)
= 0x10000 + 0x34 × 0x400 + 0x11E
= 0x10000 + 0xD000 + 0x11E
= 0x1D11E
```
这样就得出这转义序列的码点，然后我们再把它编码成 UTF-8。如果只有高代理项而欠缺低代理项，或是低代理项不在合法码点范围，则返回错误。

由于我们的 JSON 库也只支持 UTF-8，通过上述方式计算出码点后, 我们需要把码点编码成 UTF-8。这里简单介绍一下 UTF-8 的编码方式。

UTF-8 的编码单元为 8 位（1 字节），每个码点编码成 1 至 4 个字节。它的编码方式很简单，按照码点的范围，把码点的二进位分拆成 1 至最多 4 个字节：

| 码点范围            | 码点位数  | 字节1     | 字节2    | 字节3    | 字节4     |
|:------------------:|:--------:|:--------:|:--------:|:--------:|:--------:|
| U+0000 ~ U+007F    | 7        | 0xxxxxxx |
| U+0080 ~ U+07FF    | 11       | 110xxxxx | 10xxxxxx |
| U+0800 ~ U+FFFF    | 16       | 1110xxxx | 10xxxxxx | 10xxxxxx |
| U+10000 ~ U+10FFFF | 21       | 11110xxx | 10xxxxxx | 10xxxxxx | 10xxxxxx |

### noexcept 用法
noexcept 编译期完成声明和检查工作.noexcept 主要是解决的问题是减少运行时开销. 运行时开销指的是, 
编译器需要为代码生成一些额外的代码用来包裹原始代码，当出现异常时可以抛出一些相关的堆栈 stack unwinding 错误信息, 
这里面包含，错误位置, 错误原因, 调用顺序和层级路径等信息.  
当使用 noexcept 声明一个函数不会抛出异常的时候, 编译器就不会去生成这些额外的代码, 直接的减小的生成文件的大小, 间接的优化了程序运行效率.

- 每个函数都考虑 noexcept 会很麻烦，所以只在明显的时候使用
- 现在编译器在好路径上异常没有影响，noexcept 可能的作用是减小体积
- 推荐在构造、复制等常用操作标记 noexcept，这样性能提升可能会比较大。例如 vector 不会使用你的类 move 操作，除非它被标记为 noexcept（有的编译器能自动推导）
- noexcept 主要是给使用者看的，对编译器影响不大

### placement new 用法
所谓 placement new 就是在用户指定的内存位置上构建新的对象，这个构建过程不需要额外分配内存，只需要调用对象的构造函数即可。  
placement new 把原本 new 做的两步工作分开来。第一步你自己分配内存，第二步你调用类的构造函数在自己分配的内存上构建新的对象。

placement new 的好处：  
1）在已分配好的内存上进行对象的构建，构建速度快。  
2）已分配好的内存可以反复利用，有效避免内存碎片问题。

### unique_ptr 用法
unique_ptr 不共享它的指针。它无法复制到其他 unique_ptr，无法通过值传递到函数，也无法用于需要副本的任何标准模板库 (STL) 算法。
只能移动 unique_ptr。这意味着，内存资源所有权将转移到另一 unique_ptr，并且原始 unique_ptr 不再拥有此资源。  
我们建议你将对象限制为由一个所有者所有，因为多个所有权会使程序逻辑变得复杂。  
因此，当需要智能指针用于纯 C++ 对象时，可使用 unique_ptr，而当构造 unique_ptr 时，可使用 make_unique 函数。

使用 unique_ptr 来管理动态内存，只要 unique_ptr 指针创建成功，其析构函数都会被调用。确保动态资源被释放。