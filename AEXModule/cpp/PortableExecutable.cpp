#include "../h/PortableExecutable.h"

#include <winnt.h>

PE::PE(LPCSTR filename)
{
    this->Import(filename);
}

std::string PE::ErrorToString(PE_ERROR error)
{
    switch (error) {
    case PE_ERROR::SUCCESS: return "�ɹ�";
    case PE_ERROR::FILE_NOT_OPEN: return "�ļ�δ��";
    case PE_ERROR::DOS_HEADER_INVALID: return "DOSͷ��Ч";
    case PE_ERROR::NT_HEADER_INVALID: return "NTͷ��Ч";
    case PE_ERROR::NOT_SUPPORTED_ARCHITECTURE: return "��֧�ֵļܹ�";
    case PE_ERROR::SECTION_HEADER_INVALID: return "����ͷ��Ч";
    case PE_ERROR::OPTIONAL_HEADER_MAGIC_INVALID: return "��ѡͷħ����Ч";
    case PE_ERROR::TOO_MANY_SECTIONS: return "������������";
    case PE_ERROR::SECTION_OFFSET_INVALID: return "����ƫ��Խ��";
    case PE_ERROR::SECTION_SIZE_INVALID: return "������С��Ч";
    case PE_ERROR::TOO_MANY_DATA_DIRECTORIES: return "����Ŀ¼�����";
    case PE_ERROR::DIRECTORY_ENTRY_INVALID: return "Ŀ¼���ַ��Ч";
    case PE_ERROR::NO_SECTIONS: return "û�н���";
    case PE_ERROR::INVALID_SECTION_NAME: return "��������Ч";
    case PE_ERROR::DATA_DIRECTORY_INVALID: return "����Ŀ¼��Ч";
    case PE_ERROR::RVA_CONVERSION_FAILED: return "RVAת��ʧ��";
    case PE_ERROR::EXPORT_DIRECTORY_INVALID: return "����Ŀ¼��Ч";
    case PE_ERROR::EXPORT_FUNCTION_TABLE_INVALID: return "������������Ч";
    case PE_ERROR::EXPORT_NAME_TABLE_INVALID: return "�������Ʊ���Ч";
    case PE_ERROR::EXPORT_ORDINAL_TABLE_INVALID: return "������ű���Ч";
    case PE_ERROR::EXPORT_ORDINAL_OUT_OF_RANGE: return "�������Խ��";
    case PE_ERROR::IMPORT_FUNCTION_INVALID: return "���뺯����Ч";
    case PE_ERROR::INVALID_IMPORT_NAME: return "���뺯������Ч";
    case PE_ERROR::IMPORT_DESCRIPTOR_INVALID: return "������������Ч";
    default: return "δ֪����";
    }
}

