#include "../h/Text.h"
#include <locale>
#include <codecvt>
#include <windows.h>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")
std::string __stdcall Text::text_join(std::vector<std::string> text_list, std::string delimiter) {
    std::string result;
    for (size_t i = 0; i < text_list.size(); ++i) {
        if (i > 0) result += delimiter;
        result += text_list[i];
    }
    return result;
}

std::vector<std::string> __stdcall Text::text_split_len(std::string text, size_t length) {
    std::vector<std::string> result;
    if (length > 0) {
        for (size_t i = 0; i < text.size(); i += length) {
            result.push_back(text.substr(i, length));
        }
    }
    return result;
}

std::string __stdcall Text::text_left_del(std::string text, size_t length) {
    if (length <= 0) {
        return "";
    }
    if (static_cast<size_t>(length) >= text.size()) {
        return "";
    }
    return text.substr(length);
}

std::string __stdcall Text::text_right_del(std::string text, size_t length) {
    if (length <= 0) {
        return "";
    }
    if (static_cast<size_t>(length) >= text.size()) {
        return "";
    }
    return text.substr(0, text.size() - length);
}

int __stdcall Text::text_find(std::string text, std::string find_text) {
    size_t position = text.find(find_text);
    if (position != std::string::npos) {
        return static_cast<int>(position);
    }
    return -1;
}

std::string __stdcall Text::text_replace(std::string text, std::string find_text, std::string replace_text, size_t count) {
    size_t start_pos = 0;
    while ((start_pos = text.find(find_text, start_pos)) != std::string::npos) {
        text.replace(start_pos, find_text.length(), replace_text);
        start_pos += replace_text.length();
        if (--count == 0) {
            break;
        }
    }
    return text;
}

std::wstring __stdcall Text::text_to_wstr(std::string text) {
    _bstr_t bstr(text.c_str());
    return (wchar_t*)bstr;
}

std::string __stdcall Text::text_to_str(std::wstring text) {
    _bstr_t bstr(text.c_str());
    return (char*)bstr;
}

std::string __stdcall Text::text_random_num(size_t length) {
    const std::string characters = "0123456789";
    std::random_device rd;  // 其随机数生成器
    std::mt19937 generator(rd());  // 用随机数生成器初始化

    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string result;
    for (size_t i = 0; i < length; ++i) {
        result += characters[distribution(generator)];
    }
    return result;
}

std::string __stdcall Text::text_random_alpha(size_t length) {
    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string result;
    for (size_t i = 0; i < length; ++i) {
        result += characters[distribution(generator)];
    }
    return result;
}

std::string __stdcall Text::text_random_alnum(size_t length) {
    const std::string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string result;
    for (size_t i = 0; i < length; ++i) {
        result += characters[distribution(generator)];
    }
    return result;
}

std::string __stdcall Text::text_random_special(size_t length) {
    const std::string characters = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!@#$%^&*()_+[]{}|;:,.<>?/";
    std::random_device rd;
    std::mt19937 generator(rd());

    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    std::string result;
    for (size_t i = 0; i < length; ++i) {
        result += characters[distribution(generator)];
    }
    return result;
}

std::string __stdcall Text::text_base64_encode(std::string text) {
    std::string encoded_text;
    int i = 0;
    int j = 0;
    unsigned char char_array_3[3]{}, char_array_4[4]{};

    for (size_t bytes_read = 0; bytes_read < text.size(); ++bytes_read) {
        char_array_3[i++] = text[bytes_read];
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++) {
                encoded_text += base64_chars[char_array_4[i]];
            }
            i = 0;
        }
    }

    if (i > 0) {
        for (j = i; j < 3; j++) {
            char_array_3[j] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++) {
            encoded_text += base64_chars[char_array_4[j]];
        }

        while (i++ < 3) {
            encoded_text += '=';
        }
    }

    return encoded_text;
}

std::string __stdcall Text::text_base64_decode(std::string text) {
    std::string decoded_text;
    int i = 0;
    int j = 0;
    int in_ = 0;
    unsigned char char_array_4[4]{}, char_array_3[3]{};

    for (size_t bytes_read = 0; bytes_read < text.size(); ++bytes_read) {
        if (text[bytes_read] == '=') {
            break;
        }
        size_t index = base64_chars.find(text[bytes_read]);
        if (index == std::string::npos) {
            continue;
        }
        char_array_4[i++] = text[bytes_read];
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            }
            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; i < 3; i++) {
                decoded_text += char_array_3[i];
            }
            i = 0;
        }
    }

    if (i > 0) {
        for (j = i; j < 4; j++) {
            char_array_4[j] = 0;
        }

        for (j = 0; j < 4; j++) {
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; j < i - 1; j++) {
            decoded_text += char_array_3[j];
        }
    }

    return decoded_text;
}

