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
        std::vector<std::string> columns;
        for (int i = 0; i < cols; i++) {
            columns.push_back(std::string((const char*)sqlite3_column_text(stmt, i)));
        }
        result.data.push_back(columns);
    };
    sqlite3_finalize(stmt);
    return result;
};

std::string Sqlite::getError()
{
    return this->error;
}

int Sqlite::lastInsertRowid() const {
    if (this->content == nullptr) throw std::runtime_error("数据库打开或未设置");
    return sqlite3_last_insert_rowid(this->content->db);
}