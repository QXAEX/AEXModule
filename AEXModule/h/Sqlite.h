#pragma once
#include "../h/sqlite3/sqlite3.h"
#include "../h/Text.h"
#include <vector>
#include <string>
#include <iostream>
typedef struct SQLITE_RESULT {
    std::vector<std::string> tabName; // ����
    std::vector<std::vector<std::string>> data; // ����
} PSQLITE_RESULT;
// sqlite������
class Sqlite {
public:
    // ���ͷ�ʱ���ͷ��������ݿ�ָ��
    ~Sqlite();
    /*
    * �����ݿ�(�򿪺�Ĭ�Ͻ��˴δ򿪵��趨Ϊ��������)
    * @param np ���ݿ����� �� ·��
    * @param id ���ݿ��ʶ����Ĭ��ΪNULL
    * @return �ɹ�����true��ʧ�ܷ���false,����Ѿ��������ݿ⣬Ҳ����false
    */
    bool open(const std::string np,int id = NULL);
    /*
    * �ر����ݿ�
    * @param id ���ݿ��ʶ����Ĭ��ΪNULL,����ж�����ݿ�����趨Ϊͬһ��id,��رո�id��Ӧ���������ݿ�
    * @return �ɹ�����true��ʧ�ܷ���false
    */
    bool close(int id = NULL);
    /*
     * ���õ�ǰ���ݿ�
     * @param id ���ݿ��ʶ��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool setDb(int id);
    /*
     * ���õ�ǰ���ݿ�
     * @param name ���ݿ����� �� ·��
     * @return �ɹ�����true��ʧ�ܷ���false
     */
    virtual bool setDb(const std::string& name);
    /*
    * ִ��ָ��
    * @param sql ָ��
    * @return �ɹ�����true��ʧ�ܷ���false
    */
    bool exec(const std::string& sql);
    /*
    * ִ��ָ��
    * @param sql ָ��
    * @param args �ɱ�����������滻SQL�е�?ռλ��
    * @return �ɹ�����true��ʧ�ܷ���false
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
     * ��ѯָ��
     * @param sql ��ѯ�� SQL ���
     * @return ����ɹ������ز�ѯ��������ʧ�ܣ����ؿ�
     */
    SQLITE_RESULT query(const std::string& sql);
    /*
     * ��ѯָ��
     * @param sql ��ѯ�� SQL ���
     * @param args �ɱ�����������滻SQL�е�?ռλ��
     * @return ����ɹ������ز�ѯ��������ʧ�ܣ����ؿ�
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
    * ��ȡ������Ϣ
    * @return ������Ϣ
    */
    std::string getError();
    /*
    * ��ӻ�ȡ������ID�ķ���
    */ 
    int lastInsertRowid() const;
private:
    typedef struct sqlite3_content {
        int id; // ���ݿ��ʶ��
        std::string np;// ���� �� ·��
        sqlite3* db;// ���ݿ�ָ��
        sqlite3_content(const std::string& np, int id);
    } *psqlite3_content;
    std::vector<sqlite3_content> dbList; // �洢���ݿ�ָ����б�
    psqlite3_content content;// ��ǰ���ݿ�ָ��
    std::string error; // ������Ϣ
};
