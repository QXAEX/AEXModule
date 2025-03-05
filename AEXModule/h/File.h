#pragma once
#include <windows.h>
#include <string>
#include "Byteset.h"
#include "Text.h"
#include "System.h"

/*
* 文件类
* 写入的编码与你当前编译器使用的代码编码一样,如果写入中文,请注意编码问题
*/
typedef class File {
public:
    File() = default;
    File(std::string path);
    ~File();
    /*
    * 打开文件
    * @param path 文件路径
    * @param create 是否创建文件, 默认为false
    * @return 是否成功打开文件
    */
    bool Open(std::string path, bool create = false);
    /*
    * 关闭文件
    * @return 是否成功关闭文件
    */
    bool Close();
    /*
    * 删除文件
    * @return 是否成功删除文件
    */
    bool DelFile();
    /*
    * 置文件指针到指定位置
    * @param pos 位置, 默认为0
    * @param MoveMethod 移动方式, 默认为FILE_BEGIN
    * @return 是否成功置文件指针
    */
    bool Seek(long long pos = 0, DWORD MoveMethod = FILE_BEGIN);
    /*
    * 取文件指针位置
    * @return 文件指针位置
    */
    long long GetPos() const;
    /*
    * 取文件大小
    * @return 文件大小
    */
    size_t Size() const;
    /*
    * 写入数据
    * @param data 待写入数据
    * @return 是否成功写入数据
    */
    bool Write(Byteset data) const;
    /*
    * 读取数据
    * @param size 读取数据的大小
    * @return 读取到的数据
    */
    Byteset Read(long long size) const;
    /*
    * 读取数据
    * @param pos 读取数据的位置
    * @param size 读取数据的大小
    * @return 读取到的数据
    */
    Byteset ReadAt(long long pos, long long size);
    /*
    * 删除数据
    * @param size 待删除数据的大小
    * @return 是否成功删除数据
    */
    bool Delete(long long size) const;
    /*
    * 替换数据
    * @param new_data 待替换的数据
    * @param size 替换长度
    * @return 是否成功替换数据
    */
    bool Replace(Byteset new_data,long long size) const;
    /*
    * 查找数据
    * @param data 待查找的数据
    * @param index 起始查找位置, 默认为0
    * @param flag 是否倒找,默认为false
    * @return 找到的数据的位置, -1表示未找到
    */
    size_t Find(Byteset data,size_t index = NULL,bool flag = false) const;
    /*  
    * 文件是否存在
    * @param path 文件路径
    * @return 是否存在
    */
    BOOL Exists(std::string path);
    /*
    * 复制文件
    * @param src 源文件路径
    * @param dst 目标文件路径
    * @return 是否成功复制文件
    */
    BOOL Copy(std::string src, std::string dst);
private:
    std::string path = "";
    long long pos = NULL;
public:
    bool is_open;
    HANDLE hfile = NULL;
} * PFile;