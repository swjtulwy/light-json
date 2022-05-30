# light-json :轻量级Json解析库

## noexcept 用法
noexcept 编译期完成声明和检查工作.noexcept 主要是解决的问题是减少运行时开销. 运行时开销指的是, 编译器需要为代码生成一些额外的代码用来包裹原始代码，当出现异常时可以抛出一些相关的堆栈stack unwinding错误信息, 这里面包含，错误位置, 错误原因, 调用顺序和层级路径等信息.当使用noexcept声明一个函数不会抛出异常候, 编译器就不会去生成这些额外的代码, 直接的减小的生成文件的大小, 间接的优化了程序运行效率.

- 每个函数都考虑noexcept会很麻烦，所以只在明显的时候使用
- 现在编译器在好路径上异常没有影响，noexcept可能的作用是减小体积
- 推荐在构造、复制等常用操作标记noexcept，这样性能提升可能会比较大。例如vector不会使用你的类move操作，除非它被标记为noexcept（有的编译器能自动推导）
- noexcept主要是给使用者看的，对编译器影响不大

## placement new用法
所谓placement new就是在用户指定的内存位置上构建新的对象，这个构建过程不需要额外分配内存，只需要调用对象的构造函数即可。
placement new把原本new做的两步工作分开来。第一步你自己分配内存，第二步你调用类的构造函数在自己分配的内存上构建新的对象。

placement new的好处：
1）在已分配好的内存上进行对象的构建，构建速度快。
2）已分配好的内存可以反复利用，有效避免内存碎片问题。
https://blog.csdn.net/zhangxinrun/article/details/5940019?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-5940019-blog-74169847.pc_relevant_paycolumn_v3&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7ERate-1-5940019-blog-74169847.pc_relevant_paycolumn_v3&utm_relevant_index=2

## Unicode 知识
U+0000 至 U+FFFF 这组 Unicode 字符称为基本多文种平面（basic multilingual plane, BMP），还有另外 16 个平面。那么 BMP 以外的字符，JSON 会使用代理对（surrogate pair）表示 \uXXXX\uYYYY。在 BMP 中，保留了 2048 个代理码点。如果第一个码点是 U+D800 至 U+DBFF，我们便知道它的代码对的高代理项（high surrogate），之后应该伴随一个 U+DC00 至 U+DFFF 的低代理项（low surrogate）。然后，我们用下列公式把代理对 (H, L) 变换成真实的码点：

`codepoint = 0x10000 + (H − 0xD800) × 0x400 + (L − 0xDC00)`
举个例子，高音谱号字符 𝄞 → U+1D11E 不是 BMP 之内的字符。在 JSON 中可写成转义序列 \uD834\uDD1E，我们解析第一个 \uD834 得到码点 U+D834，我们发现它是 U+D800 至 U+DBFF 内的码点，所以它是高代理项。然后我们解析下一个转义序列 \uDD1E 得到码点 U+DD1E，它在 U+DC00 至 U+DFFF 之内，是合法的低代理项。我们计算其码点：
``
H = 0xD834, L = 0xDD1E
codepoint = 0x10000 + (H − 0xD800) × 0x400 + (L − 0xDC00)
= 0x10000 + (0xD834 - 0xD800) × 0x400 + (0xDD1E − 0xDC00)
= 0x10000 + 0x34 × 0x400 + 0x11E
= 0x10000 + 0xD000 + 0x11E
= 0x1D11E
``
这样就得出这转义序列的码点，然后我们再把它编码成 UTF-8。如果只有高代理项而欠缺低代理项，或是低代理项不在合法码点范围，我们都返回 LEPT_PARSE_INVALID_UNICODE_SURROGATE 错误。