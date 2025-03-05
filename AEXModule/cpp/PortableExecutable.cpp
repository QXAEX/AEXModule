#include "../h/PortableExecutable.h"

#include <winnt.h>

PE::PE(LPCSTR filename)
{
    this->Import(filename);
}

std::string PE::ErrorToString(PE_ERROR error)
{
    switch (error) {
    case PE_ERROR::SUCCESS: return "成功";
    case PE_ERROR::FILE_NOT_OPEN: return "文件未打开";
    case PE_ERROR::DOS_HEADER_INVALID: return "DOS头无效";
    case PE_ERROR::NT_HEADER_INVALID: return "NT头无效";
    case PE_ERROR::NOT_SUPPORTED_ARCHITECTURE: return "不支持的架构";
    case PE_ERROR::SECTION_HEADER_INVALID: return "节区头无效";
    case PE_ERROR::OPTIONAL_HEADER_MAGIC_INVALID: return "可选头魔数无效";
    case PE_ERROR::TOO_MANY_SECTIONS: return "节区数量过多";
    case PE_ERROR::SECTION_OFFSET_INVALID: return "节区偏移越界";
    case PE_ERROR::SECTION_SIZE_INVALID: return "节区大小无效";
    case PE_ERROR::TOO_MANY_DATA_DIRECTORIES: return "数据目录项过多";
    case PE_ERROR::DIRECTORY_ENTRY_INVALID: return "目录项地址无效";
    case PE_ERROR::NO_SECTIONS: return "没有节区";
    case PE_ERROR::INVALID_SECTION_NAME: return "节区名无效";
    case PE_ERROR::DATA_DIRECTORY_INVALID: return "数据目录无效";
    case PE_ERROR::RVA_CONVERSION_FAILED: return "RVA转换失败";
    case PE_ERROR::EXPORT_DIRECTORY_INVALID: return "导出目录无效";
    case PE_ERROR::EXPORT_FUNCTION_TABLE_INVALID: return "导出函数表无效";
    case PE_ERROR::EXPORT_NAME_TABLE_INVALID: return "导出名称表无效";
    case PE_ERROR::EXPORT_ORDINAL_TABLE_INVALID: return "导出序号表无效";
    case PE_ERROR::EXPORT_ORDINAL_OUT_OF_RANGE: return "导出序号越界";
    case PE_ERROR::IMPORT_FUNCTION_INVALID: return "导入函数无效";
    case PE_ERROR::INVALID_IMPORT_NAME: return "导入函数名无效";
    case PE_ERROR::IMPORT_DESCRIPTOR_INVALID: return "导入描述符无效";
    default: return "未知错误";
    }
}

auto __stdcall PE::Import(LPCSTR filename) -> BOOL
{
    //判断文件是否存在
    if (!this->file.Exists(filename)) {
        return FALSE;
    }
    //复制文件
    this->filename = std::string(filename);
    this->dstFilename = std::string(filename) + ".bak";
    DeleteFileA(this->dstFilename.c_str());
    this->file.Copy(filename, this->dstFilename);
    return this->file.Open(this->dstFilename);
}

auto __stdcall PE::Export(LPCSTR filename) -> BOOL
{
    return this->file.Copy(this->dstFilename, filename);
}

