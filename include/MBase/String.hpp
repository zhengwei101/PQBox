#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#include <cctype>

#include <string>
#include <vector>
#include <algorithm>

#undef W2A
#undef A2W

namespace MBase {

enum class ATYPE
{
    GBK,
    UTF8,
    UNKOWN
};

namespace {

int PreNUm(unsigned char byte)
{
    unsigned char mask = 0x80;
    int num = 0;
    for (int i = 0; i < 8; i++) {
        if ((byte & mask) == mask) {
            mask = mask >> 1;
            num++;
        }
        else {
            break;
        }
    }
    return num;
}

bool IsUtf8(unsigned char* data, int len)
{
    int num = 0;
    int i = 0;
    while (i < len) {
        if ((data[i] & 0x80) == 0x00) {
            // 0XXX_XXXX
            i++;
            continue;
        }
        else if ((num = PreNUm(data[i])) > 2) {
            // 110X_XXXX 10XX_XXXX
            // 1110_XXXX 10XX_XXXX 10XX_XXXX
            // 1111_0XXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
            // 1111_10XX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
            // 1111_110X 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
            // preNUm() 返回首个字节8个bits中1的个数，该数量也是该字符所使用的字节数
            i++;
            for (int j = 0; j < num - 1; j++) {
                //判断后面num - 1 个字节是不是都是10开
                if ((data[i] & 0xc0) != 0x80) {
                    return false;
                }
                i++;
            }
        }
        else {
            //其他情况说明不是utf-8
            return false;
        }
    }
    return true;
}

bool IsGBK(unsigned char* data, int len)
{
    int i = 0;
    while (i < len) {
        if (data[i] <= 0x7f) {
            //编码小于等于127,只有一个字节的编码，兼容ASCII
            i++;
            continue;
        }
        else {
            //大于127的使用双字节编码
            if (data[i] >= 0x81 && data[i] <= 0xfe && data[i + 1] >= 0x40 && data[i + 1] <= 0xfe
                && data[i + 1] != 0xf7) {
                i += 2;
                continue;
            }
            else {
                return false;
            }
        }
    }
    return true;
}

bool MBCSToUnicode(const char* input, std::wstring& output, int code_page)
{
    output.clear();
    size_t size = ::MultiByteToWideChar(code_page, 0, input, -1, NULL, 0);
    if (size <= 0)
        return false;

    output.resize(size - 1);

    if (output.empty())
        return true;

    ::MultiByteToWideChar(code_page, 0, input, -1, &output[0], static_cast<int>(output.size()));

    return true;
}

bool MBCSToUnicode(const std::string& input, std::wstring& output, int code_page)
{
    output.clear();
    int size = ::MultiByteToWideChar(
        code_page, 0, input.c_str(), static_cast<int>(input.size()), NULL, 0);

    output.resize(size);

    if (output.empty())
        return true;

    ::MultiByteToWideChar(code_page,
                          0,
                          input.c_str(),
                          static_cast<int>(input.size()),
                          &output[0],
                          static_cast<int>(output.size()));

    return true;
}

bool UnicodeToMBCS(const wchar_t* input, std::string& output, int code_page)
{
    output.clear();
    size_t size = ::WideCharToMultiByte(code_page, 0, input, -1, NULL, 0, NULL, NULL);
    if (size <= 0)
        return false;

    output.resize(size - 1);

    if (output.empty())
        return true;

    ::WideCharToMultiByte(
        code_page, 0, input, -1, &output[0], static_cast<int>(output.size()), NULL, NULL);

    return true;
}

bool UnicodeToMBCS(const std::wstring& input, std::string& output, int code_page)
{
    output.clear();
    int size = ::WideCharToMultiByte(
        code_page, 0, input.c_str(), static_cast<int>(input.size()), NULL, 0, NULL, NULL);

    output.resize(size);

    if (output.empty())
        return true;

    ::WideCharToMultiByte(code_page,
                          0,
                          input.c_str(),
                          static_cast<int>(input.size()),
                          &output[0],
                          static_cast<int>(output.size()),
                          NULL,
                          NULL);

    return true;
}

template <class T>
void LTrim(std::basic_string<T>& s)
{
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
}

template <class T>
void RTrim(std::basic_string<T>& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}
} // namespace

class String
{
public:
    static int ByteSizeFromUtf8First(unsigned char ch)
    {
        int count = -1;

        if ((ch & 0x80) == 0) {
            //这是用来兼容ASSIC的。也就是说一个标准的ASSIC文件就是UTF-8文件
            count = 1;
        }
        else if ((ch & 0xE0) == 0xC0) {
            //0xE0是11100000
            //0xC0是11000000
            //是说前面3个bit 是110 ，那当前的字符和后面的一个字符组成一个汉字
            count = 2;
        }
        else if ((ch & 0xF0) == 0xE0) {
            //0xF0是11110000
            //0xE0是11100000
            //是说前面4个bit 是1110 ，那当前的字符和后面的两个字符组成一个汉字
            count = 3;
        }
        else if ((ch & 0xF8) == 0xF0) {
            count = 4;
        }
        else if ((ch & 0xFC) == 0xF8) {
            count = 5;
        }
        else if ((ch & 0xFE) == 0xFC) {
            count = 6;
        }
        else {
            //throw invalid_utf8_exception(); // Stop on invalid characters
        }

        return count;
    }