auto __stdcall PE::Import(LPCSTR filename) -> BOOL
{
    //�ж��ļ��Ƿ����
    if (!this->file.Exists(filename)) {
        return FALSE;
    }
    //�����ļ�
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
    // 1. ����ļ��Ƿ��Ѵ�
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) return PE_ERROR::FILE_NOT_OPEN;
    // 2. ��֤ DOS ͷ
    PE_HEADER header = this->GetHeader();
    if (header.H_DOS.e_magic != IMAGE_DOS_SIGNATURE) return PE_ERROR::DOS_HEADER_INVALID;
    // 3. ��֤ NT ͷǩ��
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) return PE_ERROR::NT_HEADER_INVALID;
    // 4. ��֤�ļ�ͷ�ܹ�
    const WORD machine = header.H_NT.FileHeader.Machine;
    if (machine != IMAGE_FILE_MACHINE_I386 &&
        machine != IMAGE_FILE_MACHINE_AMD64 &&
        machine != IMAGE_FILE_MACHINE_IA64) return PE_ERROR::NOT_SUPPORTED_ARCHITECTURE;
    // 5. ��֤��ѡͷħ��
    const WORD magic = header.H_NT.Optional.H32.Magic;
    if (magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC &&
        magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) return PE_ERROR::OPTIONAL_HEADER_MAGIC_INVALID;
    // 6. ��֤��������������
    const WORD numSections = header.H_NT.FileHeader.NumberOfSections;
    // һ��PE�ļ����ᳬ��100������
    if (numSections > 100) return PE_ERROR::TOO_MANY_SECTIONS;
    // 7. ��֤����������Ч��
    LARGE_INTEGER fileSize;
    if (!GetFileSizeEx(this->file.hfile, &fileSize)) return PE_ERROR::SECTION_HEADER_INVALID;
    PE_ERROR er;
    std::vector<PE_SECTION_HEADER> sections = GetSectionHeaders(&er);
    if (er != PE_ERROR::SUCCESS) return er;
    for (const auto& section : sections) {
        // �������ļ�ƫ����Ч��
        if (section.PointerToRawData > fileSize.QuadPart) return PE_ERROR::SECTION_HEADER_INVALID;
        // ��������С��Ч��
        if (section.SizeOfRawData > (DWORD)(fileSize.QuadPart - section.PointerToRawData)) return PE_ERROR::SECTION_SIZE_INVALID;
    }
    // 8. ��֤����Ŀ¼��
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
    // �������Ŀ¼�������Ϸ���
    if (dirCount > IMAGE_NUMBEROF_DIRECTORY_ENTRIES) return PE_ERROR::TOO_MANY_DATA_DIRECTORIES;
    // ��֤�ؼ�Ŀ¼����Ч��
    for (DWORD i = 0; i < dirCount; ++i) {
        const DWORD va = directories[i].VirtualAddress;
        const DWORD size = directories[i].Size;
        if (va == 0 && size == 0) continue; // �����Ŀ¼��
        // ���Ŀ¼���Ƿ�ָ����Чλ��
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

    // 1. ��ȡ DOS ͷ
    this->file.Seek(0);
    Byteset bytes = this->file.Read(sizeof(IMAGE_DOS_HEADER));
    if (bytes.size() != sizeof(IMAGE_DOS_HEADER) ||
        ((IMAGE_DOS_HEADER*)bytes.data())->e_magic != IMAGE_DOS_SIGNATURE) {
        return header;
    }
    memcpy(&header.H_DOS, bytes.data(), sizeof(IMAGE_DOS_HEADER));

    // 2. ��λ�� NT ͷ (e_lfanew)
    if (!this->file.Seek(header.H_DOS.e_lfanew)) {
        return header;
    }

    // 3. ��ȡ NT ͷǩ��
    bytes = this->file.Read(sizeof(DWORD));
    if (bytes.size() != sizeof(DWORD) ||
        *(DWORD*)bytes.data() != IMAGE_NT_SIGNATURE) {
        return header;
    }
    header.H_NT.Signature = *(DWORD*)bytes.data();

    // 4. ��ȡ PE �ļ�ͷ (IMAGE_FILE_HEADER)
    bytes = this->file.Read(sizeof(IMAGE_FILE_HEADER));
    if (bytes.size() != sizeof(IMAGE_FILE_HEADER)) {
        return header;
    }
    memcpy(&header.H_NT.FileHeader, bytes.data(), sizeof(IMAGE_FILE_HEADER));

    // 5. ��ȡ��ѡͷ (���� SizeOfOptionalHeader)
    DWORD optionalHeaderSize = header.H_NT.FileHeader.SizeOfOptionalHeader;
    if (optionalHeaderSize == 0) {
        return header; // �޿�ѡͷ���Ǳ�׼PE�ļ���
    }

    bytes = this->file.Read(optionalHeaderSize);
    if (bytes.size() != optionalHeaderSize) {
        return header;
    }

    // ���� Magic �ֶ�
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
        return header; // Magic ��Ч
    }

    return header;
}
auto __stdcall PE::GetSectionHeaders(PE_ERROR* error) -> std::vector<PE_SECTION_HEADER> {
    std::vector<PE_SECTION_HEADER> sections;
    // ��ʼ��������
    if (error) *error = PE_ERROR::SUCCESS;
    // 1. ����ļ�״̬
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return sections;
    }
    // 2. ��ȡPEͷ��Ϣ
    PE_HEADER header = this->GetHeader();
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) {
        if (error) *error = PE_ERROR::NT_HEADER_INVALID;
        return sections;
    }
    // 3. ��֤��������
    const WORD numberOfSections = header.H_NT.FileHeader.NumberOfSections;
    if (numberOfSections == 0) {
        if (error) *error = PE_ERROR::NO_SECTIONS;
        return sections;
    }
    // 4. �������ͷ��ʼλ�ã��ؼ������㣩
    const LONGLONG sectionHeaderStart =
        header.H_DOS.e_lfanew +                 // NTͷ��ʼ
        sizeof(DWORD) +                         // Signature
        sizeof(IMAGE_FILE_HEADER) +             // FileHeader
        header.H_NT.FileHeader.SizeOfOptionalHeader; // OptionalHeader
    if (!this->file.Seek(sectionHeaderStart)) {
        if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
        return sections;
    }
    // 5. ѭ����ȡ����ͷ
    for (WORD i = 0; i < numberOfSections; ++i) {
        // ��ȡ����ͷ����
        Byteset bytes = this->file.Read(sizeof(PE_SECTION_HEADER));
        if (bytes.size() != sizeof(PE_SECTION_HEADER)) {
            sections.clear();
            if (error) *error = PE_ERROR::SECTION_HEADER_INVALID;
            return sections;
        }
        // ת��Ϊ�ṹ��
        PE_SECTION_HEADER section = *(PE_SECTION_HEADER*)bytes.data();
        // ��֤��������Ч�ԣ���ѡ��
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

    // 1. ����ļ��Ƿ��
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return imports;
    }

    // 2. ��ȡ PE ͷ����֤��Ч��
    PE_HEADER header = this->GetHeader();
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) {
        if (error) *error = PE_ERROR::NT_HEADER_INVALID;
        return imports;
    }

    // 3. ��ȡ���������Ŀ¼
    IMAGE_DATA_DIRECTORY importDir;
    if (header.H_NT.Optional.H32.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        importDir = header.H_NT.Optional.H32.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    }
    else {
        importDir = header.H_NT.Optional.H64.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    }

    // 4. ��֤��������
    if (importDir.VirtualAddress == 0 || importDir.Size == 0) {
        if (error) *error = PE_ERROR::DATA_DIRECTORY_INVALID;
        return imports;
    }

    // 5. RVA ת�ļ�ƫ��
    DWORD importTableOffset = RvaToFileOffset(importDir.VirtualAddress);
    if (importTableOffset == 0) {
        if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
        return imports;
    }

    // 6. ��������������
    DWORD currentOffset = importTableOffset;
    while (true) {
        // ��ȡ��������������
        Byteset descData = this->file.ReadAt(currentOffset, sizeof(IMAGE_IMPORT_DESCRIPTOR));
        if (descData.size() != sizeof(IMAGE_IMPORT_DESCRIPTOR)) {
            if (error) *error = PE_ERROR::IMPORT_DESCRIPTOR_INVALID;
            return imports;
        }

        IMAGE_IMPORT_DESCRIPTOR rawDesc;
        memcpy(&rawDesc, descData.data(), sizeof(IMAGE_IMPORT_DESCRIPTOR));

        // ��ֹ������ȫ��������
        if (rawDesc.OriginalFirstThunk == 0 && rawDesc.TimeDateStamp == 0 &&
            rawDesc.ForwarderChain == 0 && rawDesc.Name == 0 && rawDesc.FirstThunk == 0) {
            break;
        }

        PE_IMPORT_DESCRIPTOR desc{};
        desc.OriginalFirstThunk = rawDesc.OriginalFirstThunk;
        desc.TimeDateStamp = rawDesc.TimeDateStamp;
        desc.ForwarderChain = rawDesc.ForwarderChain;
        desc.FirstThunk = rawDesc.FirstThunk;

        // 7. ���� DLL ����
        DWORD nameOffset = RvaToFileOffset(rawDesc.Name);
        if (nameOffset == 0) {
            if (error) *error = PE_ERROR::INVALID_IMPORT_NAME;
            return imports;
        }
        Byteset nameData = this->file.ReadAt(nameOffset, 256); // ��ȡ���256�ֽ�
        desc.DLLName = (LPCSTR)nameData.data();

        // 8. �������뺯���б�
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

            if (thunkValue == 0) break; // �������

            PE_IMPORT_FUNCTION func;
            if (thunkValue & (is64bit ? IMAGE_ORDINAL_FLAG64 : IMAGE_ORDINAL_FLAG32)) {
                // ����ŵ���
                func.Ordinal = static_cast<WORD>(thunkValue & 0xFFFF);
            }
            else {
                // �����Ƶ���
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

    // 1. ����ļ��Ƿ��
    if (!this->file.is_open || this->file.hfile == INVALID_HANDLE_VALUE) {
        if (error) *error = PE_ERROR::FILE_NOT_OPEN;
        return exports;
    }

    // 2. ��ȡ PE ͷ����֤��Ч��
    PE_HEADER header = this->GetHeader();
    if (header.H_NT.Signature != IMAGE_NT_SIGNATURE) {
        if (error) *error = PE_ERROR::NT_HEADER_INVALID;
        return exports;
    }

    // 3. ��ȡ����������Ŀ¼
    IMAGE_DATA_DIRECTORY exportDir;
    if (header.H_NT.Optional.H32.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC) {
        exportDir = header.H_NT.Optional.H32.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    }
    else {
        exportDir = header.H_NT.Optional.H64.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    }

    // 4. ��֤���������
    if (exportDir.VirtualAddress == 0 || exportDir.Size == 0) {
        if (error) *error = PE_ERROR::DATA_DIRECTORY_INVALID;
        return exports;
    }

    // 5. RVA ת�ļ�ƫ��
    DWORD exportTableOffset = RvaToFileOffset(exportDir.VirtualAddress);
    if (exportTableOffset == 0) {
        if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
        return exports;
    }

    // 6. ��ȡ����Ŀ¼�ṹ
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

    // 7. ����ģ������
    DWORD nameOffset = RvaToFileOffset(rawDir.Name);
    if (nameOffset == 0) {
        if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
        return exports;
    }
    Byteset nameData = this->file.ReadAt(nameOffset, 256);
    dir.ModuleName = Text::text_unicode_to_ascii(Text::text_utf8_to_unicode((LPCSTR)nameData.data()));
    // 8. ��ȡ������ַ��
    DWORD funcTableOffset = RvaToFileOffset(rawDir.AddressOfFunctions);
    Byteset funcData = this->file.ReadAt(funcTableOffset, rawDir.NumberOfFunctions * sizeof(DWORD));
    if (funcData.size() != rawDir.NumberOfFunctions * sizeof(DWORD)) {
        if (error) *error = PE_ERROR::EXPORT_FUNCTION_TABLE_INVALID;
        return exports;
    }
    const DWORD* funcTable = reinterpret_cast<const DWORD*>(funcData.data());
    // 9. ��ȡ����ָ���
    DWORD namePtrTableOffset = RvaToFileOffset(rawDir.AddressOfNames);
    Byteset namePtrData = this->file.ReadAt(namePtrTableOffset, rawDir.NumberOfNames * sizeof(DWORD));
    if (namePtrData.size() != rawDir.NumberOfNames * sizeof(DWORD)) {
        if (error) *error = PE_ERROR::EXPORT_NAME_TABLE_INVALID;
        return exports;
    }
    const DWORD* namePtrTable = reinterpret_cast<const DWORD*>(namePtrData.data());
    // 10. ��ȡ��ű�
    DWORD ordinalTableOffset = RvaToFileOffset(rawDir.AddressOfNameOrdinals);
    Byteset ordinalData = this->file.ReadAt(ordinalTableOffset, rawDir.NumberOfNames * sizeof(WORD));
    if (ordinalData.size() != rawDir.NumberOfNames * sizeof(WORD)) {
        if (error) *error = PE_ERROR::EXPORT_ORDINAL_TABLE_INVALID;
        return exports;
    }
    const WORD* ordinalTable = reinterpret_cast<const WORD*>(ordinalData.data());
    // 11. �������������б�
    for (DWORD i = 0; i < rawDir.NumberOfNames; i++) {
        PE_EXPORT_FUNCTION func;
        // ��ȡ��������
        DWORD nameRva = namePtrTable[i];
        DWORD nameOffset = RvaToFileOffset(nameRva);
        if (nameOffset == 0) {
            if (error) *error = PE_ERROR::RVA_CONVERSION_FAILED;
            return exports;
        }
        Byteset funcNameData = this->file.ReadAt(nameOffset, 256);
        func.Name = std::string(reinterpret_cast<const char*>(funcNameData.data()));
        // ��ȡ���
        WORD ordinal = ordinalTable[i];
        func.Ordinal = dir.Base + ordinal;
        // ��ȡ������ַ
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
    return 0; // ��Ч RVA
}