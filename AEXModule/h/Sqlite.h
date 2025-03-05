#pragma once
#include "../h/sqlite3/sqlite3.h"
#include "../h/Text.h"
#include <vector>
#include <string>
#include <iostream>
typedef struct SQLITE_RESULT {
    std::vector<std::string> tabName; // 表名
    std::vector<std::vector<std::string>> data; // 数据
} PSQLITE_RESULT;
// sqlite操作类
class Sqlite {
public:
    // 类释放时，释放所有数据库指针
    ~Sqlite();
    /*
    * 打开数据库(打开后默认将此次打开的设定为操作对象)
    * @param np 数据库名字 或 路径
    * @param id 数据库标识符，默认为NULL
    * @return 成功返回true，失败返回false,如果已经打开了数据库，也返回false
    */
    bool open(const std::string np,int id = NULL);
    /*
    * 关闭数据库
    * @param id 数据库标识符，默认为NULL,如果有多个数据库对象被设定为同一个id,则关闭该id对应的所有数据库
    * @return 成功返回true，失败返回false
    */
    bool close(int id = NULL);
    /*
     * 设置当前数据库
     * @param id 数据库标识符
     * @return 成功返回true，失败返回false
     */
    virtual bool setDb(int id);
    /*
     * 设置当前数据库
     * @param name 数据库名称 或 路径
     * @return 成功返回true，失败返回false
     */
    virtual bool setDb(const std::string& name);
    /*
    * 执行指令
    * @param sql 指令
    * @return 成功返回true，失败返回false
    */
    bool exec(const std::string& sql);
    /*
    * 执行指令
    * @param sql 指令
    * @param args 可变参数，用于替换SQL中的?占位符
    * @return 成功返回true，失败返回false
    */
    template<typename... Args>
    bool execs(const std::string& sql, Args... args) {
        std::string temp = sql;
        for (std::string arg : { args... }) {
            temp = Text::text_replace(temp, "?", "'" + arg + "'", 1);
        };
        return this->exec(temp);
    }
    /*
     * 查询指令
     * @param sql 查询的 SQL 语句
     * @return 如果成功，返回查询结果；如果失败，返回空
     */
    SQLITE_RESULT query(const std::string& sql);
    /*
     * 查询指令
     * @param sql 查询的 SQL 语句
     * @param args 可变参数，用于替换SQL中的?占位符
     * @return 如果成功，返回查询结果；如果失败，返回空
    */
    template<typename... Args>
    SQLITE_RESULT querys(const std::string& sql, Args... args) {
        std::string temp = sql;
        for (std::string arg : { args... }) {
            temp = Text::text_replace(temp, "?", "'" + arg + "'", 1);
        };
        return this->query(temp);
    }
    /*
    * 获取错误信息
    * @return 错误信息
    */
    std::string getError();
    /*
    * 添加获取最后插入ID的方法
    */ 
    int lastInsertRowid() const;
private:
    typedef struct sqlite3_content {
        int id; // 数据库标识符
        std::string np;// 名字 或 路径
        sqlite3* db;// 数据库指针
        sqlite3_content(const std::string& np, int id);
    } *psqlite3_content;
    std::vector<sqlite3_content> dbList; // 存储数据库指针的列表
    psqlite3_content content;// 当前数据库指针
    std::string error; // 错误信息
};