auto __stdcall PE::Validate() -> PE_ERROR {
    // 1. 检查文件是否已打开
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) return PE_ERROR::FILE_NOT_OPEN;
    // 2. 验证 DOS 头
    PE_HEADER header = this->GetHeader();
    if (header.H_DOS.e_magic != IMAGE_DOS_SIGNATURE) return PE_ERROR::DOS_HEADER_INVALID;
    // 3. 验证 NT 头签名
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) return PE_ERROR::NT_HEADER_INVALID;
    // 4. 验证文件头架构
    const WORD machine = header.H_NT.FileHeader.Machine;
    if (machine != IMAGE_FILE_MACHINE_I386 &&
        machine != IMAGE_FILE_MACHINE_AMD64 &&
        machine != IMAGE_FILE_MACHINE_IA64) return PE_ERROR::NOT_SUPPORTED_ARCHITECTURE;
    // 5. 验证可选头魔数
    const WORD magic = header.H_NT.Optional.H32.Magic;
    if (magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
        magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) return PE_ERROR::OPTIONAL_HEADER_MAGIC_INVALID;
    // 6. 验证节区数量合理性
    const WORD numSections = header.H_NT.FileHeader.NumberOfSections;
    // 一般PE文件不会超过100个节区
    if (numSections > 100) return PE_ERROR::TOO_MANY_SECTIONS;
    // 7. 验证节区数据有效性
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(this->file.hfile, &fileSize)) return PE_ERROR::SECTION_HEADER_INVALID;
    PE_ERROR er;
    std::vector<PE_SECTION_HEADER> sections = GetSectionHeaders(&er);
    if (er != PE_ERROR::SUCCESS) return er;
    for (const auto& section : sections) {
        // 检查节区文件偏移有效性
        if (section.PointerToRawData > fileSize.QuadPart) return PE_ERROR::SECTION_HEADER_INVALID;
        // 检查节区大小有效性
        if (section.SizeOfRawData > (DWORD)(fileSize.QuadPart - section.PointerToRawData)) return PE_ERROR::SECTION_SIZE_INVALID;
    }
    // 8. 验证数据目录项
    const IMAGE_DATA_DIRECTORY* directories = nullptr;
    DWORD dirCount = 0;
    if (magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        directories = header.H_NT.Optional.H32.DataDirectory;
        dirCount = header.H_NT.Optional.H32.NumberOfRvaAndSizes;
    }
    else {
        directories = header.H_NT.Optional.H64.DataDirectory;
        dirCount = header.H_NT.Optional.H64.NumberOfRvaAndSizes;
    }
    // 检查数据目录项数量合法性
    if (dirCount > IMAGE_NUMBEROF_DIRECTORY_ENTRIES) return PE_ERROR::TOO_MANY_DATA_DIRECTORIES;
    // 验证关键目录项有效性
    for (DWORD i = 0; i < dirCount; ++i) {
        const DWORD va = directories[i].VirtualAddress;
        const DWORD size = directories[i].Size;
        if (va == 0 && size == 0) continue; // 允许空目录项
        // 检查目录项是否指向有效位置
        if (va > (magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC ?
            header.H_NT.Optional.H32.SizeOfImage :
            header.H_NT.Optional.H64.SizeOfImage)) return PE_ERROR::DIRECTORY_ENTRY_INVALID;
    }
    return PE_ERROR::SUCCESS;
}
auto __stdcall PE::GetHeader() -> PE_HEADER {
    PE_HEADER header = { 0 };

    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        return header;
    }

    // 1. 读取 DOS 头
    this->file.Seek(0);
    Byteset bytes = this->file.Read(sizeof(IMAGE_DOS_HEADER));
    if (bytes.size() != sizeof(IMAGE_DOS_HEADER) ||
        ((IMAGE_DOS_HEADER*)bytes.data())->e_magic != IMAGE_DOS_SIGNATURE) {
        return header;
    }
    memcpy(&header.H_DOS, bytes.data(), sizeof(IMAGE_DOS_HEADER));

    // 2. 定位到 NT 头 (e_lfanew)
    if (!this->file.Seek(header.H_DOS.e_lfanew)) {
        return header;
    }

    // 3. 读取 NT 头签名
    bytes = this->file.Read(sizeof(DWORD));
    if (bytes.size() != sizeof(DWORD) ||
        *(DWORD*)bytes.data() != IMAGE_NT_SIGNATURE) {
        return header;
    }
    header.H_NT.Signature = *(DWORD*)bytes.data();

    // 4. 读取 PE 文件头 (IMAGE_FILE_HEADER)
    bytes = this->file.Read(sizeof(IMAGE_FILE_HEADER));
    if (bytes.size() != sizeof(IMAGE_FILE_HEADER)) {
        return header;
    }
    memcpy(&header.H_NT.FileHeader, bytes.data(), sizeof(IMAGE_FILE_HEADER));

    // 5. 读取可选头 (根据 SizeOfOptionalHeader)
    DWORD optionalHeaderSize = header.H_NT.FileHeader.SizeOfOptionalHeader;
    if (optionalHeaderSize == 0) {
        return header; // 无可选头（非标准PE文件）
    }

    bytes = this->file.Read(optionalHeaderSize);
    if (bytes.size() != optionalHeaderSize) {
        return header;
    }

    // 解析 Magic 字段
    WORD magic = *(WORD*)bytes.data();
    if (magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        if (optionalHeaderSize != sizeof(IMAGE_OPTIONAL_HEADER32)) {
            return header;
        }
        header.H_NT.Optional.H32 = *(IMAGE_OPTIONAL_HEADER32*)bytes.data();
    }
    else if (magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        if (optionalHeaderSize != sizeof(IMAGE_OPTIONAL_HEADER64)) {
            return header;
        }
        header.H_NT.Optional.H64 = *(IMAGE_OPTIONAL_HEADER64*)bytes.data();
    }
    else {
        return header; // Magic 无效
    }

    return header;
}
auto __stdcall PE::GetSectionHeaders(PE_ERROR* error) -> std::vector<PE_SECTION_HEADER> {
    std::vector<PE_SECTION_HEADER> sections;
    // 初始化错误码
    if (error) *error = PE_ERROR::SUCCESS;
    // 1. 检查文件状态
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return sections;
    }
    // 2. 获取PE头信息
    PE_HEADER header = this->GetHeader();
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) {
        if (error) *error = PE_ERROR::NT_HEADER_INVALID;
        return sections;
    }
    // 3. 验证节区数量
    const WORD numberOfSections = header.H_NT.FileHeader.NumberOfSections;
    if (numberOfSections == 0) {
        if (error) *error = PE_ERROR::NO_SECTIONS;
        return sections;
    }
    // 4. 计算节区头起始位置（关键修正点）
    const LONGLONG sectionHeaderStart =
        header.H_DOS.e_lfanew +                 // NT头起始
        sizeof(DWORD) +                         // Signature
        sizeof(IMAGE_FILE_HEADER) +             // FileHeader
        header.H_NT.FileHeader.SizeOfOptionalHeader; // OptionalHeader
    if (!this->file.Seek(sectionHeaderStart)) {
        if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
        return sections;
    }
    // 5. 循环读取节区头
    for (WORD i = 0; i < numberOfSections; ++i) {
        // 读取节区头数据
        Byteset bytes = this->file.Read(sizeof(PE_SECTION_HEADER));
        if (bytes.size() != sizeof(PE_SECTION_HEADER)) {
            sections.clear();
            if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
            return sections;
        }
        // 转换为结构体
        PE_SECTION_HEADER section = *(PE_SECTION_HEADER*)bytes.data();
        // 验证节区名有效性（可选）
        if (section.Name[0] == '\0') {
            if (error) *error = PE_ERROR::INVALID_SECTION_NAME;
            return sections;
        }
        sections.push_back(section);
    }
    return sections;
}

