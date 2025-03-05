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
    * @brief 获取 PE 文件导入表信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件导入表信息
    */
    auto WINAPI GetImportTable(PE_ERROR* error = nullptr)->std::vector<PE_IMPORT_DESCRIPTOR>;
    /*
    * @brief 获取 PE 文件导出表信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件导出表信息
    */
    auto WINAPI GetExportTable(PE_ERROR* error = nullptr)->std::vector<PE_EXPORT_DIRECTORY>;
    /*
    * @brief 获取 PE 文件资源表信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件资源表信息
    */
    auto WINAPI GetResourceTable(PE_ERROR* error = nullptr)->std::vector<PE_RESOURCE_DIRECTORY>;
    /*
    * @brief 获取 PE 文件重定位表信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件重定位表信息
    */
    auto WINAPI GetRelocationTable(PE_ERROR* error = nullptr)->std::vector<PE_BASE_RELOCATION>;
    /*
    * @brief 获取 PE 文件调试信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件调试信息
    */
    auto WINAPI GetDebugInfo(PE_ERROR* error = nullptr)->PE_DEBUG_DIRECTORY;
    /*
    * @brief 获取 PE 文件 TLS 表信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件 TLS 表信息
    */
    auto WINAPI GetTlsTable(PE_ERROR* error = nullptr)->PE_TLS_DIRECTORY32;
    /*
    * @brief 获取 PE 文件 TLS 表信息
    * @param error 错误码指针，若为 nullptr 则不返回错误信息
    * @return PE 文件 TLS 表信息
    */
    auto WINAPI GetTlsTable64(PE_ERROR* error = nullptr)->PE_TLS_DIRECTORY64;
    //// ----------------- 新增方法声明 -----------------
    //// 基础信息获取
    ///// @brief 获取入口点 RVA 地址
    //auto WINAPI GetEntryPointRVA() const->DWORD;
    ///// @brief 获取镜像基址 (ImageBase)
    //auto WINAPI GetImageBase() const->ULONGLONG;
    ///// @brief 判断是否为 64 位 PE 文件
    //auto WINAPI Is64Bit() const->BOOL;
    ///// @brief 获取子系统类型 (GUI/CUI 等)
    //auto WINAPI GetSubsystem() const->WORD;
    //// 数据目录操作
    ///// @brief 获取指定数据目录项 (导入表/导出表/资源表等)
    ///// @param index 目录项索引 (如 IMAGE_DIRECTORY_ENTRY_IMPORT)
    //auto WINAPI GetDataDirectory(DWORD index) const->IMAGE_DATA_DIRECTORY;
    //// 资源操作
    ///// @brief 提取指定资源数据
    ///// @param type 资源类型 (如 RT_ICON)
    ///// @param id 资源 ID
    //auto WINAPI ExtractResource(LPCSTR type, WORD id) const->Byteset;
    ///// @brief 替换指定资源数据
    //auto WINAPI ReplaceResource(LPCSTR type, WORD id, const Byteset& newData)->BOOL;
    //// 导入表操作
    ///// @brief 获取所有导入函数信息 (DLL名称 -> 函数列表)
    //auto WINAPI GetImports() const->std::map<std::string, std::vector<std::string>>;
    ///// @brief 添加新的导入函数
    //auto WINAPI AddImportFunction(LPCSTR dllName, LPCSTR funcName)->BOOL;
    //// 导出表操作
    ///// @brief 获取导出函数列表
    //auto WINAPI GetExports() const->std::vector<std::string>;
    //// 重定位操作
    ///// @brief 获取重定位表信息
    //auto WINAPI GetRelocations() const->std::vector<IMAGE_BASE_RELOCATION>;
    //// 调试信息
    ///// @brief 获取调试信息目录 (如 PDB 路径)
    //auto WINAPI GetDebugInfo() const->IMAGE_DEBUG_DIRECTORY;
    //// TLS 操作
    ///// @brief 获取 TLS 回调函数地址列表
    //auto WINAPI GetTlsCallbacks() const->std::vector<DWORD>;
    //// 校验和与签名
    ///// @brief 计算并修复 PE 校验和
    //auto WINAPI FixChecksum()->BOOL;
    ///// @brief 验证数字签名有效性
    //auto WINAPI VerifyDigitalSignature() const->BOOL;
    //// 节区操作
    ///// @brief 添加新节区 (用于代码注入/数据扩展)
    ///// @param name 节区名 (8字符限制)
    ///// @param size 节区大小
    ///// @param characteristics 节区属性
    //auto WINAPI AddSection(LPCSTR name, DWORD size, DWORD characteristics)->BOOL;
    ///// @brief 按名称查找节区头
    //auto WINAPI FindSection(LPCSTR name) const->PE_SECTION_HEADER;
    //// 地址转换
    ///// @brief 将 RVA 转换为文件偏移
    //auto WINAPI RvaToFileOffset(DWORD rva) const->DWORD;
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
};