#pragma once

#include <sstream>
#include <fstream>
#include <format>

#include <pugixml/pugixml.hpp>

using XMLDoc = pugi::xml_document;
using XMLNode = pugi::xml_node;
using XMLAttr = pugi::xml_attribute;
using XMLResult = pugi::xml_parse_result;

#include "String.hpp"

#define USE_NBSP_REPLACE_SPACE true

namespace MBase {

struct xml_string_writer : pugi::xml_writer
{
    std::string result;
    virtual void write(const void* data, size_t size)
    {
        result.append(static_cast<const char*>(data), size);
    }
};

class MXml
{
public:
    static void WriteXmlFile(const std::string& filepath, XMLDoc* doc, bool toHtml = false)
    {
        if (!doc) {
            return;
        }

        std::stringstream ss;
        doc->print(ss, L"  ");
        std::string str = ss.str();

        if (toHtml) {
            str = Text2Html(str);
        }

        std::ofstream output(filepath);
        if (output) {
            output << str;
            output.close();
        }
        else {
            //std::string err = ::strerror(errno);
        }
    }
    static std::string GetNodeString(const XMLNode& node)
    {
        xml_string_writer writer;
        node.print(writer, L"", pugi::format_raw);
        return writer.result;
    }
    static std::string Text2Html(const std::string& t)
    {
        //全角空格处理
        std::wstring wstr = String::A2W(t);
        String::ReplaceAll(wstr, L"　", L" ");

        std::string text(String::W2A(wstr));
        if (text.empty()) {
            return text;
        }

        size_t i = 0;
        std::string new_text;

        while (text[i] && i < text.size()) {
            int utf8_len = String::ByteSizeFromUtf8First(text[i]);

#ifdef USE_NBSP_REPLACE_SPACE
            if (utf8_len == 1 && text[i] == ' ') {
                if (i + 1 < text.size() && String::ByteSizeFromUtf8First(text[i + 1]) == 1) {
                    if (text[i + 1] == ' ' || text[i + 1] == ')') {
                        new_text += "&nbsp;";

                        i++;
                        continue;
                    }
                }
            }
#endif

            // &amp;
            if (text[i] == '&' && i + 5 < text.size()) {
                if (text[i + 1] == 'a' && text[i + 2] == 'm' && text[i + 3] == 'p'
                    && text[i + 4] == ';') {
                    new_text += '&';
                    i = i + 5;
                    continue;
                }
            }

            if (utf8_len > 0) {
                for (int j = 0; j < utf8_len; ++j) {
                    new_text += text[i];
                    i++;
                }
            }
            else {
                i++;
            }
        }

        String::ReplaceAll(new_text, "&lt;", "<");
        String::ReplaceAll(new_text, "&gt;", ">");

        String::ReplaceAll(new_text, "'judge'", R"("judge")");
        String::ReplaceAll(new_text, "'choice_1'", R"("choice_1")");

        return new_text;
    }
    static std::string GenerateHtmlText(const XMLNode& node)
    {
        std::string body = Text2Html(GetNodeString(node));
        std::string title = String::GetStringFromU8(u8"HTML_Title");
        std::string meta{R"(http-equiv="Content-Type" content="text/html;charset=utf-8")"};
        std::string emphasis{
            ".chinese-emphasis{text-emphasis:dot;-webkit-text-emphasis:dot;-webkit-text-emphasis-"
            "position:under left;text-emphasis-position:under left;}"};

        std::string head = std::format(
            "<head><title>{}</title><meta {} /><style>{}</style></head>", title, meta, emphasis);

        std::stringstream ss;
        ss << "<!DOCTYPE html>\n<html>\n";
        ss << head;
        ss << body;
        ss << "</html>";

        std::string html = ss.str();
        ss.str("");
        return html;
    }
};
} // namespace MBase