auto __stdcall PE::GetImportTable(PE_ERROR* error) -> std::vector<PE_IMPORT_DESCRIPTOR> {
    std::vector<PE_IMPORT_DESCRIPTOR> imports;
    if (error) *error = PE_ERROR::SUCCESS;

    // 1. 检查文件是否打开
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return imports;
    }

    // 2. 获取 PE 头并验证有效性
    PE_HEADER header = this->GetHeader();
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) {
        if (error) *error = PE_ERROR::NT_HEADER_INVALID;
        return imports;
    }

    // 3. 获取导入表数据目录
    IMAGE_DATA_DIRECTORY importDir;
    if (header.H_NT.Optional.H32.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        importDir = header.H_NT.Optional.H32.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    }
    else {
        importDir = header.H_NT.Optional.H64.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    }

    // 4. 验证导入表存在
    if (importDir.VirtualAddress == 0 || importDir.Size == 0) {
        if (error) *error = PE_ERROR::DATA_DIRECTORY_INVALID;
        return imports;
    }

    // 5. RVA 转文件偏移
    DWORD importTableOffset = RvaToFileOffset(importDir.VirtualAddress);
    if (importTableOffset == 0) {
        if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
        return imports;
    }

    // 6. 遍历导入描述符
    DWORD currentOffset = importTableOffset;
    while (true) {
        // 读取单个导入描述符
        Byteset descData = this->file.ReadAt(currentOffset, sizeof(IMAGE_IMPORT_DESCRIPTOR));
        if (descData.size() != sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
            if (error) *error = PE_ERROR::IMPORT_DESCRIPTOR_INVALID;
            return imports;
        }

        IMAGE_IMPORT_DESCRIPTOR rawDesc;
        memcpy(&rawDesc, descData.data(), sizeof(IMAGE_IMPORT_DESCRIPTOR));

        // 终止条件：全零描述符
        if (rawDesc.OriginalFirstThunk == 0 && rawDesc.TimeDateStamp == 0 &&
            rawDesc.ForwarderChain == 0 && rawDesc.Name == 0 && rawDesc.FirstThunk == 0) {
            break;
        }

        PE_IMPORT_DESCRIPTOR desc{};
        desc.OriginalFirstThunk = rawDesc.OriginalFirstThunk;
        desc.TimeDateStamp = rawDesc.TimeDateStamp;
        desc.ForwarderChain = rawDesc.ForwarderChain;
        desc.FirstThunk = rawDesc.FirstThunk;

        // 7. 解析 DLL 名称
        DWORD nameOffset = RvaToFileOffset(rawDesc.Name);
        if (nameOffset == 0) {
            if (error) *error = PE_ERROR::INVALID_IMPORT_NAME;
            return imports;
        }
        Byteset nameData = this->file.ReadAt(nameOffset, 256); // 读取最多256字节
        desc.DLLName = (LPCSTR)nameData.data();

        // 8. 解析导入函数列表
        DWORD thunkRva = (rawDesc.OriginalFirstThunk != 0) ? rawDesc.OriginalFirstThunk : rawDesc.FirstThunk;
        DWORD thunkOffset = RvaToFileOffset(thunkRva);
        if (thunkOffset == 0) {
            if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
            return imports;
        }

        bool is64bit = (header.H_NT.Optional.H32.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC);
        size_t thunkSize = is64bit ? sizeof(ULONGLONG) : sizeof(DWORD);

        DWORD funcOffset = thunkOffset;
        while (true) {
            Byteset thunkData = this->file.ReadAt(funcOffset, thunkSize);
            if (thunkData.size() != thunkSize) break;

            ULONGLONG thunkValue = is64bit ?
                *(ULONGLONG*)(thunkData.data()) :
                *(DWORD*)(thunkData.data());

            if (thunkValue == 0) break; // 结束标记

            PE_IMPORT_FUNCTION func;
            if (thunkValue & (is64bit ? IMAGE_ORDINAL_FLAG64 : IMAGE_ORDINAL_FLAG32)) {
                // 按序号导入
                func.Ordinal = static_cast<WORD>(thunkValue & 0xFFFF);
            }
            else {
                // 按名称导入
                DWORD hintNameOffset = RvaToFileOffset(static_cast<DWORD>(thunkValue));
                Byteset hintData = this->file.ReadAt(hintNameOffset, sizeof(WORD));
                if (hintData.size() != sizeof(WORD)) {
                    if (error) *error = PE_ERROR::IMPORT_FUNCTION_INVALID;
                    return imports;
                }
                func.Hint = *(WORD*)(hintData.data());

                Byteset nameData = this->file.ReadAt(hintNameOffset + sizeof(WORD), 256);
                func.Name = std::string(reinterpret_cast<const char*>(nameData.data()));
            }

            desc.Functions.push_back(func);
            funcOffset += thunkSize;
        }

        imports.push_back(desc);
        currentOffset += sizeof(IMAGE_IMPORT_DESCRIPTOR);
    }

    return imports;
}
auto __stdcall PE::GetExportTable(PE_ERROR* error) -> std::vector<PE_EXPORT_DIRECTORY> {
    std::vector<PE_EXPORT_DIRECTORY> exports;
    if (error) *error = PE_ERROR::SUCCESS;

    // 1. 检查文件是否打开
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return exports;
    }

    // 2. 获取 PE 头并验证有效性
    PE_HEADER header = this->GetHeader();
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) {
        if (error) *error = PE_ERROR::NT_HEADER_INVALID;
        return exports;
    }

    // 3. 获取导出表数据目录
    IMAGE_DATA_DIRECTORY exportDir;
    if (header.H_NT.Optional.H32.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        exportDir = header.H_NT.Optional.H32.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    }
    else {
        exportDir = header.H_NT.Optional.H64.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    }

    // 4. 验证导出表存在
    if (exportDir.VirtualAddress == 0 || exportDir.Size == 0) {
        if (error) *error = PE_ERROR::DATA_DIRECTORY_INVALID;
        return exports;
    }

    // 5. RVA 转文件偏移
    DWORD exportTableOffset = RvaToFileOffset(exportDir.VirtualAddress);
    if (exportTableOffset == 0) {
        if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
        return exports;
    }

    // 6. 读取导出目录结构
    Byteset dirData = this->file.ReadAt(exportTableOffset, sizeof(IMAGE_EXPORT_DIRECTORY));
    if (dirData.size() != sizeof(IMAGE_EXPORT_DIRECTORY)) {
        if (error) *error = PE_ERROR::EXPORT_DIRECTORY_INVALID;
        return exports;
    }

    IMAGE_EXPORT_DIRECTORY rawDir;
    memcpy(&rawDir, dirData.data(), sizeof(IMAGE_EXPORT_DIRECTORY));

    PE_EXPORT_DIRECTORY dir;
    dir.Characteristics = rawDir.Characteristics;
    dir.TimeDateStamp = rawDir.TimeDateStamp;
    dir.MajorVersion = rawDir.MajorVersion;
    dir.MinorVersion = rawDir.MinorVersion;
    dir.Base = rawDir.Base;
    dir.NumberOfFunctions = rawDir.NumberOfFunctions;
    dir.NumberOfNames = rawDir.NumberOfNames;
    dir.AddressOfFunctions = rawDir.AddressOfFunctions;
    dir.AddressOfNames = rawDir.AddressOfNames;
    dir.AddressOfNameOrdinals = rawDir.AddressOfNameOrdinals;

    // 7. 解析模块名称
    DWORD nameOffset = RvaToFileOffset(rawDir.Name);
    if (nameOffset == 0) {
        if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
        return exports;
    }
    Byteset nameData = this->file.ReadAt(nameOffset, 256);
    dir.ModuleName = Text::text_unicode_to_ascii(Text::text_utf8_to_unicode((LPCSTR)nameData.data()));
    // 8. 读取函数地址表
    DWORD funcTableOffset = RvaToFileOffset(rawDir.AddressOfFunctions);
    Byteset funcData = this->file.ReadAt(funcTableOffset, rawDir.NumberOfFunctions * sizeof(DWORD));
    if (funcData.size() != rawDir.NumberOfFunctions * sizeof(DWORD)) {
        if (error) *error = PE_ERROR::EXPORT_FUNCTION_TABLE_INVALID;
        return exports;
    }
    const DWORD* funcTable = reinterpret_cast<const DWORD*>(funcData.data());
    // 9. 读取名称指针表
    DWORD namePtrTableOffset = RvaToFileOffset(rawDir.AddressOfNames);
    Byteset namePtrData = this->file.ReadAt(namePtrTableOffset, rawDir.NumberOfNames * sizeof(DWORD));
    if (namePtrData.size() != rawDir.NumberOfNames * sizeof(DWORD)) {
        if (error) *error = PE_ERROR::EXPORT_NAME_TABLE_INVALID;
        return exports;
    }
    const DWORD* namePtrTable = reinterpret_cast<const DWORD*>(namePtrData.data());
    // 10. 读取序号表
    DWORD ordinalTableOffset = RvaToFileOffset(rawDir.AddressOfNameOrdinals);
    Byteset ordinalData = this->file.ReadAt(ordinalTableOffset, rawDir.NumberOfNames * sizeof(WORD));
    if (ordinalData.size() != rawDir.NumberOfNames * sizeof(WORD)) {
        if (error) *error = PE_ERROR::EXPORT_ORDINAL_TABLE_INVALID;
        return exports;
    }
    const WORD* ordinalTable = reinterpret_cast<const WORD*>(ordinalData.data());
    // 11. 构建导出函数列表
    for (DWORD i = 0; i < rawDir.NumberOfNames; i++) {
        PE_EXPORT_FUNCTION func;
        // 获取函数名称
        DWORD nameRva = namePtrTable[i];
        DWORD nameOffset = RvaToFileOffset(nameRva);
        if (nameOffset == 0) {
            if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
            return exports;
        }
        Byteset funcNameData = this->file.ReadAt(nameOffset, 256);
        func.Name = std::string(reinterpret_cast<const char*>(funcNameData.data()));
        // 获取序号
        WORD ordinal = ordinalTable[i];
        func.Ordinal = dir.Base + ordinal;
        // 获取函数地址
        if (ordinal >= rawDir.NumberOfFunctions) {
            if (error) *error = PE_ERROR::EXPORT_ORDINAL_OUT_OF_RANGE;
            return exports;
        }
        func.AddressRVA = funcTable[ordinal];
        dir.Functions.push_back(func);
    }
    exports.push_back(dir);
    return exports;
}

