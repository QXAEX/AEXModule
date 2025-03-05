#pragma once
#include <windows.h>
#include <string>
#include <vector>

// ---------------------------- ������ö�� ----------------------------
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

// ---------------------------- ��ƽ̨��ַ���ݶ��� ----------------------------
typedef ULONGLONG PE_RVA;   // ͳһʹ�� 64 λ���� RVA�����ݴ��ַ�ļ���
typedef ULONGLONG PE_PTR;    // ͳһָ�����ͣ����� 32/64 λ��

// ---------------------------- PE ͷ�ṹ�� ----------------------------
typedef struct PE_HEADER {
    IMAGE_DOS_HEADER H_DOS;  // DOS ͷ������Ϊ�����Ƽ��ݣ�

    struct {
        DWORD Signature;     // PE ǩ�� "PE\0\0"
        IMAGE_FILE_HEADER FileHeader;

        union {
            IMAGE_OPTIONAL_HEADER32 H32;  // 32 λ��ѡͷ
            IMAGE_OPTIONAL_HEADER64 H64;  // 64 λ��ѡͷ
        } Optional;
    } H_NT;
} *PPE_HEADER;

// ---------------------------- ����ͷ�������ֶΣ� ----------------------------
typedef struct PE_SECTION_HEADER {
    BYTE    Name[8];                     // �������ƣ�ASCII��
    union {
        DWORD   PhysicalAddress;         // �����ַ�������ã�
        DWORD   VirtualSize;             // �ڴ��е��ܴ�С
    } Misc;
    DWORD   VirtualAddress;              // �ڴ��е���ʼ RVA��DWORD Ϊ PE �淶���壩
    DWORD   SizeOfRawData;               // �ļ��е�ʵ�ʴ�С
    DWORD   PointerToRawData;            // �ļ�ƫ�ƣ�DWORD Ϊ PE �淶���壩
    DWORD   PointerToRelocations;        // �ض�λ��ƫ�ƣ������ã�
    DWORD   PointerToLinenumbers;        // �кű�ƫ�ƣ������ã�
    WORD    NumberOfRelocations;         // �ض�λ�����������ã�
    WORD    NumberOfLinenumbers;         // �к������������ã�
    DWORD   Characteristics;             // �������Ա�־
} *PPE_SECTION_HEADER;

// ---------------------------- ���뺯���ṹ�壨�����ֶΣ� ----------------------------
typedef struct PE_IMPORT_FUNCTION {
    bool    IsOrdinal;                  // �Ƿ�Ϊ��ŵ���
    WORD    Ordinal;                    // ���ֵ
    WORD    Hint;                       // ��ʾֵ
    std::string Name;                   // �������ƣ��������Ƶ��룩
};

// ---------------------------- ���������ṹ�壨�����ֶΣ� ----------------------------
typedef struct PE_EXPORT_FUNCTION {
    std::string Name;                   // �������ƣ�����Ϊ�գ�
    WORD        Ordinal;                // ������ţ���� Base��
    PE_RVA      AddressRVA;             // ���� RVA������ 32/64 λ��
};

// ---------------------------- �����ṹ�壨�����ֶΣ� ----------------------------
typedef struct PE_IMPORT_DESCRIPTOR {
    union {
        DWORD   Characteristics;        // 0 ��ʾ��ֹ
        PE_RVA  OriginalFirstThunk;     // ԭʼ IAT RVA������ 32/64 λ��
    };
    DWORD   TimeDateStamp;              // ʱ���
    DWORD   ForwarderChain;             // ת������-1 ��ʾ��ת����
    PE_RVA  NameRVA;                    // DLL ���Ƶ� RVA������ 32/64 λ��
    PE_RVA  FirstThunk;                 // IAT RVA������ 32/64 λ��

    // �����������ʱ���ݣ�����������Ʋ��֣�
    std::string DLLName;                // ͨ�� NameRVA ����
    std::vector<PE_IMPORT_FUNCTION> Functions;
} *PPE_IMPORT_DESCRIPTOR;

// ---------------------------- ������ṹ�壨�����ֶΣ� ----------------------------
typedef struct PE_EXPORT_DIRECTORY {
    DWORD   Characteristics;            // �����ֶΣ�ͨ��Ϊ 0��
    DWORD   TimeDateStamp;              // ʱ���
    WORD    MajorVersion;               // ���汾��
    WORD    MinorVersion;               // �ΰ汾��
    PE_RVA  NameRVA;                    // ģ�����Ƶ� RVA������ 32/64 λ��
    DWORD   Base;                       // ������Ż�ַ��PE �淶Ϊ DWORD��
    DWORD   NumberOfFunctions;          // ������������
    DWORD   NumberOfNames;              // �����Ƶĵ���������
    PE_RVA  AddressOfFunctions;         // ������ַ�� RVA
    PE_RVA  AddressOfNames;             // �������Ʊ� RVA
    PE_RVA  AddressOfNameOrdinals;      // ������ű� RVA

    // �����������ʱ���ݣ�����������Ʋ��֣�
    std::string ModuleName;             // ͨ�� NameRVA ����
    std::vector<PE_EXPORT_FUNCTION> Functions;
} *PPE_EXPORT_DIRECTORY;
// ---------------------------- ��ԴĿ¼�ṹ�� ----------------------------
typedef struct PE_RESOURCE_DIRECTORY {
    DWORD   Characteristics;        // ��ԴĿ¼������ͨ��Ϊ0��
    DWORD   TimeDateStamp;          // ��ԴĿ¼ʱ���
    WORD    MajorVersion;           // ���汾�ţ���Դ��ʽ�汾��
    WORD    MinorVersion;           // �ΰ汾�ţ���Դ��ʽ�汾��
    WORD    NumberOfNamedEntries;   // ��������������Ŀ����
    WORD    NumberOfIdEntries;      // ��ID��������Ŀ����
    //  IMAGE_RESOURCE_DIRECTORY_ENTRY DirectoryEntries[]; // Ŀ¼��Ŀ���飨��̬���ȣ�
} *PPE_RESOURCE_DIRECTORY;

