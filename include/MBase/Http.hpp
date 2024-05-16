#pragma once

#include <stdlib.h>

#include <string>
#include <sstream>
#include <fstream>
#include <format>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <cpp-httplib/httplib.h>

#include "String.hpp"
#include "File.hpp"
#include "Url.hpp"

namespace MBase {

struct DownloadParam
{
    std::string url;
    std::string cdn_host;
    std::string save_dir;
    std::string save_path;
};

class MHttp
{
public:
    static bool DownLoadFile(const DownloadParam& download_param, std::string& errMsg)
    {
        //解析Url
        Url url;
        if (!url.parse(download_param.url)) {
            errMsg = "url is not correct";
            return false;
        }

        std::string save_path;

        if (!download_param.save_dir.empty()) {
            save_path = download_param.save_dir + url.file();
        }
        else {
            save_path = download_param.save_path;
        }

        if (save_path.empty()) {
            errMsg = "save_path is empty";
            return false;
        }

        if (!MFile::CheckFileExist(save_path)) {
            std::string host
                = download_param.cdn_host.empty() ? url.host() : download_param.cdn_host;
            return DownLoadFile(save_path, url.protocol(), host, url.path(), errMsg);
        }

        return true;
    }

    static bool DownLoadFile(const std::string& localPath,
                             const std::string& protol,
                             const std::string& host,
                             const std::string& url,
                             std::string& errMsg)
    {
        char drive[_MAX_DRIVE] = {0};
        char dir[_MAX_DIR] = {0};
        char fname[_MAX_FNAME] = {0};
        char ext[_MAX_EXT] = {0};

        //分割该路径，得到盘符，目录，文件名，后缀名
        _splitpath_s(
            localPath.c_str(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);

        std::string save_dir;
        if (strlen(ext) > 0) {
            char szPath[_MAX_PATH] = {0};
            _makepath_s(szPath, _MAX_PATH, drive, dir, nullptr, nullptr);
            save_dir = szPath;
        }
        else {
            errMsg = std::format("CreateDir Failed: {}", "file path is not correct");
            return false;
        }

        std::string err("");
        if (!MFile::CreateDir(save_dir.c_str(), err)) {
            errMsg = std::format("CreateDir Failed: {}", err);
            return false;
        }

        std::error_code ec;
        std::string strLocalFileTemp = localPath + ".tmp";
        if (fs::exists(fs::path(strLocalFileTemp))) {
            fs::remove(fs::path(strLocalFileTemp), ec);
            if (ec) {
                errMsg = std::format("Remove tmpFile failed: {}", ec.message());
                return false;
            }
        }

        std::fstream file;
        file.open(strLocalFileTemp,
                  std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc);
        if (!file.good()) {
            errMsg = std::format("Open tmpFile failed： {}", strLocalFileTemp);
            return false;
        }

        bool bHttpsUrl = (protol == "https");
        std::string body_str("");

        if (bHttpsUrl) {
            std::string s = host;

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
            httplib::SSLClient cli(s);
#else
            httplib::Client cli(s);
#endif

            auto res = cli.Get(url.c_str(), [&](const char* data, size_t data_length) {
                if (file.is_open()) {
                    file.write(data, data_length);
                    file.flush();
                }
                return true;
            });
        }

        file.close();
         
        fs::rename(strLocalFileTemp, localPath, ec);
        if (!ec) {
            return true;
        }

        errMsg = std::format("rename fileTemp to localPath failed: {}", ec.message());
        return false;
    }
};
} // namespace MBase