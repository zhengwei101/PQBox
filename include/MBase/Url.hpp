#pragma once

#include <string>
#include <map>
#include <vector>
#include <regex>
#include <sstream>

namespace MBase {

class Url
{
public:
    static std::string UrlEncode(std::string text)
    {
        std::ostringstream escaped;
        for (auto i = text.begin(); i != text.end(); ++i) {
            std::string::value_type c = (*i);
            // Keep alphanumeric and other accepted characters intact
            if (isalnum((unsigned char)c) || (c == '-') || (c == '_') || (c == '.') || (c == '~')) {
                escaped << c;
            }
            // else if (c == ' ') {
            //	escaped << '+';
            // }
            else {
                // Any other characters are percent-encoded
                escaped << '%';
                escaped << to_hex((unsigned char)c >> 4);
                escaped << to_hex((unsigned char)c);
            }
        }
        return escaped.str();
    }
    static std::string UrlDecode(std::string text)
    {
        std::ostringstream unescaped;
        for (auto i = text.begin(); i != text.end(); ++i) {
            std::string::value_type c = (*i);
            if (c == '%') {
                if (i[1] && i[2]) {
                    int ch1 = from_hex(i[1]);
                    int ch2 = from_hex(i[2]);
                    char h = (char)(ch1 << 4 | ch2);
                    unescaped << h;
                    i += 2;
                }
            }
            else if (c == '+') {
                unescaped << ' ';
            }
            else {
                unescaped << c;
            }
        }
        return unescaped.str();
    }

    bool parse(const std::string& urlStr)
    {
        static const std::regex expression(
            //       proto                 host               port
            "^(\?:([^:/\?#]+)://)\?(\\w+[^/\?#:]*)(\?::(\\d+))\?"
            //       path                  file       parameters
            "(/\?(\?:[^\?#/]*/)*)\?([^\?#]*)\?(\\\?(.*))\?");

        std::cmatch groups;
        if (!std::regex_match(urlStr.c_str(), groups, expression)) {
            // Invalid url
            return false;
        }

        protocol_ = std::string(groups[1].first, groups[1].second);
        host_ = std::string(groups[2].first, groups[2].second);
        std::string portStr(groups[3].first, groups[3].second);
        pathWithoutFile_ = std::string(groups[4].first, groups[4].second);
        file_ = std::string(groups[5].first, groups[5].second);
        parameter_ = std::string(groups[6].first, groups[6].second);
        path_ = pathWithoutFile_ + file_;

        if (!portStr.empty()) {
            port_ = atoi(groups[3].first);
        }
        else {
            auto it = defaultPortsMap().find(protocol_);
            if (it != defaultPortsMap().end()) {
                port_ = it->second;
            }
            else {
                // Invalid port
                return false;
            }
        }

        return true;
    }

    const std::string& protocol() const { return protocol_; }
    const std::string& host() const { return host_; }
    const int port() const { return port_; }
    const std::string& path() const { return path_; }
    const std::string& pathWithoutFile() const { return pathWithoutFile_; }
    const std::string& file() const { return file_; }
    const std::string& parameter() const { return parameter_; }

    std::string hostPort() const
    {
        std::stringstream ss;
        ss << host_ << ':' << port_;
        return ss.str();
    }

    std::string getParamValue(const std::string& name) const
    {
        if (parameter_.empty()) {
            return std::string();
        }

        std::string params = UrlDecode(parameter_);
        std::smatch result;
        if (std::regex_search(
                params.cbegin(), params.cend(), result, std::regex(name + "=(.*?)&"))) {
            // 匹配具有多个参数的url
            // *? 重复任意次，但尽可能少重复
            return result[1];
        }
        else if (std::regex_search(
                     params.cbegin(), params.cend(), result, std::regex(name + "=(.*)"))) {
            // 匹配只有一个参数的url
            return result[1];
        }
        return std::string();
    }

private:
    static char from_hex(char ch) { return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10; }

    static char to_hex(char code)
    {
        static char hex[] = "0123456789abcdef";
        return hex[code & 15];
    }

    const std::map<std::string, int> initDefaultPortsMap()
    {
        std::map<std::string, int> defaultPortsMap;
        defaultPortsMap["http"] = 80;
        defaultPortsMap["https"] = 443;
        return defaultPortsMap;
    }

    const std::map<std::string, int>& defaultPortsMap()
    {
        static std::map<std::string, int> defaultPortsMap = initDefaultPortsMap();
        return defaultPortsMap;
    }

private:
    int port_{0};
    std::string host_;
    std::string path_;
    std::string file_;
    std::string protocol_;
    std::string parameter_;
    std::string pathWithoutFile_;
};

inline std::ostream& operator<<(std::ostream& os, const Url& obj)
{
    os << "Url [Host = " << obj.host() << ", Protocol = " << obj.protocol()
       << ", Port = " << obj.port() << "]";
    return os;
}

} // namespace MBase