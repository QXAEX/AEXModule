#include "../h/System.h"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <Psapi.h>
#include <sys/stat.h>
#include <sys/types.h>

std::string WINAPI System::GetTime(bool format)
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm now_tm;
    localtime_s(&now_tm, &now_time_t);
    std::ostringstream oss;
    if (format)
    {
        oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");
    }
    else
    {
        oss << std::put_time(&now_tm, "%Y%m%d%H%M%S");
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

bool WINAPI System::CreateDir(std::string path)
{
    return CreateDirectoryA(path.c_str(), nullptr);
}

bool WINAPI System::RemoveDir(std::string path)
{
    return RemoveDirectoryA(path.c_str());
}