// ---------------------------- ��ַ�ض�λ�ṹ�� ----------------------------
typedef struct PE_BASE_RELOCATION {
    DWORD   VirtualAddress;         // �ض�λ�����ʼ RVA
    DWORD   SizeOfBlock;            // �ض�λ���ܴ�С�������˽ṹ��ͺ���TypeOffset���飩
    //  WORD    TypeOffset[1];       // �ض�λ�����飨��4λ���ͣ���12λƫ�ƣ�
} *PPE_BASE_RELOCATION;

// ---------------------------- ����Ŀ¼�ṹ�� ----------------------------
typedef struct PE_DEBUG_DIRECTORY {
    DWORD   Characteristics;        // ������Ϣ�����������ֶΣ�ͨ��Ϊ0��
    DWORD   TimeDateStamp;          // ������Ϣ����ʱ���
    WORD    MajorVersion;           // ���Ը�ʽ���汾��
    WORD    MinorVersion;           // ���Ը�ʽ�ΰ汾��
    DWORD   Type;                   // ������Ϣ���ͣ��� IMAGE_DEBUG_TYPE_CODEVIEW��
    DWORD   SizeOfData;             // �������ݴ�С���ֽڣ�
    DWORD   AddressOfRawData;       // �������� RVA���ڴ��ַ��
    DWORD   PointerToRawData;       // ���������ļ�ƫ�ƣ������ڣ�
} *PPE_DEBUG_DIRECTORY;

// ---------------------------- 32λ TLS Ŀ¼�ṹ�� ----------------------------
typedef struct PE_TLS_DIRECTORY32 {
    DWORD   StartAddressOfRawData;  // TLS ��ʼ�����ݵ���ʼ�ļ�ƫ��
    DWORD   EndAddressOfRawData;    // TLS ��ʼ�����ݵĽ����ļ�ƫ�ƣ���ռ��
    DWORD   AddressOfIndex;         // TLS ���������� RVA��ָ�� DWORD��
    DWORD   AddressOfCallBacks;     // TLS �ص���������� RVA��PIMAGE_TLS_CALLBACK*��
    DWORD   SizeOfZeroFill;         // ����������С
    union {
        DWORD Characteristics;      // TLS Ŀ¼���ԣ�λ��־��
        struct {
            DWORD Reserved0 : 20;   // ����λ������Ϊ0��
            DWORD Alignment : 4;    // �ڴ���뷽ʽ��2^Alignment���� 3 ��ʾ 8 �ֽڶ��룩
            DWORD Reserved1 : 8;    // ����λ������Ϊ0��
        } DUMMYSTRUCTNAME;          // λ������
    } DUMMYUNIONNAME;               // ����������
} *PPE_TLS_DIRECTORY32;

// ---------------------------- 64λ TLS Ŀ¼�ṹ�� ----------------------------
typedef struct PE_TLS_DIRECTORY64 {
    ULONGLONG StartAddressOfRawData;// TLS ��ʼ�����ݵ���ʼ�ļ�ƫ�ƣ�64λ��չ��
    ULONGLONG EndAddressOfRawData;  // TLS ��ʼ�����ݵĽ����ļ�ƫ�ƣ�64λ��չ��
    ULONGLONG AddressOfIndex;       // TLS ���������� RVA��64λָ�룩
    ULONGLONG AddressOfCallBacks;   // TLS �ص���������� RVA��64λָ�룩
    DWORD SizeOfZeroFill;           // ����������С����32λ��ͬ��
    union {
        DWORD Characteristics;      // TLS Ŀ¼���ԣ�λ��־��ͬ32λ��
        struct {
            DWORD Reserved0 : 20;   // ����λ������Ϊ0��
            DWORD Alignment : 4;    // �ڴ���뷽ʽ��2^Alignment��
            DWORD Reserved1 : 8;    // ����λ������Ϊ0��
        } DUMMYSTRUCTNAME;          // λ������
    } DUMMYUNIONNAME;               // ����������
} *PPE_TLS_DIRECTORY64;
#pragma pack(pop)