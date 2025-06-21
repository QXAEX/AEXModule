#include "../h/Sqlite.h"
#include <stdexcept>

Sqlite::sqlite3_content::sqlite3_content(const std::string& np, int id)
{
    this->np = np;
    this->id = id;
    this->db = nullptr;
}

Sqlite::~Sqlite()
{
    for (sqlite3_content item : this->dbList) {
        sqlite3_close(item.db);
    }
}

bool Sqlite::open(const std::string np, int id)
{
    sqlite3_content sc(np,id);
    for (sqlite3_content item : this->dbList) {
        if (item.np.find(np) != std::string::npos) {
            return false;
        }
    }
    if (sqlite3_open(np.c_str(), &sc.db) == SQLITE_OK) {
        this->dbList.push_back(sc);
        return this->setDb(np);
    }
    printf("error: %s\n", sqlite3_errmsg(sc.db));
	return false;
}
bool Sqlite::close(int id)
{
    bool flag = false;
    for (size_t i = 0; i < this->dbList.size(); i++) {
        if (this->dbList[i].id == id) {
            flag = sqlite3_close(this->dbList[i].db) == SQLITE_OK;
            this->dbList.erase(this->dbList.begin() + i);
        }
    }
    return flag;
}

bool Sqlite::setDb(int id)
{
    for (size_t i = 0; i < this->dbList.size(); i++) {
        if (this->dbList[i].id == id) {
            this->content = &this->dbList[i];
            return true;
        }
    }
    return false;
}

bool Sqlite::setDb(const std::string& name)
{
    for (size_t i = 0; i < this->dbList.size(); i++) {
        if (this->dbList[i].np.find(name) != std::string::npos) {
            this->content = &this->dbList[i];
            return true;
        }
    }
    return false;
}

bool Sqlite::exec(const std::string& sql)
{
    if (this->content == nullptr) throw std::runtime_error("数据库打开或未设置");
    char* errorMessage = nullptr; // 用于存储错误信息
    int rc = sqlite3_exec(this->content->db, sql.c_str(), nullptr, 0, &errorMessage);

    if (rc != SQLITE_OK) {
        this->error = errorMessage ? std::string(errorMessage) : "";
        if(errorMessage) sqlite3_free(errorMessage);
        return false;
    }
    int rt = sqlite3_changes(this->content->db);
    return rt == 0 ? rc == SQLITE_OK : true;
}


bool Sqlite::execs(const std::string& sql, std::initializer_list<std::string> args)
{
    std::string temp = sql;
    const std::regex number_regex(R"(^[-+]?\d+\.?\d*$)");  // 匹配整数/浮点数
    const std::regex datetime_regex(R"(^\d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2}$)"); // 匹配时间
    for (const std::string& arg : args) {
        std::string processed_arg;
        if (std::regex_match(arg, number_regex)) {
            processed_arg = arg; // 数字不做处理
        }
        else if (std::regex_match(arg, datetime_regex)) {
            processed_arg = "datetime('" + arg + "')"; // SQLite时间格式化
        }
        else {
            // 转义单引号并包裹
            processed_arg = "'" + Text::text_replace(arg, "'", "''") + "'";
        }

        size_t pos = temp.find('?');
        if (pos != std::string::npos) {
            temp.replace(pos, 1, processed_arg);
        }
    }
    return this->exec(temp);
}

bool Sqlite::execs(const std::string& sql, std::vector<std::string> args)
{
    std::string temp = sql;
    const std::regex number_regex(R"(^[-+]?\d+\.?\d*$)");  // 匹配整数/浮点数
    const std::regex datetime_regex(R"(^\d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2}$)"); // 匹配时间
    for (const std::string& arg : args) {   
        std::string processed_arg;
        if (std::regex_match(arg, number_regex)) {
            processed_arg = arg; // 数字不做处理
        }
        else if (std::regex_match(arg, datetime_regex)) {
            processed_arg = "datetime('" + arg + "')"; // SQLite时间格式化
        }
        else {
            // 转义单引号并包裹
            processed_arg = "'" + Text::text_replace(arg, "'", "''") + "'";
        }

        size_t pos = temp.find('?');
        if (pos != std::string::npos) {
            temp.replace(pos, 1, processed_arg);
        }
    }
    return this->exec(temp);
}

