#include "../h/System.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <Psapi.h>
#include <sys/stat.h>
#include <sys/types.h>
std::string WINAPI System::GetTime(bool format, bool hideSecond)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;
    localtime_s(&now_tm, &now_time_t);
    std::ostringstream oss;
    if (format)
    {
        if (hideSecond) {
            oss << std::put_time(&now_tm, "%Y-%m-%d");
        }
        else {
            oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
        }
    }
    else
    {
        if (hideSecond) {
            oss << std::put_time(&now_tm, "%Y%m%d");
        }
        else {
            oss << std::put_time(&now_tm, "%Y%m%d%H%M%S");
        }
    }
    return oss.str();
}

std::string WINAPI System::GetRunPath(DWORD pid)
{
    if (pid == NULL) {
        pid = GetCurrentProcessId();
    }

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (hProcess == NULL) {
        return "";
    }

    char path[MAX_PATH];
    DWORD size = MAX_PATH;
    if (GetModuleFileNameExA(hProcess, NULL, path, size) == 0) {
        CloseHandle(hProcess);
        return "";
    }

    CloseHandle(hProcess);
    std::string fullPath(path);
    fullPath = fullPath.substr(0, fullPath.find_last_of("\\/"));
    return fullPath;
}

__int64 WINAPI System::GetRunTime()
{
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
    return duration.count();
}

double WINAPI System::DataSizeConversion(double size, int& type, __int64 time)
{
    const float
        TIME_FACTOR = 1000,
        SIZE_FACTOR = 1024;
    double result = size;
    if (time != -1) {
        result = (TIME_FACTOR / time) * size;
    }
    else if (time == 0) {
        result = 1000;
        type = 3;
    }
    else {
        type = 1;
        while (result >= SIZE_FACTOR) {
            result /= SIZE_FACTOR;
            type++;
        }
    }
    return result;
}

bool WINAPI System::CreateDir(std::string path) {
    // 统一路径分隔符
    for (char& c : path) {
        if (c == '/') c = '\\';
    }

    // 智能截断文件路径
    size_t file_pos = path.find_last_of('\\');
    if (file_pos != std::string::npos && path.find('.', file_pos) != std::string::npos) {
        path = path.substr(0, file_pos);
    }

    // 相对路径补全
    if (path.find(':') == std::string::npos) {
        std::string base = GetRunPath();
        path = path.empty() || (path[0] != '\\' && path.find("\\\\") != 0) 
               ? base + "\\" + path 
               : base + path;
    }

    // 层级式目录创建
    bool created = false;  // 新增创建状态标记
    size_t start = 0;
    while ((start = path.find_first_not_of('\\', start)) != std::string::npos) {
        size_t end = path.find('\\', start);
        std::string segment = path.substr(0, end != std::string::npos ? end : path.size());
        
        // 跳过根目录校验
        if (segment.size() > 3 || segment.back() != '\\') {
            DWORD attrs = GetFileAttributesA(segment.c_str());
            if (attrs == INVALID_FILE_ATTRIBUTES) {
                // 仅当实际创建成功时标记
                if (CreateDirectoryA(segment.c_str(), nullptr)) {
                    created = true;  // 记录成功创建操作
                } else if (GetLastError() != ERROR_ALREADY_EXISTS) {
                    return false;    // 非预期错误立即返回
                }
            } else if (!(attrs & FILE_ATTRIBUTE_DIRECTORY)) {
                return false;  // 路径被文件占用
            }
            // 已存在目录不更新created状态
        }
        
        start = end != std::string::npos ? end + 1 : path.size();
    }
    return created;  // 最终返回创建状态
}

bool WINAPI System::RemoveDir(std::string path)
{
    return RemoveDirectoryA(path.c_str());
}