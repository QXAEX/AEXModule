#pragma once
#include <vector>
#include <string>
#include <iostream>
typedef class Byteset {
public:
    // 默认构造函数
    Byteset() = default;

    ~Byteset();

    // 从字符串构造
    Byteset(const std::string& str);

    // 从字节数组构造
    Byteset(const void* bytes, long long size);

    // 从字节数组构造
    Byteset(const char* bytes);

    //从int类型构造
    Byteset(int num);

    //从long类型构造
    Byteset(long num);

    //从long long类型构造
    Byteset(long long num);

    //从size_t类型构造
    Byteset(size_t num);

    //从float类型构造
    Byteset(float num);

    //从double类型构造
    Byteset(double num);

    //从long double类型构造
    Byteset(long double num);

    //从bool类型构造
    Byteset(bool flag);

    //从char类型构造
    Byteset(char ch);

    //从short类型构造
    Byteset(short num);

    // 拷贝构造函数
    Byteset(const Byteset& other);

    // 移动构造函数
    Byteset(Byteset&& other) noexcept;

    //数据比较处理
    bool operator==(const Byteset& other) const;

    //数据不等比较处理
    bool operator!=(const Byteset& other) const;

    // 拷贝赋值操作符
    Byteset& operator=(const Byteset& other);

    // 拷贝赋值操作符
    Byteset& operator+=(const Byteset& other);
    //拷贝赋值操作符:文本字符串
    Byteset& operator=(const std::string& str);

    //拷贝赋值操作符:字节数组
    Byteset& operator=(const char* bytes);
    // 拷贝赋值操作符
    Byteset& operator=(std::initializer_list<unsigned char> init_list);

    // 初始化列表构造函数
    Byteset(std::initializer_list<unsigned char> init_list);

    /*
    * 追加字节
    * @param bytes 字节数组
    * @param size 字节数组大小
    */
    Byteset& append(const void* bytes, long long size);

    // 追加另一个 Byteset
    Byteset& append(const Byteset& other);

    // 追加字节
    void push_back(unsigned char byte);

    // 弹出最后一个字节
    unsigned char pop_back();

    // 访问字节
    unsigned char& operator[](long long index);

    // 访问字节
    const unsigned char& operator[](long long index) const;

    // 获取字节集大小
    long long size() const;

    // 调整字节集大小
    void resize(long long size);
    // 判断是否为空
    bool empty() const;

    // 清空字节集
    void clear();

    /*
    * 获取内部数据指针
    * @param index 要获取的字节集的索引,默认为NULL,表示获取整个字节集
    * @return 字节集数据指针
    */
    const unsigned char* data(long long index = NULL) const;

    //标准化输出
    friend std::ostream& operator<<(std::ostream& os, const Byteset& byteset);

    /*
    * 寻找字节集
    * @param other 要寻找的字节集
    * @param pos 开始搜索的位置
    * @param vague 是否模糊查找,模糊查找时,忽略other中与之匹配的字节,默认 不模糊查找
    * @param vagueContent 模糊查找的忽略字节集,默认为{0}
    * @return 找到的位置，如果没有找到，则返回-1
    */
    long long find(const Byteset& other, long long pos = NULL, bool vague = false, const Byteset& vagueContent = nullptr) const;

    /*
    * 替换字节集
    * @param old 要替换的字节集
    * @param new_str 新的字节集
    * @param count 替换次数,默认-1全部替换
    * @param pos 开始搜索的位置
    * @return 被替换的字节集个数
    */
    long long replace(const Byteset& old, const Byteset& new_str, int count = -1, long long pos = NULL);

    /*
    * 移除字节集
    * @param other 要移除的字节集
    * @param pos 开始搜索的位置
    * @param count 移除次数,默认-1全部移除
    * @return 被移除的字节集个数
    */
    long long remove(const Byteset& other, long long pos = NULL, int count = -1);

    /*
    * 获取字节集的子集
    * @param pos 开始位置
    * @param len 子集长度
    * @return 子集
    */
    Byteset subBytes(long long pos, long long len) const;

    //字节集的开始位置
    std::vector<unsigned char>::iterator begin();

    //字节集的开始位置
    std::vector<unsigned char>::const_iterator begin() const;

    //字节集的结束位置
    std::vector<unsigned char>::iterator end();

    //字节集的结束位置
    std::vector<unsigned char>::const_iterator end() const;

    //字节集的逆向开始位置
    std::vector<unsigned char>::reverse_iterator rbegin();

    //字节集的逆向开始位置
    std::vector<unsigned char>::const_reverse_iterator rbegin() const;

    //字节集的逆向结束位置
    std::vector<unsigned char>::reverse_iterator rend();

    //字节集的逆向结束位置
    std::vector<unsigned char>::const_reverse_iterator rend() const;
private:
    std::vector<unsigned char> data_;
} *LPBYTESET;