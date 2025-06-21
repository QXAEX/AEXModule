#include "../../../h/Network/Http.h"
#include <sstream>
// MIME映射表
std::unordered_map<std::string, std::string> mime_types = {
    // 基础文档
    {".html",   "text/html; charset=utf-8"},
    {".htm",    "text/html; charset=utf-8"},
    {".txt",    "text/plain; charset=utf-8"},
    {".xml",    "text/xml; charset=utf-8"},
    {".pdf",    "application/pdf"},
    // 代码资源
    {".js",     "application/javascript"},
    {".mjs",    "application/javascript"},
    {".css",    "text/css"},
    {".json",   "application/json"},
    {".wasm",   "application/wasm"},
    // 图像类型
    {".png",    "image/png"},
    {".jpg",    "image/jpeg"},
    {".jpeg",   "image/jpeg"},
    {".gif",    "image/gif"},
    {".webp",   "image/webp"},
    {".svg",    "image/svg+xml"},
    {".ico",    "image/x-icon"},
    {".bmp",    "image/bmp"},
    {".tiff",   "image/tiff"},
    // 字体文件
    {".woff",   "font/woff"},
    {".woff2",  "font/woff2"},
    {".ttf",    "font/ttf"},
    {".otf",    "font/otf"},
    {".eot",    "application/vnd.ms-fontobject"},
    // 多媒体
    {".mp3",    "audio/mpeg"},
    {".wav",    "audio/wav"},
    {".mp4",    "video/mp4"},
    {".webm",   "video/webm"},
    {".ogg",    "audio/ogg"},
    {".ogv",    "video/ogg"},
    // 数据文件
    {".csv",    "text/csv; charset=utf-8"},
    {".xls",    "application/vnd.ms-excel"},
    {".xlsx",   "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {".doc",    "application/msword"},
    {".docx",   "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    // 压缩文件
    {".zip",    "application/zip"},
    {".tar",    "application/x-tar"},
    {".gz",     "application/gzip"},
    {".7z",     "application/x-7z-compressed"},
    {".rar",    "application/vnd.rar"},
    // 安全相关
    {".pem",    "application/x-pem-file"},
    {".cer",    "application/pkix-cert"},
    {".crt",    "application/x-x509-ca-cert"},
    // 特殊类型
    {".yaml",   "application/x-yaml"},
    {".sh",     "application/x-shellscript"},
    {".dmg",    "application/x-apple-diskimage"},
    {".exe",    "application/octet-stream"},
    {".msi",    "application/x-msi"},
    // Web应用扩展
    {".webmanifest", "application/manifest+json"},
    {".3mf",    "model/3mf"},
    {".apk",    "application/vnd.android.package-archive"}
};

// 获取MIME类型
std::string get_mime_type(const std::string& path) {
    size_t dot_pos = path.find_last_of('.');
    if (dot_pos != std::string::npos) {
        std::string ext = path.substr(dot_pos);
        auto it = mime_types.find(ext);
        return (it != mime_types.end()) ? it->second : "application/octet-stream";
    }
    return "text/plain";
}
bool HTTP::SendTypeText(SOCKET client, Byteset data, HTTP::HTTPHANDLE headers)
{
    std::ostringstream oss;
    std::string content = std::string(data.begin(), data.end());
    oss << std::string(headers["Protocol"].begin(), headers["Protocol"].end()) << " 200 OK\r\n"
        << "Content-Type: text/html\r\n"
        << "Content-Length: " << data.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << content;
    Byteset _data = oss.str();
    return send(client, (PCHAR)_data.data(), _data.size(), 0) != SOCKET_ERROR;
}

bool HTTP::SendTypeBinary(SOCKET client, Byteset data, HTTP::HTTPHANDLE headers)
{
    std::ostringstream header;
    header << std::string(headers["Protocol"].begin(), headers["Protocol"].end()) << " 200 OK\r\n"
        << "Content-Type: " << get_mime_type((PCHAR)headers["Path"].data()) << "\r\n"
        << "Content-Length: " << data.size() << "\r\n"
        << "Connection: close\r\n\r\n";
    Byteset _data = Byteset(header.str()).append(data);
    return send(client, (PCHAR)_data.data(), _data.size(), 0) != SOCKET_ERROR;
}