    static std::string GetFileDir(const std::string& fileFullPath)
    {
        std::string fileDir;
        std::size_t pos = fileFullPath.rfind('/');
        if (pos == std::string::npos) {
            pos = fileFullPath.rfind('\\');
        }

        if (pos != std::string::npos) {
            fileDir = fileFullPath.substr(0, pos + 1);
        }

        return fileDir;
    }

    static char* SkipSpace(char* str)
    {
        if (str == NULL)
            return NULL;
        char* p = str;
        while (p && *p) {
            if (*p == ' ' || *p == '\t') {
                p++;
            }
            else if (memcmp(p, "&nbsp;", 6) == 0) {
                p += 6;
            }
            else {
                return p;
            }
        }
        return p;
    }

    static std::string GetStringFromU8(const std::string& s) { return s; }

    static std::string GetStringFromU8(std::string&& s) { return std::move(s); }

#if defined(__cpp_lib_char8_t)
    static std::string GetStringFromU8(const std::u8string& s)
    {
        return std::string(s.begin(), s.end());
    }
#endif

    static ATYPE GetCoding(const std::string& data)
    {
        unsigned char uc[sizeof(data)]{};
        std::copy(data.begin(), data.end(), uc);
        int len = static_cast<int>(data.size());
        return GetCoding(uc, len);
    }

    //需要说明的是，IsGBK()是通过双字节是否落在gbk的编码范围内实现的，
    //而utf-8编码格式的每个字节都是落在gbk的编码范围内?
    //所以只有先调用IsUtf8()先判断不是utf-8编码，再调用IsGBK()才有意义
    static ATYPE GetCoding(unsigned char* data, int len)
    {
        ATYPE coding = ATYPE::UNKOWN;
        if (IsUtf8(data, len) == true) {
            coding = ATYPE::UTF8;
        }
        else if (IsGBK(data, len) == true) {
            coding = ATYPE::GBK;
        }
        else {
            coding = ATYPE::UNKOWN;
        }
        return coding;
    }

    static std::wstring A2W(const std::string& s, ATYPE type = ATYPE::UTF8)
    {
        if (s.empty()) {
            return std::wstring();
        }

        std::wstring output(L"");

        switch (type) {
            case ATYPE::GBK:
                MBCSToUnicode(s, output, CP_ACP);
                break;
            case ATYPE::UTF8:
                MBCSToUnicode(s, output, CP_UTF8);
                break;
            default:
                break;
        }
        return output;
    }

