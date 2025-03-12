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
    * @brief 加载 PE 文件
    * @param filename PE 文件名(xx.dll/xx.exe/xx.sys)
    * @return 成功返回 TRUE，失败返回 FALSE
    */
    auto WINAPI Import(LPCSTR filename)->BOOL;
    /*
    * @brief 导出 PE 文件
    * @param filename PE 文件名(xx.dll/xx.exe/xx.sys)
    * @return 成功返回 TRUE，失败返回 FALSE
    */
    auto WINAPI Export(LPCSTR filename)->BOOL;
    /*
    * @brief 是否是64位PE文件
    * @param header PE文件头信息
    * @return 64位返回true，否则返回false
    */
    auto WINAPI is64bit(const PE_HEADER& header) -> bool;
    /*
    * @brief 验证 PE 文件是否有效
    * @return 返回错误码 0则为有效
    */
    auto WINAPI Validate()->PE_ERROR;
    /*
    * @brief 获取 PE 文件头信息
    * @return PE 文件头信息
    */
    auto WINAPI GetHeader()->PE_HEADER;
    /*
    * @brief 获取 PE 文件节区信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件节区信息
    */
    auto WINAPI GetSectionHeaders(PE_ERROR* error = nullptr)->std::vector<PE_SECTION_HEADER>;
    /*
    * @brief PE加文件节区信息
    * @param sectionName 节区名称, 最大长度为 8 字节
    * @param virtualSize 节区虚拟大小, 最大4 GB
    * @param rawSize 节区大小, 最大4 GB
    * @param characteristics 节区属性
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件节区信息
    */
    auto WINAPI AddSectionHeader(LPCSTR sectionName, DWORD virtualSize, DWORD rawSize, DWORD characteristics, PE_ERROR* error = nullptr)->BOOL;
    /*
    * @brief PE删除文件节区信息
    * @param sectionName 节区名称, 最大长度为 8 字节
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件节区信息
    */
    auto WINAPI DeleteSectionHeader(LPCSTR sectionName, PE_ERROR* error = nullptr)->BOOL;
    /*
    * @brief 获取 PE 文件节区信息
    * @param name 节区名称
    * @param section 节区信息指针
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return 成功返回 TRUE，失败返回 FALSE
    */
    auto WINAPI GetSectionByName(LPCSTR name, PE_SECTION_HEADER* section, PE_ERROR* error)->BOOL;
    /*
    * @brief 寻找最大节区索引
    * @param baseName 节区名称前缀
    * @return 最大节区索引
    */
    auto WINAPI FindMaxSectionIndex(const std::string& baseName) -> int;
    /*
    * @brief PE修改文件节区信息
    * @param oldName 旧节区名称，最大长度为 8 字节
    * @param newName 新节区名称，最大长度为 8 字节，传 nullptr 则不修改名称
    * @param newVirtualSize 新节区虚拟大小，最大4 GB，传 0 则不修改大小
    * @param newRawSize 新节区大小，最大4 GB，传 0 则不修改大小
    * @param newCharacteristics 新节区属性，传 0 则不修改属性
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件节区信息
    */
    auto WINAPI ModifySectionHeader(LPCSTR oldName, LPCSTR newName = nullptr, DWORD newVirtualSize = NULL, DWORD newRawSize = NULL, DWORD newCharacteristics = NULL, PE_ERROR* error = nullptr)->BOOL;
    /*
    * @brief 获取 PE 文件导入表信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件导入表信息
    */
    auto WINAPI GetImportTable(PE_ERROR* error = nullptr)->std::vector<PE_IMPORT_DESCRIPTOR>;
    /*
    * @brief PE添加导入表信息
    * @param dllName 导入库名，最大长度为 260 字节
    * @param Functions 导入函数列表，若为空则无函数导入dll
    * @param sectionName 节区名称，最大长度为 8 字节，传 nullptr 则使用默认节区名称 ".aexdt"
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件导入表信息
    */
    //auto WINAPI AddImportTable(LPCSTR dllName, std::vector<PE_IMPORT_FUNCTION> Functions =   std::vector<PE_IMPORT_FUNCTION>(), LPCSTR sectionName = ".aexdt", PE_ERROR* error = nullptr)->BOOL;
    /*
    * @brief 获取 PE 文件导出表信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件导出表信息
    */
    auto WINAPI GetExportTable(PE_ERROR* error = nullptr)->std::vector<PE_EXPORT_DIRECTORY>;
    /*
    * RVA 转换为文件偏移
    * @param rva RVA 地址
    * @return 文件偏移地址
    */
    DWORD RvaToFileOffset(DWORD rva);
private:
    File file;
    std::string filename;
    std::string dstFilename;
private:
    BOOL ErrorCheck(PE_ERROR* error, PE_ERROR code) {
        if (error) *error = code;
        return FALSE;
    }
    /*
    * @brief 转换 PE 文件头信息
    * @param peHeader PE 文件头信息
    * @return IMAGE_FILE_HEADER
    */
    static IMAGE_SECTION_HEADER PE_SECTION_HEADER_to_IMAGE(const PE_SECTION_HEADER& peSec);
    /*
    * @brief 转换 IMAGE_SECTION_HEADER
    * @param imgSec IMAGE_SECTION_HEADER
    * @return PE_SECTION_HEADER
    */
    static PE_SECTION_HEADER IMAGE_to_PE_SECTION_HEADER(const IMAGE_SECTION_HEADER& imgSec);
    /*
    * @brief 分配空间
    * @param neededSize 需要分配的空间大小
    * @param outOffset 输出分配的偏移地址
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return 成功返回 TRUE，失败返回 FALSE
    */
    BOOL AllocateSpace(DWORD neededSize, DWORD* outOffset, PE_ERROR* error = nullptr);
    /*
    * @brief 计算对齐值
    * @param value 值
    * @param alignment 对齐值
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return 对齐后的值
    */
    DWORD AlignValue(DWORD value, DWORD alignment, PE_ERROR* error = nullptr);
    /*
    * @brief 文件偏移到rva
    * @param fileOffset 文件偏移地址
    * @return RVA地址
    */
    DWORD FileOffsetToRva(DWORD fileOffset);
    /*
    * @brief 更新数据目录
    * @param index 数据目录索引
    * @param dir 数据目录
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return 成功返回 TRUE，失败返回 FALSE
    */
    BOOL UpdateDataDirectory(int index, IMAGE_DATA_DIRECTORY* dir, PE_ERROR* error);
    /*
    * @brief 计算导入表大小
    * @param dllName 导入库名
    * @return 导入表大小
    */
    DWORD CalculateImportTableSize(LPCSTR dllName);
    /*
    * @brief 寻找导入表结束位置
    * @param data 字节集
    * @return 导入表结束位置
    */
    DWORD FindImportDescriptorEnd(const Byteset& data);
    /*
    * @brief 分配 PE 文件导入表空间
    * @param size 导入表大小
    * @param outOffset 输出分配的偏移地址
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return 成功返回 TRUE，失败返回 FALSE
    */
    BOOL AllocateImportSpace(DWORD size, DWORD* outOffset, PE_ERROR* error);

    // 集成数据目录获取
    IMAGE_DATA_DIRECTORY& GetDataDirectory(int index);
};