auto __stdcall PE::GetResourceTable(PE_ERROR* error) -> std::vector<PE_RESOURCE_DIRECTORY>
{
    return std::vector<PE_RESOURCE_DIRECTORY>();
}

auto __stdcall PE::GetRelocationTable(PE_ERROR* error) -> std::vector<PE_BASE_RELOCATION>
{
    return std::vector<PE_BASE_RELOCATION>();
}

auto __stdcall PE::GetDebugInfo(PE_ERROR* error) -> PE_DEBUG_DIRECTORY
{
    return PE_DEBUG_DIRECTORY();
}

auto __stdcall PE::GetTlsTable(PE_ERROR* error) -> PE_TLS_DIRECTORY32
{
    return PE_TLS_DIRECTORY32();
}

auto __stdcall PE::GetTlsTable64(PE_ERROR* error) -> PE_TLS_DIRECTORY64
{
    return PE_TLS_DIRECTORY64();
}

DWORD PE::RvaToFileOffset(DWORD rva) {
    for (const auto& section : this->GetSectionHeaders()) {
        if (rva >= section.VirtualAddress &&
            rva < section.VirtualAddress + section.Misc.VirtualSize) {
            return section.PointerToRawData + (rva - section.VirtualAddress);
        }
    }
    return 0; // 无效 RVA
}