SQLITE_RESULT Sqlite::query(const std::string& sql)
{
    if (this->content == nullptr) throw std::runtime_error("数据库打开或未设置");
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(this->content->db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        this->error = std::string(sqlite3_errmsg(this->content->db));
        return SQLITE_RESULT();
    }
    int cols = sqlite3_column_count(stmt);
    SQLITE_RESULT result;
    for (int i = 0; i < cols; i++) {
        result.tabName.push_back(sqlite3_column_name(stmt, i));
    }
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        std::map<std::string, std::string> columns;
        for (int i = 0; i < cols; i++) {
            columns[std::string(sqlite3_column_name(stmt, i))] = std::string(sqlite3_column_text(stmt, i) ? (const char*)sqlite3_column_text(stmt, i) : "null");
        }
        result.data.push_back(columns);
    };
    sqlite3_finalize(stmt);
    return result;
};

SQLITE_RESULT Sqlite::querys(const std::string& sql, std::initializer_list<std::string> args)
{
    std::string temp = sql;
    const std::regex number_regex(R"(^[-+]?\d+\.?\d*$)");  // 匹配整数/浮点数
    const std::regex datetime_regex(R"(^\d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2}$)"); // 匹配时间
    for (const std::string& arg : args) {
        std::string processed_arg;
        if (std::regex_match(arg, number_regex)) {
            processed_arg = arg; // 数字不做处理
        }
        else if (std::regex_match(arg, datetime_regex)) {
            processed_arg = "datetime('" + arg + "')"; // SQLite时间格式化
        }
        else {
            // 转义单引号并包裹
            processed_arg = "'" + Text::text_replace(arg, "'", "''") + "'";
        }

        size_t pos = temp.find('?');
        if (pos != std::string::npos) {
            temp.replace(pos, 1, processed_arg);
        }
    }

    return this->query(temp);
}

SQLITE_RESULT Sqlite::querys(const std::string& sql, std::vector<std::string> args)
{
    std::string temp = sql;
    const std::regex number_regex(R"(^[-+]?\d+\.?\d*$)");  // 匹配整数/浮点数
    const std::regex datetime_regex(R"(^\d{4}-\d{2}-\d{2}\s\d{2}:\d{2}:\d{2}$)"); // 匹配时间
    for (const std::string& arg : args) {
        std::string processed_arg;
        if (std::regex_match(arg, number_regex)) {
            processed_arg = arg; // 数字不做处理
        }
        else if (std::regex_match(arg, datetime_regex)) {
            processed_arg = "datetime('" + arg + "')"; // SQLite时间格式化
        }
        else {
            // 转义单引号并包裹
            processed_arg = "'" + Text::text_replace(arg, "'", "''") + "'";
        }

        size_t pos = temp.find('?');
        if (pos != std::string::npos) {
            temp.replace(pos, 1, processed_arg);
        }
    }

    return this->query(temp);
}

std::string Sqlite::getError()
{
    return this->error;
}

int Sqlite::lastInsertRowid() const {
    if (this->content == nullptr) throw std::runtime_error("数据库打开或未设置");
    return sqlite3_last_insert_rowid(this->content->db);
}

int Sqlite::getTableTotal(const std::string& tableName) {
    if (tableName.empty() || !isValidIdentifier(tableName)) {
        this->error = "Invalid table name: " + tableName;
        return -1;
    }
    const std::string sql = "SELECT COUNT(*) FROM \"" + escapeString(tableName) + "\";";
    sqlite3_stmt* stmt = nullptr;
    int prepareRet = sqlite3_prepare_v2(this->content->db, sql.c_str(), -1, &stmt, nullptr);
    if (prepareRet != SQLITE_OK) {
        this->error = sqlite3_errstr(prepareRet);
        if (stmt) sqlite3_finalize(stmt);
        return -1;
    }
    int count = -1;
    const int stepRet = sqlite3_step(stmt);
    switch (stepRet) {
    case SQLITE_ROW:
        count = sqlite3_column_int(stmt, 0);
        this->error.clear();
        break;
    case SQLITE_DONE:  // 空表情况
        count = 0;
        this->error.clear();
        break;
    default:
        this->error = sqlite3_errmsg(this->content->db);
        break;
    }
    sqlite3_finalize(stmt);
    return count;
}

// 辅助函数：验证SQL标识符合法性
bool Sqlite::isValidIdentifier(const std::string& str) {
    static const std::regex identifierRegex(R"(^[a-zA-Z_][a-zA-Z0-9_]*$)");
    return std::regex_match(str, identifierRegex);
}

// 辅助函数：防止SQL注入
std::string Sqlite::escapeString(const std::string& input) {
    std::string output;
    output.reserve(input.size() * 2);
    for (char c : input) {
        if (c == '"') output += "\"";
        output += c;
    }
    return output;
}
