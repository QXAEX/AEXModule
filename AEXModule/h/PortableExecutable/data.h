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
    IMPORT_DESCRIPTOR_INVALID = 23,
    NO_RESOURCE_TABLE = 24,
    INVALID_RVA = 25,
    READ_FILE = 26,
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
// ---------------------------- 节区属性标志位 ----------------------------
// 标准 PE 节区属性标志位定义 (完整列表)
#define PE_SECTION_FLAG_CNT_CODE              0x00000020  // 节区包含可执行代码
#define PE_SECTION_FLAG_CNT_INITIALIZED_DATA  0x00000040  // 节区包含已初始化数据
#define PE_SECTION_FLAG_CNT_UNINITIALIZED_DATA 0x00000080 // 节区包含未初始化数据
#define PE_SECTION_FLAG_MEM_DISCARDABLE       0x02000000  // 可丢弃
#define PE_SECTION_FLAG_MEM_NOT_CACHED        0x04000000  // 不可缓存
#define PE_SECTION_FLAG_MEM_NOT_PAGED         0x08000000  // 不可分页
#define PE_SECTION_FLAG_MEM_SHARED            0x10000000  // 可共享
#define PE_SECTION_FLAG_MEM_EXECUTE           0x20000000  // 可执行
#define PE_SECTION_FLAG_MEM_READ              0x40000000  // 可读
#define PE_SECTION_FLAG_MEM_WRITE             0x80000000  // 可写

// ---------------------------- 节区属性包装器 ----------------------------
typedef struct PE_SECTION_ATTRIBUTES {
    DWORD Flags;  // 实际存储的位掩码
    // 属性：可执行
    void SetExecutable(bool val) { val ? Flags |= PE_SECTION_FLAG_MEM_EXECUTE : Flags &= ~PE_SECTION_FLAG_MEM_EXECUTE; }
    // 是否 可执行
    bool IsExecutable() const { return Flags & PE_SECTION_FLAG_MEM_EXECUTE; }
    // 属性：可写
    void SetWritable(bool val) { val ? Flags |= PE_SECTION_FLAG_MEM_WRITE : Flags &= ~PE_SECTION_FLAG_MEM_WRITE; }
    // 是否 可写
    bool IsWritable() const { return Flags & PE_SECTION_FLAG_MEM_WRITE; }
    // 属性：可读
    void SetReadable(bool val) { val ? Flags |= PE_SECTION_FLAG_MEM_READ : Flags &= ~PE_SECTION_FLAG_MEM_READ; }
    // 是否 可读
    bool IsReadable() const { return Flags & PE_SECTION_FLAG_MEM_READ; }
    // 属性：共享
    void SetShared(bool val) { val ? Flags |= PE_SECTION_FLAG_MEM_SHARED : Flags &= ~PE_SECTION_FLAG_MEM_SHARED; }
    // 是否 可共享
    bool IsShared() const { return Flags & PE_SECTION_FLAG_MEM_SHARED; }
    // 属性：不可分页
    void SetNotPaged(bool val) { val ? Flags |= PE_SECTION_FLAG_MEM_NOT_PAGED : Flags &= ~PE_SECTION_FLAG_MEM_NOT_PAGED; }
    // 是否 非分页
    bool IsNotPaged() const { return Flags & PE_SECTION_FLAG_MEM_NOT_PAGED; }
    // 属性：不可缓存
    void SetNotCached(bool val) { val ? Flags |= PE_SECTION_FLAG_MEM_NOT_CACHED : Flags &= ~PE_SECTION_FLAG_MEM_NOT_CACHED; }
    // 是否 非缓存
    bool IsNotCached() const { return Flags & PE_SECTION_FLAG_MEM_NOT_CACHED; }
    // 属性：可丢弃
    void SetDiscardable(bool val) { val ? Flags |= PE_SECTION_FLAG_MEM_DISCARDABLE : Flags &= ~PE_SECTION_FLAG_MEM_DISCARDABLE; }
    // 是否 可丢弃
    bool IsDiscardable() const { return Flags & PE_SECTION_FLAG_MEM_DISCARDABLE; }
    // 属性：包含代码
    void SetContainsCode(bool val) { val ? Flags |= PE_SECTION_FLAG_CNT_CODE : Flags &= ~PE_SECTION_FLAG_CNT_CODE; }
    // 是否 包含代码
    bool ContainsCode() const { return Flags & PE_SECTION_FLAG_CNT_CODE; }
    // 属性：包含已初始化数据
    void SetContainsInitializedData(bool val) { val ? Flags |= PE_SECTION_FLAG_CNT_INITIALIZED_DATA : Flags &= ~PE_SECTION_FLAG_CNT_INITIALIZED_DATA; }
    // 是否 包含已初始化数据
    bool ContainsInitializedData() const { return Flags & PE_SECTION_FLAG_CNT_INITIALIZED_DATA; }
    // 属性：包含未初始化数据
    void SetContainsUninitializedData(bool val) { val ? Flags |= PE_SECTION_FLAG_CNT_UNINITIALIZED_DATA : Flags &= ~PE_SECTION_FLAG_CNT_UNINITIALIZED_DATA; }
    // 是否 包含未初始化数据
    bool ContainsUninitializedData() const { return Flags & PE_SECTION_FLAG_CNT_UNINITIALIZED_DATA; }
} *PPE_SECTION_ATTRIBUTES;

// ---------------------------- 节区头 ----------------------------
typedef struct PE_SECTION_HEADER {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];  // 使用标准定义
    union {
        DWORD   PhysicalAddress;  // 物理地址（已弃用）
        DWORD   VirtualSize;       // 内存中的总大小
    } Misc;
    DWORD   VirtualAddress;        // 内存中的起始 RVA
    DWORD   SizeOfRawData;         // 文件中的实际大小
    DWORD   PointerToRawData;      // 文件偏移
    DWORD   PointerToRelocations;  // 重定位表偏移（已弃用）
    DWORD   PointerToLinenumbers;   // 行号表偏移（已弃用）
    WORD    NumberOfRelocations;   // 重定位项数（已弃用）
    WORD    NumberOfLinenumbers;   // 行号项数（已弃用）
    DWORD   Characteristics;       // 使用标准 DWORD 类型存储标志位
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

// 资源目录条目
typedef struct PE_RESOURCE_DIRECTORY_ENTRY {
    union {
        struct {
            DWORD NameOffset : 31;
            DWORD NameIsString : 1;
        };
        DWORD Id;
    };
    union {
        DWORD OffsetToData;
        DWORD OffsetToDirectory;
    };

    std::wstring NameString;    // UNICODE字符串名称
    bool IsDirectory;           // 是否为子目录
    bool IsNamed;               // 是否为命名资源
} *PPE_RESOURCE_DIRECTORY_ENTRY;

// 资源目录表
typedef struct PE_RESOURCE_DIRECTORY {
    DWORD Characteristics;
    DWORD TimeDateStamp;
    WORD MajorVersion;
    WORD MinorVersion;
    WORD NumberOfNamedEntries;
    WORD NumberOfIdEntries;

    std::vector<PE_RESOURCE_DIRECTORY_ENTRY> Entries;
} *PPE_RESOURCE_DIRECTORY;

#pragma pack(pop)