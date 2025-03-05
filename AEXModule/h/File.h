#pragma once
#include <windows.h>
#include <string>
#include "Byteset.h"
#include "Text.h"
#include "System.h"

/*
* �ļ���
* д��ı������㵱ǰ������ʹ�õĴ������һ��,���д������,��ע���������
*/
typedef class File {
public:
    File() = default;
    File(std::string path);
    ~File();
    /*
    * ���ļ�
    * @param path �ļ�·��
    * @param create �Ƿ񴴽��ļ�, Ĭ��Ϊfalse
    * @return �Ƿ�ɹ����ļ�
    */
    bool Open(std::string path, bool create = false);
    /*
    * �ر��ļ�
    * @return �Ƿ�ɹ��ر��ļ�
    */
    bool Close();
    /*
    * ɾ���ļ�
    * @return �Ƿ�ɹ�ɾ���ļ�
    */
    bool DelFile();
    /*
    * ���ļ�ָ�뵽ָ��λ��
    * @param pos λ��, Ĭ��Ϊ0
    * @param MoveMethod �ƶ���ʽ, Ĭ��ΪFILE_BEGIN
    * @return �Ƿ�ɹ����ļ�ָ��
    */
    bool Seek(long long pos = 0, DWORD MoveMethod = FILE_BEGIN);
    /*
    * ȡ�ļ�ָ��λ��
    * @return �ļ�ָ��λ��
    */
    long long GetPos() const;
    /*
    * ȡ�ļ���С
    * @return �ļ���С
    */
    size_t Size() const;
    /*
    * д������
    * @param data ��д������
    * @return �Ƿ�ɹ�д������
    */
    bool Write(Byteset data) const;
    /*
    * ��ȡ����
    * @param size ��ȡ���ݵĴ�С
    * @return ��ȡ��������
    */
    Byteset Read(long long size) const;
    /*
    * ��ȡ����
    * @param pos ��ȡ���ݵ�λ��
    * @param size ��ȡ���ݵĴ�С
    * @return ��ȡ��������
    */
    Byteset ReadAt(long long pos, long long size);
    /*
    * ɾ������
    * @param size ��ɾ�����ݵĴ�С
    * @return �Ƿ�ɹ�ɾ������
    */
    bool Delete(long long size) const;
    /*
    * �滻����
    * @param new_data ���滻������
    * @param size �滻����
    * @return �Ƿ�ɹ��滻����
    */
    bool Replace(Byteset new_data,long long size) const;
    /*
    * ��������
    * @param data �����ҵ�����
    * @param index ��ʼ����λ��, Ĭ��Ϊ0
    * @param flag �Ƿ���,Ĭ��Ϊfalse
    * @return �ҵ������ݵ�λ��, -1��ʾδ�ҵ�
    */
    size_t Find(Byteset data,size_t index = NULL,bool flag = false) const;
    /*  
    * �ļ��Ƿ����
    * @param path �ļ�·��
    * @return �Ƿ����
    */
    BOOL Exists(std::string path);
    /*
    * �����ļ�
    * @param src Դ�ļ�·��
    * @param dst Ŀ���ļ�·��
    * @return �Ƿ�ɹ������ļ�
    */
    BOOL Copy(std::string src, std::string dst);
private:
    std::string path = "";
    long long pos = NULL;
public:
    bool is_open;
    HANDLE hfile = NULL;
} * PFile;