std::string __stdcall Text::text_gb2312_to_utf8(std::string text) {
    // 将GB2312字符串转换为UTF-8
    int wideCharLength = MultiByteToWideChar(936 /* GB2312 */, 0, text.c_str(), -1, nullptr, 0);
    if (wideCharLength == 0) {
        throw std::runtime_error("Conversion to wide char failed");
    }

    std::wstring wideString(wideCharLength, L'\0');
    MultiByteToWideChar(936, 0, text.c_str(), -1, &wideString[0], wideCharLength);

    int utf8Length = WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (utf8Length == 0) {
        throw std::runtime_error("Conversion to UTF-8 failed");
    }

    std::string utf8String(utf8Length, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wideString.c_str(), -1, &utf8String[0], utf8Length, nullptr, nullptr);

    return utf8String;
}

std::string __stdcall Text::text_utf8_to_gb2312(std::string text) {
    // 将UTF-8字符串转换为GB2312
    int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, nullptr, 0);
    if (wideCharLength == 0) {
        throw std::runtime_error("Conversion to wide char failed");
    }

    std::wstring wideString(wideCharLength, L'\0');
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), -1, &wideString[0], wideCharLength);

    int gb2312Length = WideCharToMultiByte(936 /* GB2312 */, 0, wideString.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (gb2312Length == 0) {
        throw std::runtime_error("Conversion to GB2312 failed");
    }

    std::string gb2312String(gb2312Length, '\0');
    WideCharToMultiByte(936, 0, wideString.c_str(), -1, &gb2312String[0], gb2312Length, nullptr, nullptr);

    return gb2312String;
}

std::string __stdcall Text::text_unicode_to_utf8(std::wstring text)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), NULL, 0, NULL, NULL);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, text.c_str(), (int)text.size(), &result[0], size_needed, NULL, NULL);
    return result;
}

std::wstring __stdcall Text::text_utf8_to_unicode(std::string text)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)text.size(), NULL, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), (int)text.size(), &result[0], size_needed);
    return result;
}

std::string __stdcall Text::text_unicode_to_ascii(std::wstring text)
{
    int size_needed = WideCharToMultiByte(CP_ACP, 0, text.c_str(), (int)text.size(), NULL, 0, NULL, NULL);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_ACP, 0, text.c_str(), (int)text.size(), &result[0], size_needed, NULL, NULL);
    return result;
}

std::wstring __stdcall Text::text_ascii_to_unicode(std::string text)
{
    int size_needed = MultiByteToWideChar(CP_ACP, 0, text.c_str(), (int)text.size(), NULL, 0);
    std::wstring result(size_needed, 0);
    MultiByteToWideChar(CP_ACP, 0, text.c_str(), (int)text.size(), &result[0], size_needed);
    return result;
}

std::string __stdcall Text::text_dec_to_any(size_t num, int base) {
    if (base < 2 || base > 36) {
        throw std::invalid_argument("基数必须在2到36之间");
    }

    if (num == 0) {
        return "0";
    }

    std::string result;

    while (num > 0) {
        int remainder = num % base;
        num /= base;

        if (remainder < 10) {
            result += ('0' + remainder);
        }
        else {
            result += ('A' + remainder - 10);
        }
    }

    std::reverse(result.begin(), result.end());
    return result;
}

size_t __stdcall Text::text_any_to_dec(std::string num, int base) {
    if (base < 2 || base > 36) {
        throw std::invalid_argument("基数必须在2到36之间");
    }

    size_t result = 0;

    for (char digit : num) {
        int value;
        if (digit >= '0' && digit <= '9') {
            value = digit - '0';
        }
        else if (digit >= 'A' && digit <= 'Z') {
            value = digit - 'A' + 10;
        }
        else {
            throw std::invalid_argument("无效的数字字符");
        }

        if (value >= base) {
            throw std::invalid_argument("字符超出基数范围");
        }

        result = result * base + value;
    }

    return result;
}