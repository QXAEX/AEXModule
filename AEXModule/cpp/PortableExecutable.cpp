#include "../h/PortableExecutable.h"
#include <algorithm>
#include <winnt.h>
#include <cstring> // 添加此行

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
    case PE_ERROR::NO_RESOURCE_TABLE: return "资源表不存在";
    case PE_ERROR::INVALID_RVA: return "RVA转换失败";
    case PE_ERROR::READ_FILE: return "文件读取错误";
    case PE_ERROR::DUPLICATE_IMPORT: return "重复的导入";
    case PE_ERROR::INVALID_ALIGNMENT: return "对齐方式无效";
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

auto __stdcall PE::is64bit(const PE_HEADER& header) -> bool
{
    return (header.H_NT.Optional.H32.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC);
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
    this->file.Seek(0);
    Byteset bytes = this->file.Read(sizeof(IMAGE_DOS_HEADER));
    if (bytes.size() != sizeof(IMAGE_DOS_HEADER)) {
        return header;
    }
    memcpy(&header.H_DOS, bytes.data(), sizeof(IMAGE_DOS_HEADER));
    if (!this->file.Seek(header.H_DOS.e_lfanew)) {
        return header;
    }
    bytes = this->file.Read(sizeof(DWORD));
    if (bytes.size() != sizeof(DWORD) || *(DWORD*)bytes.data() != IMAGE_NT_SIGNATURE) {
        return header;
    }
    header.H_NT.Signature = *(DWORD*)bytes.data();
    bytes = this->file.Read(sizeof(IMAGE_FILE_HEADER));
    if (bytes.size() != sizeof(IMAGE_FILE_HEADER)) {
        return header;
    }
    memcpy(&header.H_NT.FileHeader, bytes.data(), sizeof(IMAGE_FILE_HEADER));
    DWORD optionalHeaderSize = header.H_NT.FileHeader.SizeOfOptionalHeader;
    if (optionalHeaderSize == 0) {
        return header;
    }
    bytes = this->file.Read(optionalHeaderSize);
    if (bytes.size() != optionalHeaderSize) {
        return header;
    }
    WORD magic = *(WORD*)bytes.data();
    if (magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        if (optionalHeaderSize != sizeof(IMAGE_OPTIONAL_HEADER32)) {
            return header;
        }
        memcpy(&header.H_NT.Optional.H32, bytes.data(), sizeof(IMAGE_OPTIONAL_HEADER32));
    }
    else if (magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        if (optionalHeaderSize != sizeof(IMAGE_OPTIONAL_HEADER64)) {
            return header;
        }
        memcpy(&header.H_NT.Optional.H64, bytes.data(), sizeof(IMAGE_OPTIONAL_HEADER64));
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
auto __stdcall PE::AddSectionHeader(LPCSTR sectionName, DWORD virtualSize, DWORD rawSize, DWORD characteristics, PE_ERROR* error) -> BOOL
{
    if (error) *error = PE_ERROR::SUCCESS;
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return FALSE;
    }
    if (strlen(sectionName) > IMAGE_SIZEOF_SHORT_NAME) {
        if (error) *error = PE_ERROR::INVALID_SECTION_NAME;
        return FALSE;
    }
    if (virtualSize > 0x4000000 || rawSize > 0x4000000) { // 64MB限制
        if (error) *error = PE_ERROR::SECTION_SIZE_INVALID;
        return FALSE;
    }

    PE_HEADER header = GetHeader();
    PE_ERROR tmpErr;
    auto sections = GetSectionHeaders(&tmpErr);
    if (tmpErr != PE_ERROR::SUCCESS) {
        if (error) *error = tmpErr;
        return FALSE;
    }

    DWORD sectionAlignment = header.H_NT.Optional.H32.SectionAlignment;
    DWORD fileAlignment = header.H_NT.Optional.H32.FileAlignment;

    DWORD newVirtualAddress = 0;
    DWORD newPointerToRaw = 0;
    if (!sections.empty()) {
        const auto& last = sections.back();
        newVirtualAddress = (last.VirtualAddress + last.Misc.VirtualSize + sectionAlignment - 1)
            & ~(sectionAlignment - 1);
        newPointerToRaw = (last.PointerToRawData + last.SizeOfRawData + fileAlignment - 1)
            & ~(fileAlignment - 1);
    }
    else {
        newVirtualAddress = sectionAlignment;
        newPointerToRaw = fileAlignment;
    }
    IMAGE_SECTION_HEADER newSection = { 0 };
    memcpy(newSection.Name, sectionName, IMAGE_SIZEOF_SHORT_NAME);
    newSection.Misc.VirtualSize = virtualSize;
    newSection.VirtualAddress = newVirtualAddress;
    newSection.SizeOfRawData = rawSize;
    newSection.PointerToRawData = newPointerToRaw;
    newSection.Characteristics = characteristics;

    DWORD sectionTableOffset = header.H_DOS.e_lfanew 
        + sizeof(DWORD)
        + sizeof(IMAGE_FILE_HEADER)  
        + header.H_NT.FileHeader.SizeOfOptionalHeader; 
    DWORD newSectionOffset = sectionTableOffset + sections.size() * sizeof(IMAGE_SECTION_HEADER);
    if (!file.Seek(newSectionOffset) ||
        !file.Write(Byteset(&newSection, sizeof(IMAGE_SECTION_HEADER))))
    {
        if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
        return FALSE;
    }
    WORD newNumberOfSections = header.H_NT.FileHeader.NumberOfSections + 1;
    DWORD numberOfSectionsOffset = header.H_DOS.e_lfanew
        + offsetof(IMAGE_NT_HEADERS32, FileHeader.NumberOfSections);

    if (!file.Seek(numberOfSectionsOffset) ||
        !file.Write(Byteset(&newNumberOfSections, sizeof(WORD))))
    {
        if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
        return FALSE;
    }

    DWORD newSizeOfImage = newVirtualAddress + virtualSize;
    newSizeOfImage = (newSizeOfImage + sectionAlignment - 1) & ~(sectionAlignment - 1);
    DWORD sizeOfImageOffset = header.H_DOS.e_lfanew
        + offsetof(IMAGE_NT_HEADERS32, OptionalHeader.SizeOfImage);
    if (header.H_NT.Optional.H32.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
    {
        if (!file.Seek(sizeOfImageOffset) ||
            !file.Write(Byteset(&newSizeOfImage, sizeof(DWORD))))
        {
            if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
            return FALSE;
        }
    }
    else if (header.H_NT.Optional.H64.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC)
    {
        ULONGLONG sizeOfImage64 = newSizeOfImage;
        if (!file.Seek(sizeOfImageOffset) ||
            !file.Write(Byteset(&sizeOfImage64, sizeof(ULONGLONG))))
        {
            if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
            return FALSE;
        }
    }

    LARGE_INTEGER oldFileSize;
    GetFileSizeEx(file.hfile, &oldFileSize);
    DWORD newFileSize = newPointerToRaw + rawSize;

    if (newFileSize > oldFileSize.QuadPart)
    {
        if (!file.Seek(newFileSize) || !SetEndOfFile(file.hfile))
        {
            if (error) *error = PE_ERROR::SECTION_SIZE_INVALID;
            return FALSE;
        }
    }

    return TRUE;
}
auto __stdcall PE::DeleteSectionHeader(LPCSTR sectionName, PE_ERROR* error) -> BOOL
{
    if (error) *error = PE_ERROR::SUCCESS;
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return FALSE;
    }
    PE_ERROR tmpErr;
    auto sections = GetSectionHeaders(&tmpErr);
    if (tmpErr != PE_ERROR::SUCCESS) {
        if (error) *error = tmpErr;
        return FALSE;
    }

    // 查找目标节区
    auto it = std::find_if(sections.begin(), sections.end(),
        [&](const PE_SECTION_HEADER& s) {
            return strncmp((const char*)s.Name, sectionName, IMAGE_SIZEOF_SHORT_NAME) == 0;
        });

    if (it == sections.end()) {
        if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
        return FALSE;
    }

    // 计算节区头表位置
    PE_HEADER header = GetHeader();
    const DWORD sectionTableOffset = header.H_DOS.e_lfanew +
        sizeof(IMAGE_NT_HEADERS) +
        header.H_NT.FileHeader.SizeOfOptionalHeader;

    // 移动后续节区头
    for (size_t i = it - sections.begin() + 1; i < sections.size(); ++i)
    {
        // 读取当前节区头
        if (!file.Seek(sectionTableOffset + i * sizeof(IMAGE_SECTION_HEADER))) {
            if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
            return FALSE;
        }

        Byteset data = file.Read(sizeof(IMAGE_SECTION_HEADER));
        if (data.size() != sizeof(IMAGE_SECTION_HEADER)) {
            if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
            return FALSE;
        }

        // 写入前一个位置
        if (!file.Seek(sectionTableOffset + (i - 1) * sizeof(IMAGE_SECTION_HEADER)) ||
            !file.Write(data))
        {
            if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
            return FALSE;
        }
    }

    // 清除最后一个节区头
    Byteset zero(sizeof(IMAGE_SECTION_HEADER), 0);
    if (!file.Seek(sectionTableOffset + (sections.size() - 1) * sizeof(IMAGE_SECTION_HEADER)) ||
        !file.Write(zero))
    {
        if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
        return FALSE;
    }

    // 更新节区数量
    header.H_NT.FileHeader.NumberOfSections--;
    if (!file.Seek(header.H_DOS.e_lfanew + offsetof(IMAGE_NT_HEADERS, FileHeader.NumberOfSections)) ||
        !file.Write(Byteset(&header.H_NT.FileHeader.NumberOfSections, sizeof(WORD))))
    {
        if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
        return FALSE;
    }

    return TRUE;
}
auto __stdcall PE::GetSectionByName(LPCSTR name, PE_SECTION_HEADER* section, PE_ERROR* error) -> BOOL
{
    std::vector<PE_SECTION_HEADER> sections = GetSectionHeaders();
    for (const auto& sec : sections) {
        if (strncmp((LPCSTR)sec.Name, name, IMAGE_SIZEOF_SHORT_NAME) == 0) {
            *section = sec;
            return TRUE;
        }
    }
    if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
    return FALSE;
}
auto __stdcall PE::FindMaxSectionIndex(const std::string& baseName) -> int
{
    int maxIndex = -1;
    std::vector<PE_SECTION_HEADER> sections = GetSectionHeaders();
    for (const auto& sec : sections) {
        std::string name(reinterpret_cast<const char*>(sec.Name), IMAGE_SIZEOF_SHORT_NAME);
        if (name.find(baseName) == 0) {
            size_t suffixStart = baseName.length();
            std::string suffix = name.substr(suffixStart);
            if (!suffix.empty()) {
                try {
                    int index = std::stoi(suffix);
                    maxIndex = (((maxIndex) > (index)) ? (maxIndex) : (index));
                }
                catch (...) {
                    // 忽略非数字后缀
                }
            }
            else {
                maxIndex = (((maxIndex) > (0)) ? (maxIndex) : (0)); // 处理 ".aexdt" 这种情况
            }
        }
    }
    return maxIndex;
}
auto __stdcall PE::ModifySectionHeader(LPCSTR oldName, LPCSTR newName, DWORD newVirtualSize, DWORD newRawSize, DWORD newCharacteristics, PE_ERROR* error) -> BOOL
{
    if (error) *error = PE_ERROR::SUCCESS;
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return FALSE;
    }
    PE_ERROR tmpErr;
    auto sections = GetSectionHeaders(&tmpErr);
    if (tmpErr != PE_ERROR::SUCCESS) {
        if (error) *error = tmpErr;
        return FALSE;
    }
    auto it = std::find_if(sections.begin(), sections.end(),
        [&](const PE_SECTION_HEADER& peSec) {
            IMAGE_SECTION_HEADER imgSec = PE_SECTION_HEADER_to_IMAGE(peSec);
            return strncmp((const char*)imgSec.Name, oldName, IMAGE_SIZEOF_SHORT_NAME) == 0;
        });
    if (it == sections.end()) {
        if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
        return FALSE;
    }
    PE_SECTION_HEADER modified = *it;
    IMAGE_SECTION_HEADER imgModified = PE_SECTION_HEADER_to_IMAGE(modified);
    if (newName != nullptr) {
        memset(imgModified.Name, 0, IMAGE_SIZEOF_SHORT_NAME);
        strncpy_s(
            (char*)imgModified.Name,
            IMAGE_SIZEOF_SHORT_NAME,
            newName,
            _TRUNCATE
        );
    }
    if (newVirtualSize > 0) imgModified.Misc.VirtualSize = newVirtualSize;
    if (newRawSize > 0) imgModified.SizeOfRawData = newRawSize;
    if (newCharacteristics != 0) imgModified.Characteristics = newCharacteristics;
    modified = IMAGE_to_PE_SECTION_HEADER(imgModified);
    PE_HEADER header = GetHeader();
    const DWORD sectionTableOffset = header.H_DOS.e_lfanew
        + sizeof(DWORD)
        + sizeof(IMAGE_FILE_HEADER)
        + header.H_NT.FileHeader.SizeOfOptionalHeader;
    const DWORD targetOffset = sectionTableOffset +
        (it - sections.begin()) * sizeof(IMAGE_SECTION_HEADER);
    if (!file.Seek(targetOffset) ||
        !file.Write(Byteset(&imgModified, sizeof(IMAGE_SECTION_HEADER))))
    {
        if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
        return FALSE;
    }
    return TRUE;
}
auto __stdcall PE::GetImportTable(PE_ERROR* error) -> std::vector<PE_IMPORT_DESCRIPTOR> {
    std::vector<PE_IMPORT_DESCRIPTOR> imports;
    if (error) *error = PE_ERROR::SUCCESS;
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return imports;
    }
    PE_HEADER header = this->GetHeader();
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) {
        if (error) *error = PE_ERROR::NT_HEADER_INVALID;
        return imports;
    }
    IMAGE_DATA_DIRECTORY importDir;
    if (header.H_NT.Optional.H32.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        importDir = header.H_NT.Optional.H32.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    }
    else {
        importDir = header.H_NT.Optional.H64.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    }
    if (importDir.VirtualAddress == 0 || importDir.Size == 0) {
        if (error) *error = PE_ERROR::DATA_DIRECTORY_INVALID;
        return imports;
    }
    DWORD importTableOffset = RvaToFileOffset(importDir.VirtualAddress);
    if (importTableOffset == 0) {
        if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
        return imports;
    }
    DWORD currentOffset = importTableOffset;
    while (true) {
        Byteset descData = this->file.ReadAt(currentOffset, sizeof(IMAGE_IMPORT_DESCRIPTOR));
        if (descData.size() != sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
            if (error) *error = PE_ERROR::IMPORT_DESCRIPTOR_INVALID;
            return imports;
        }
        IMAGE_IMPORT_DESCRIPTOR rawDesc;
        memcpy(&rawDesc, descData.data(), sizeof(IMAGE_IMPORT_DESCRIPTOR));
        if (rawDesc.OriginalFirstThunk == 0 && rawDesc.TimeDateStamp == 0 &&
            rawDesc.ForwarderChain == 0 && rawDesc.Name == 0 && rawDesc.FirstThunk == 0) {
            break;
        }
        PE_IMPORT_DESCRIPTOR desc{};
        desc.OriginalFirstThunk = rawDesc.OriginalFirstThunk;
        desc.TimeDateStamp = rawDesc.TimeDateStamp;
        desc.ForwarderChain = rawDesc.ForwarderChain;
        desc.FirstThunk = rawDesc.FirstThunk;
        DWORD nameOffset = RvaToFileOffset(rawDesc.Name);
        if (nameOffset == 0) {
            if (error) *error = PE_ERROR::INVALID_IMPORT_NAME;
            return imports;
        }
        Byteset nameData = this->file.ReadAt(nameOffset, 256);
        desc.DLLName = (LPCSTR)nameData.data();
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
            if (thunkValue == 0) break;
            PE_IMPORT_FUNCTION func;
            if (thunkValue & (is64bit ? IMAGE_ORDINAL_FLAG64 : IMAGE_ORDINAL_FLAG32)) {
                func.Ordinal = static_cast<WORD>(thunkValue & 0xFFFF);
            }
            else {
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
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return exports;
    }
    PE_HEADER header = this->GetHeader();
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) {
        if (error) *error = PE_ERROR::NT_HEADER_INVALID;
        return exports;
    }
    IMAGE_DATA_DIRECTORY exportDir;
    if (header.H_NT.Optional.H32.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        exportDir = header.H_NT.Optional.H32.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    }
    else {
        exportDir = header.H_NT.Optional.H64.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    }
    if (exportDir.VirtualAddress == 0 || exportDir.Size == 0) {
        if (error) *error = PE_ERROR::DATA_DIRECTORY_INVALID;
        return exports;
    }
    DWORD exportTableOffset = RvaToFileOffset(exportDir.VirtualAddress);
    if (exportTableOffset == 0) {
        if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
        return exports;
    }
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
    DWORD nameOffset = RvaToFileOffset(rawDir.Name);
    if (nameOffset == 0) {
        if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
        return exports;
    }
    Byteset nameData = this->file.ReadAt(nameOffset, 256);
    dir.ModuleName = Text::text_unicode_to_ascii(Text::text_utf8_to_unicode((LPCSTR)nameData.data()));
    DWORD funcTableOffset = RvaToFileOffset(rawDir.AddressOfFunctions);
    Byteset funcData = this->file.ReadAt(funcTableOffset, rawDir.NumberOfFunctions * sizeof(DWORD));
    if (funcData.size() != rawDir.NumberOfFunctions * sizeof(DWORD)) {
        if (error) *error = PE_ERROR::EXPORT_FUNCTION_TABLE_INVALID;
        return exports;
    }
    const DWORD* funcTable = reinterpret_cast<const DWORD*>(funcData.data());
    DWORD namePtrTableOffset = RvaToFileOffset(rawDir.AddressOfNames);
    Byteset namePtrData = this->file.ReadAt(namePtrTableOffset, rawDir.NumberOfNames * sizeof(DWORD));
    if (namePtrData.size() != rawDir.NumberOfNames * sizeof(DWORD)) {
        if (error) *error = PE_ERROR::EXPORT_NAME_TABLE_INVALID;
        return exports;
    }
    const DWORD* namePtrTable = reinterpret_cast<const DWORD*>(namePtrData.data());
    DWORD ordinalTableOffset = RvaToFileOffset(rawDir.AddressOfNameOrdinals);
    Byteset ordinalData = this->file.ReadAt(ordinalTableOffset, rawDir.NumberOfNames * sizeof(WORD));
    if (ordinalData.size() != rawDir.NumberOfNames * sizeof(WORD)) {
        if (error) *error = PE_ERROR::EXPORT_ORDINAL_TABLE_INVALID;
        return exports;
    }
    const WORD* ordinalTable = reinterpret_cast<const WORD*>(ordinalData.data());
    for (DWORD i = 0; i < rawDir.NumberOfNames; i++) {
        PE_EXPORT_FUNCTION func;
        DWORD nameRva = namePtrTable[i];
        DWORD nameOffset = RvaToFileOffset(nameRva);
        if (nameOffset == 0) {
            if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
            return exports;
        }
        Byteset funcNameData = this->file.ReadAt(nameOffset, 256);
        func.Name = std::string(reinterpret_cast<const char*>(funcNameData.data()));
        WORD ordinal = ordinalTable[i];
        func.Ordinal = dir.Base + ordinal;
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

DWORD PE::RvaToFileOffset(DWORD rva) {
    for (const auto& section : this->GetSectionHeaders()) {
        if (rva >= section.VirtualAddress &&
            rva < section.VirtualAddress + section.Misc.VirtualSize) {
            return section.PointerToRawData + (rva - section.VirtualAddress);
        }
    }
    return 0; // 无效 RVA
}

IMAGE_SECTION_HEADER PE::PE_SECTION_HEADER_to_IMAGE(const PE_SECTION_HEADER& peSec)
{
    IMAGE_SECTION_HEADER imgSec = { 0 };
    memcpy(&imgSec, &peSec, sizeof(IMAGE_SECTION_HEADER));
    return imgSec;
}

PE_SECTION_HEADER PE::IMAGE_to_PE_SECTION_HEADER(const IMAGE_SECTION_HEADER& imgSec)
{
    PE_SECTION_HEADER peSec = { 0 };
    memcpy(&peSec, &imgSec, sizeof(PE_SECTION_HEADER));
    return peSec;
}

BOOL PE::AllocateSpace(DWORD neededSize, DWORD* outOffset, PE_ERROR* error)
{
    auto sections = GetSectionHeaders();
    for (const auto& sec : sections) {
        DWORD endOffset = sec.PointerToRawData + sec.SizeOfRawData;
        DWORD fileSize = (DWORD)file.Size();
        if (endOffset < fileSize) {
            DWORD available = fileSize - endOffset;
            if (available >= neededSize) {
                *outOffset = endOffset;
                return TRUE;
            }
        }
    }
    *outOffset = (DWORD)file.Size();
    if (!file.Seek(*outOffset + neededSize) || !SetEndOfFile(file.hfile)) {
        if (error) *error = PE_ERROR::SECTION_SIZE_INVALID;
        return FALSE;
    }
    return TRUE;
}

DWORD PE::AlignValue(DWORD value, DWORD alignment, PE_ERROR* error) {
    if ((alignment == 0) || (alignment & (alignment - 1)) != 0) {
        if (error) *error = PE_ERROR::INVALID_ALIGNMENT;
        return 0;
    }
    return (value + alignment - 1) & ~(alignment - 1);
}

DWORD PE::FileOffsetToRva(DWORD fileOffset) {
    for (const auto& section : GetSectionHeaders()) {
        if (fileOffset >= section.PointerToRawData &&
            fileOffset < section.PointerToRawData + section.SizeOfRawData)
        {
            // 重点：使用节区的VirtualAddress而非直接计算
            return (fileOffset - section.PointerToRawData) + section.VirtualAddress;
        }
    }
    return 0;
}

BOOL PE::UpdateDataDirectory(int index, IMAGE_DATA_DIRECTORY* dir, PE_ERROR* error)
{
    PE_HEADER header = GetHeader();
    DWORD dirOffset = header.H_DOS.e_lfanew
        + sizeof(DWORD)
        + sizeof(IMAGE_FILE_HEADER)
        + offsetof(IMAGE_OPTIONAL_HEADER32, DataDirectory)
        + index * sizeof(IMAGE_DATA_DIRECTORY);

    file.Seek(dirOffset);
    return file.Write(Byteset(dir, sizeof(IMAGE_DATA_DIRECTORY)));
}

DWORD PE::CalculateImportTableSize(LPCSTR dllName) {
    std::vector<PE_IMPORT_DESCRIPTOR> imports = GetImportTable();
    DWORD totalSize = 0;

    for (const auto& descPtr : imports) {
        const PE_IMPORT_DESCRIPTOR& desc = descPtr; // 解引用指针

        // 1. 基础结构体大小
        totalSize += sizeof(PE_IMPORT_DESCRIPTOR);

        // 2. 动态数据计算
        // DLL名称字符串（仅当存在非序号导入时需要）
        if (!desc.DLLName.empty()) {
            totalSize += desc.DLLName.size() + 1; // +1 包含终止符
        }

        // 3. 导入函数列表大小
        for (const auto& func : desc.Functions) {
            // 根据导入类型计算函数描述符大小
            size_t funcSize = sizeof(PE_IMPORT_FUNCTION);
            if (!func.IsOrdinal) {
                funcSize += func.Name.size() + 1; // 函数名称字符串
            }
            totalSize += funcSize;
        }
    }

    return totalSize;
}

DWORD PE::FindImportDescriptorEnd(const Byteset& data)
{
    const IMAGE_IMPORT_DESCRIPTOR* p = reinterpret_cast<const IMAGE_IMPORT_DESCRIPTOR*>(data.data());
    while (p->Characteristics != 0 ||
        p->TimeDateStamp != 0 ||
        p->ForwarderChain != 0 ||
        p->Name != 0 ||
        p->FirstThunk != 0) {
        p++;
    }
    return reinterpret_cast<const BYTE*>(p) - data.data();
}

// 辅助函数：分配导入表空间
BOOL PE::AllocateImportSpace(DWORD size, DWORD* outOffset, PE_ERROR* error) {
    // 优先使用.idata节区
    PE_SECTION_HEADER idataSection;
    if (GetSectionByName(".idata", &idataSection, nullptr)) {
        *outOffset = idataSection.PointerToRawData + idataSection.SizeOfRawData;
        return TRUE;
    }

    // 次优选择：使用指定sectionName或默认.aexdt
    char sectionName[IMAGE_SIZEOF_SHORT_NAME] = ".aexdt";
    int maxIndex = FindMaxSectionIndex(sectionName);
    _snprintf_s(sectionName, IMAGE_SIZEOF_SHORT_NAME, _TRUNCATE, "%s%d", sectionName, maxIndex + 1);

    // 创建新节区
    DWORD sectAlign = is64bit(GetHeader()) ?
        GetHeader().H_NT.Optional.H64.SectionAlignment :
        GetHeader().H_NT.Optional.H32.SectionAlignment;
    DWORD fileAlign = 0x200; // 默认文件对齐

    PE_SECTION_ATTRIBUTES attr;
    attr.SetReadable(true);
    attr.SetContainsInitializedData(true);

    if (!AddSectionHeader(
        sectionName,
        sectAlign,
        fileAlign,
        attr.Flags,
        error
    )) {
        return FALSE;
    }

    // 获取新节区位置
    if (!GetSectionByName(sectionName, &idataSection, error)) {
        return FALSE;
    }
    *outOffset = idataSection.PointerToRawData;
    return TRUE;
}

IMAGE_DATA_DIRECTORY& PE::GetDataDirectory(int index)
{
    return is64bit(GetHeader()) ?
        GetHeader().H_NT.Optional.H64.DataDirectory[index] :
        GetHeader().H_NT.Optional.H32.DataDirectory[index];
}