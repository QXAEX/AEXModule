#pragma once
#include <windows.h>
#include <string>
#include <vector>

// ---------------------------- 错误码枚举 ----------------------------
typedef enum PE_ERROR : DWORD {
    SUCCESS = 0,
    FILE_NOT_OPEN = 1,
    DOS_HEADER_INVALID = 2,
    NT_HEADER_INVALID = 3,
    NOT_SUPPORTED_ARCHITECTURE = 4,
    SECTION_HEADER_INVALID = 5,
    OPTIONAL_HEADER_MAGIC_INVALID = 6,
    TOO_MANY_SECTIONS = 7,
    SECTION_OFFSET_INVALID = 8,
    SECTION_SIZE_INVALID = 9,
    TOO_MANY_DATA_DIRECTORIES = 10,
    DIRECTORY_ENTRY_INVALID = 11,
    NO_SECTIONS = 12,
    INVALID_SECTION_NAME = 13,
    DATA_DIRECTORY_INVALID = 14,
    RVA_CONVERSION_FAILED = 15,
    EXPORT_DIRECTORY_INVALID = 16,
    EXPORT_FUNCTION_TABLE_INVALID = 17,
    EXPORT_NAME_TABLE_INVALID = 18,
    EXPORT_ORDINAL_TABLE_INVALID = 19,
    EXPORT_ORDINAL_OUT_OF_RANGE = 20,
    IMPORT_FUNCTION_INVALID = 21,
    INVALID_IMPORT_NAME = 22,
    IMPORT_DESCRIPTOR_INVALID = 23
} PE_ERROR;

#pragma pack(push, 1)

// ---------------------------- 跨平台地址兼容定义 ----------------------------
typedef ULONGLONG PE_RVA;   // 统一使用 64 位保存 RVA（兼容大地址文件）
typedef ULONGLONG PE_PTR;    // 统一指针类型（兼容 32/64 位）

// ---------------------------- PE 头结构体 ----------------------------
typedef struct PE_HEADER {
    IMAGE_DOS_HEADER H_DOS;  // DOS 头（必须为二进制兼容）

    struct {
        DWORD Signature;     // PE 签名 "PE\0\0"
        IMAGE_FILE_HEADER FileHeader;

        union {
            IMAGE_OPTIONAL_HEADER32 H32;  // 32 位可选头
            IMAGE_OPTIONAL_HEADER64 H64;  // 64 位可选头
        } Optional;
    } H_NT;
} *PPE_HEADER;

// ---------------------------- 节区头（完整字段） ----------------------------
typedef struct PE_SECTION_HEADER {
    BYTE    Name[8];                     // 节区名称（ASCII）
    union {
        DWORD   PhysicalAddress;         // 物理地址（已弃用）
        DWORD   VirtualSize;             // 内存中的总大小
    } Misc;
    DWORD   VirtualAddress;              // 内存中的起始 RVA（DWORD 为 PE 规范定义）
    DWORD   SizeOfRawData;               // 文件中的实际大小
    DWORD   PointerToRawData;            // 文件偏移（DWORD 为 PE 规范定义）
    DWORD   PointerToRelocations;        // 重定位表偏移（已弃用）
    DWORD   PointerToLinenumbers;        // 行号表偏移（已弃用）
    WORD    NumberOfRelocations;         // 重定位项数（已弃用）
    WORD    NumberOfLinenumbers;         // 行号项数（已弃用）
    DWORD   Characteristics;             // 节区属性标志
} *PPE_SECTION_HEADER;

// ---------------------------- 导入函数结构体（完整字段） ----------------------------
typedef struct PE_IMPORT_FUNCTION {
    bool    IsOrdinal;                  // 是否为序号导入
    WORD    Ordinal;                    // 序号值
    WORD    Hint;                       // 提示值
    std::string Name;                   // 函数名称（若按名称导入）
};

// ---------------------------- 导出函数结构体（完整字段） ----------------------------
typedef struct PE_EXPORT_FUNCTION {
    std::string Name;                   // 函数名称（可能为空）
    WORD        Ordinal;                // 导出序号（相对 Base）
    PE_RVA      AddressRVA;             // 函数 RVA（兼容 32/64 位）
};

// ---------------------------- 导入表结构体（完整字段） ----------------------------
typedef struct PE_IMPORT_DESCRIPTOR {
    union {
        DWORD   Characteristics;        // 0 表示终止
        PE_RVA  OriginalFirstThunk;     // 原始 IAT RVA（兼容 32/64 位）
    };
    DWORD   TimeDateStamp;              // 时间戳
    DWORD   ForwarderChain;             // 转发链（-1 表示无转发）
    PE_RVA  NameRVA;                    // DLL 名称的 RVA（兼容 32/64 位）
    PE_RVA  FirstThunk;                 // IAT RVA（兼容 32/64 位）

    // 解析后的运行时数据（不参与二进制布局）
    std::string DLLName;                // 通过 NameRVA 解析
    std::vector<PE_IMPORT_FUNCTION> Functions;
} *PPE_IMPORT_DESCRIPTOR;