    static std::string W2A(const std::wstring& s, ATYPE type = ATYPE::UTF8)
    {
        if (s.empty()) {
            return std::string();
        }

        std::string output;
        switch (type) {
            case ATYPE::GBK:
                UnicodeToMBCS(s, output, CP_ACP);
                break;
            case ATYPE::UTF8:
                UnicodeToMBCS(s, output, CP_UTF8);
                break;
            default:
                break;
        }
        return output;
    }

    static std::string Utf8ToGbk(const std::string& s)
    {
        std::wstring wstr(L"");
        MBCSToUnicode(s, wstr, CP_UTF8);

        std::string output("");
        UnicodeToMBCS(wstr, output, CP_ACP);

        return output;
    }

    static std::string GbkToUtf8(const std::string& s)
    {
        std::wstring wstr(L"");
        MBCSToUnicode(s, wstr, CP_ACP);

        std::string output("");
        UnicodeToMBCS(wstr, output, CP_UTF8);

        return output;
    }

    template <class T>
    static void Trim(std::basic_string<T>& strInOut)
    {
        LTrim(strInOut);
        RTrim(strInOut);
    }

    static bool Replace(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    static void ReplaceAll(std::string& str, const std::string& from, const std::string& to)
    {
        if (from.empty())
            return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    static void ReplaceAll(std::wstring& str, const std::wstring& from, const std::wstring& to)
    {
        if (from.empty())
            return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }

    template <class T>
    static std::basic_string<T> ToLower(std::basic_string<T>& src)
    {
        std::transform(src.begin(), src.end(), src.begin(), ::tolower);
        return src;
    }

    static void Split(const std::string& s,
                      std::vector<std::string>& tokens,
                      const std::string& delimiters /*= " "*/)
    {
        auto lastPos = s.find_first_not_of(delimiters, 0);
        auto pos = s.find_first_of(delimiters, lastPos);
        while (std::string::npos != pos || std::string::npos != lastPos) {
            tokens.push_back(s.substr(lastPos, pos - lastPos));
            lastPos = s.find_first_not_of(delimiters, pos);
            pos = s.find_first_of(delimiters, lastPos);
        }
    }

    static void Split(const std::wstring& s,
                      std::vector<std::wstring>& tokens,
                      const std::wstring& delimiter /*= " "*/)
    {
        auto lastPos = s.find_first_not_of(delimiter, 0);
        auto pos = s.find_first_of(delimiter, lastPos);
        while (std::wstring::npos != pos || std::wstring::npos != lastPos) {
            tokens.push_back(s.substr(lastPos, pos - lastPos));
            lastPos = s.find_first_not_of(delimiter, pos);
            pos = s.find_first_of(delimiter, lastPos);
        }
    }

    static void Splits(std::wstring& s,
                       std::vector<std::wstring>& tokens,
                       std::vector<std::wstring>& delimiters)
    {
        if (delimiters.empty()) {
            return;
        }

        std::wstring src_str(s);
        std::wstring baseDelimiter = delimiters[0];

        for (const auto& d : delimiters) {
            ReplaceAll(src_str, d, baseDelimiter);
        }

        //最后s全部以baseDelimiter为分割
        Split(src_str, tokens, baseDelimiter);
    }

    static bool StrCompare(const wchar_t* string1, const wchar_t* string2)
    {
        return wcscmp(string1, string2) == 0;
    }

    static bool StrCompare(const char* string1, const char* string2)
    {
        return strcmp(string1, string2) == 0;
    }

    static std::wstring ToLower(const std::wstring& wstr)
    {
        std::string s = String::W2A(wstr);
        std::transform(
            s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        return A2W(s);
    }

    static std::wstring VectorToString(const std::vector<std::wstring>& V,
                                       const std::wstring& separator)
    {
        std::wstring result;
        for (const auto& v : V) {
            result += v;
            if (v != V.back()) {
                result += separator;
            }
        }
        return result;
    }
};
} // namespace MBase