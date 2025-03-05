#pragma once
#include "File.h"
#include "./PortableExecutable/data.h"
#include <vector>
#include <string>
#include <map>

class PE {
public:
    PE() = default;
    PE(LPCSTR filename);
    std::string ErrorToString(PE_ERROR error);
    /*
    * @brief ���� PE �ļ�
    * @param filename PE �ļ���(xx.dll/xx.exe/xx.sys)
    * @return �ɹ����� TRUE��ʧ�ܷ��� FALSE
    */
    auto WINAPI Import(LPCSTR filename)->BOOL;
    /*
    * @brief ���� PE �ļ�
    * @param filename PE �ļ���(xx.dll/xx.exe/xx.sys)
    * @return �ɹ����� TRUE��ʧ�ܷ��� FALSE
    */
    auto WINAPI Export(LPCSTR filename)->BOOL;
    /*
    * @brief ��֤ PE �ļ��Ƿ���Ч
    * @return ���ش����� 0��Ϊ��Ч
    */
    auto WINAPI Validate()->PE_ERROR;
    /*
    * @brief ��ȡ PE �ļ�ͷ��Ϣ
    * @return PE �ļ�ͷ��Ϣ
    */
    auto WINAPI GetHeader()->PE_HEADER;
    /*
    * @brief ��ȡ PE �ļ�������Ϣ
    * @param error ������ָ�룬��Ϊ nullptr �򲻷��ش�����Ϣ
    * @return PE �ļ�������Ϣ
    */
    auto WINAPI GetSectionHeaders(PE_ERROR* error = nullptr)->std::vector<PE_SECTION_HEADER>;
    /*
    * @brief ��ȡ PE �ļ��������Ϣ
    * @param error ������ָ�룬��Ϊ nullptr �򲻷��ش�����Ϣ
    * @return PE �ļ��������Ϣ
    */
    auto WINAPI GetImportTable(PE_ERROR* error = nullptr)->std::vector<PE_IMPORT_DESCRIPTOR>;
    /*
    * @brief ��ȡ PE �ļ���������Ϣ
    * @param error ������ָ�룬��Ϊ nullptr �򲻷��ش�����Ϣ
    * @return PE �ļ���������Ϣ
    */
    auto WINAPI GetExportTable(PE_ERROR* error = nullptr)->std::vector<PE_EXPORT_DIRECTORY>;
    /*
    * @brief ��ȡ PE �ļ���Դ����Ϣ
    * @param error ������ָ�룬��Ϊ nullptr �򲻷��ش�����Ϣ
    * @return PE �ļ���Դ����Ϣ
    */
    auto WINAPI GetResourceTable(PE_ERROR* error = nullptr)->std::vector<PE_RESOURCE_DIRECTORY>;
    /*
    * @brief ��ȡ PE �ļ��ض�λ����Ϣ
    * @param error ������ָ�룬��Ϊ nullptr �򲻷��ش�����Ϣ
    * @return PE �ļ��ض�λ����Ϣ
    */
    auto WINAPI GetRelocationTable(PE_ERROR* error = nullptr)->std::vector<PE_BASE_RELOCATION>;
    /*
    * @brief ��ȡ PE �ļ�������Ϣ
    * @param error ������ָ�룬��Ϊ nullptr �򲻷��ش�����Ϣ
    * @return PE �ļ�������Ϣ
    */
    auto WINAPI GetDebugInfo(PE_ERROR* error = nullptr)->PE_DEBUG_DIRECTORY;
    /*
    * @brief ��ȡ PE �ļ� TLS ����Ϣ
    * @param error ������ָ�룬��Ϊ nullptr �򲻷��ش�����Ϣ
    * @return PE �ļ� TLS ����Ϣ
    */
    auto WINAPI GetTlsTable(PE_ERROR* error = nullptr)->PE_TLS_DIRECTORY32;
    /*
    * @brief ��ȡ PE �ļ� TLS ����Ϣ
    * @param error ������ָ�룬��Ϊ nullptr �򲻷��ش�����Ϣ
    * @return PE �ļ� TLS ����Ϣ
    */
    auto WINAPI GetTlsTable64(PE_ERROR* error = nullptr)->PE_TLS_DIRECTORY64;
    //// ----------------- ������������ -----------------
    //// ������Ϣ��ȡ
    ///// @brief ��ȡ��ڵ� RVA ��ַ
    //auto WINAPI GetEntryPointRVA() const->DWORD;
    ///// @brief ��ȡ�����ַ (ImageBase)
    //auto WINAPI GetImageBase() const->ULONGLONG;
    ///// @brief �ж��Ƿ�Ϊ 64 λ PE �ļ�
    //auto WINAPI Is64Bit() const->BOOL;
    ///// @brief ��ȡ��ϵͳ���� (GUI/CUI ��)
    //auto WINAPI GetSubsystem() const->WORD;
    //// ����Ŀ¼����
    ///// @brief ��ȡָ������Ŀ¼�� (�����/������/��Դ���)
    ///// @param index Ŀ¼������ (�� IMAGE_DIRECTORY_ENTRY_IMPORT)
    //auto WINAPI GetDataDirectory(DWORD index) const->IMAGE_DATA_DIRECTORY;
    //// ��Դ����
    ///// @brief ��ȡָ����Դ����
    ///// @param type ��Դ���� (�� RT_ICON)
    ///// @param id ��Դ ID
    //auto WINAPI ExtractResource(LPCSTR type, WORD id) const->Byteset;
    ///// @brief �滻ָ����Դ����
    //auto WINAPI ReplaceResource(LPCSTR type, WORD id, const Byteset& newData)->BOOL;
    //// ��������
    ///// @brief ��ȡ���е��뺯����Ϣ (DLL���� -> �����б�)
    //auto WINAPI GetImports() const->std::map<std::string, std::vector<std::string>>;
    ///// @brief ����µĵ��뺯��
    //auto WINAPI AddImportFunction(LPCSTR dllName, LPCSTR funcName)->BOOL;
    //// ���������
    ///// @brief ��ȡ���������б�
    //auto WINAPI GetExports() const->std::vector<std::string>;
    //// �ض�λ����
    ///// @brief ��ȡ�ض�λ����Ϣ
    //auto WINAPI GetRelocations() const->std::vector<IMAGE_BASE_RELOCATION>;
    //// ������Ϣ
    ///// @brief ��ȡ������ϢĿ¼ (�� PDB ·��)
    //auto WINAPI GetDebugInfo() const->IMAGE_DEBUG_DIRECTORY;
    //// TLS ����
    ///// @brief ��ȡ TLS �ص�������ַ�б�
    //auto WINAPI GetTlsCallbacks() const->std::vector<DWORD>;
    //// У�����ǩ��
    ///// @brief ���㲢�޸� PE У���
    //auto WINAPI FixChecksum()->BOOL;
    ///// @brief ��֤����ǩ����Ч��
    //auto WINAPI VerifyDigitalSignature() const->BOOL;
    //// ��������
    ///// @brief ����½��� (���ڴ���ע��/������չ)
    ///// @param name ������ (8�ַ�����)
    ///// @param size ������С
    ///// @param characteristics ��������
    //auto WINAPI AddSection(LPCSTR name, DWORD size, DWORD characteristics)->BOOL;
    ///// @brief �����Ʋ��ҽ���ͷ
    //auto WINAPI FindSection(LPCSTR name) const->PE_SECTION_HEADER;
    //// ��ַת��
    ///// @brief �� RVA ת��Ϊ�ļ�ƫ��
    //auto WINAPI RvaToFileOffset(DWORD rva) const->DWORD;
    /*
    * RVA ת��Ϊ�ļ�ƫ��
    * @param rva RVA ��ַ
    * @return �ļ�ƫ�Ƶ�ַ
    */
    DWORD RvaToFileOffset(DWORD rva);
private:
    File file;
    std::string filename;
    std::string dstFilename;
};