// ---------------------------- 导出表结构体（完整字段） ----------------------------
typedef struct PE_EXPORT_DIRECTORY {
    DWORD   Characteristics;            // 保留字段（通常为 0）
    DWORD   TimeDateStamp;              // 时间戳
    WORD    MajorVersion;               // 主版本号
    WORD    MinorVersion;               // 次版本号
    PE_RVA  NameRVA;                    // 模块名称的 RVA（兼容 32/64 位）
    DWORD   Base;                       // 导出序号基址（PE 规范为 DWORD）
    DWORD   NumberOfFunctions;          // 导出函数总数
    DWORD   NumberOfNames;              // 带名称的导出函数数
    PE_RVA  AddressOfFunctions;         // 函数地址表 RVA
    PE_RVA  AddressOfNames;             // 函数名称表 RVA
    PE_RVA  AddressOfNameOrdinals;      // 函数序号表 RVA

    // 解析后的运行时数据（不参与二进制布局）
    std::string ModuleName;             // 通过 NameRVA 解析
    std::vector<PE_EXPORT_FUNCTION> Functions;
} *PPE_EXPORT_DIRECTORY;
// ---------------------------- 资源目录结构体 ----------------------------
typedef struct PE_RESOURCE_DIRECTORY {
    DWORD   Characteristics;        // 资源目录特征（通常为0）
    DWORD   TimeDateStamp;          // 资源目录时间戳
    WORD    MajorVersion;           // 主版本号（资源格式版本）
    WORD    MinorVersion;           // 次版本号（资源格式版本）
    WORD    NumberOfNamedEntries;   // 以名称命名的条目数量
    WORD    NumberOfIdEntries;      // 以ID命名的条目数量
    //  IMAGE_RESOURCE_DIRECTORY_ENTRY DirectoryEntries[]; // 目录条目数组（动态长度）
} *PPE_RESOURCE_DIRECTORY;

// ---------------------------- 基址重定位结构体 ----------------------------
typedef struct PE_BASE_RELOCATION {
    DWORD   VirtualAddress;         // 重定位块的起始 RVA
    DWORD   SizeOfBlock;            // 重定位块总大小（包含此结构体和后续TypeOffset数组）
    //  WORD    TypeOffset[1];       // 重定位项数组（高4位类型，低12位偏移）
} *PPE_BASE_RELOCATION;

// ---------------------------- 调试目录结构体 ----------------------------
typedef struct PE_DEBUG_DIRECTORY {
    DWORD   Characteristics;        // 调试信息特征（保留字段，通常为0）
    DWORD   TimeDateStamp;          // 调试信息生成时间戳
    WORD    MajorVersion;           // 调试格式主版本号
    WORD    MinorVersion;           // 调试格式次版本号
    DWORD   Type;                   // 调试信息类型（如 IMAGE_DEBUG_TYPE_CODEVIEW）
    DWORD   SizeOfData;             // 调试数据大小（字节）
    DWORD   AddressOfRawData;       // 调试数据 RVA（内存地址）
    DWORD   PointerToRawData;       // 调试数据文件偏移（若存在）
} *PPE_DEBUG_DIRECTORY;

// ---------------------------- 32位 TLS 目录结构体 ----------------------------
typedef struct PE_TLS_DIRECTORY32 {
    DWORD   StartAddressOfRawData;  // TLS 初始化数据的起始文件偏移
    DWORD   EndAddressOfRawData;    // TLS 初始化数据的结束文件偏移（独占）
    DWORD   AddressOfIndex;         // TLS 索引变量的 RVA（指向 DWORD）
    DWORD   AddressOfCallBacks;     // TLS 回调函数数组的 RVA（PIMAGE_TLS_CALLBACK*）
    DWORD   SizeOfZeroFill;         // 零填充区域大小
    union {
        DWORD Characteristics;      // TLS 目录属性（位标志）
        struct {
            DWORD Reserved0 : 20;   // 保留位（必须为0）
            DWORD Alignment : 4;    // 内存对齐方式（2^Alignment，如 3 表示 8 字节对齐）
            DWORD Reserved1 : 8;    // 保留位（必须为0）
        } DUMMYSTRUCTNAME;          // 位域命名
    } DUMMYUNIONNAME;               // 联合体命名
} *PPE_TLS_DIRECTORY32;

// ---------------------------- 64位 TLS 目录结构体 ----------------------------
typedef struct PE_TLS_DIRECTORY64 {
    ULONGLONG StartAddressOfRawData;// TLS 初始化数据的起始文件偏移（64位扩展）
    ULONGLONG EndAddressOfRawData;  // TLS 初始化数据的结束文件偏移（64位扩展）
    ULONGLONG AddressOfIndex;       // TLS 索引变量的 RVA（64位指针）
    ULONGLONG AddressOfCallBacks;   // TLS 回调函数数组的 RVA（64位指针）
    DWORD SizeOfZeroFill;           // 零填充区域大小（与32位相同）
    union {
        DWORD Characteristics;      // TLS 目录属性（位标志，同32位）
        struct {
            DWORD Reserved0 : 20;   // 保留位（必须为0）
            DWORD Alignment : 4;    // 内存对齐方式（2^Alignment）
            DWORD Reserved1 : 8;    // 保留位（必须为0）
        } DUMMYSTRUCTNAME;          // 位域命名
    } DUMMYUNIONNAME;               // 联合体命名
} *PPE_TLS_DIRECTORY64;
#pragma pack(pop)