#pragma once

#include <regex>
#include <fstream>

#include <filesystem>
namespace fs = std::filesystem;

#include <libzippp/libzippp.h>
using namespace libzippp;

#include "logger.hpp"

namespace MBase {

class MZip
{
public:
    MZip(const std::string& zipFile)
    {
        zip_ = std::make_unique<ZipArchive>(zipFile);
        if (zip_) {
            zip_->open(ZipArchive::ReadOnly);
        }
    }

    ~MZip()
    {
        if (zip_) {
            zip_->close();
        }
    }

    ZipArchive* GetZip()
    {
        if (zip_) {
            return zip_.get();
        }
        return nullptr;
    }

    std::string ReadXmlFromDocxAsText(const std::string& xmlName)
    {
        std::string strData;
        if (zip_) {
            for (auto& v : zip_->getEntries()) {
                std::string name = v.getName();
                if (xmlName == name) {
                    strData = v.readAsText();
                    break;
                }
            }
        }
        return strData;
    }

    bool ExtractMediaFiles(const std::string& saveDir, std::vector<std::string>& vMediaFiles)
    {
        if (!zip_) {
            return false;
        }

        std::string errMsg;
        MFile::CreateDir(saveDir.c_str(), errMsg);

        if (!errMsg.empty()) {
            LOGGER_ERROR("Extract", "Create Save Dir failed, {}", errMsg);
            return false;
        }

        auto entries = zip_->getEntries();
        if (entries.empty()) {
            LOGGER_ERROR("Extract", "Entries size is zero.");
            return false;
        }

        std::regex rgxWmfFile(R"(word/media/image[0-9]+.[wmf,emf])");
        std::regex rgxJpgFile(R"(word/media/image[0-9]+.[jpg,jpeg])");
        std::regex rgxPngFile(R"(word/media/image[0-9]+.png)");

        for (auto it = entries.begin(); it != entries.end(); ++it) {
            ZipEntry entry = *it;
            std::string name = entry.getName();

            if (std::regex_search(name, rgxWmfFile) || std::regex_search(name, rgxJpgFile)
                || std::regex_search(name, rgxPngFile)) {
                std::uint64_t size = entry.getSize();
                void* binaryData = entry.readAsBinary();
                if (binaryData) {
                    fs::path p(entry.getName());
                    std::string file_name = p.filename().string();
                    std::string name = p.stem().string();
                    std::string extension = p.extension().string();
                    std::string local_path = fmt::format("{}{}", saveDir, file_name);

                    try {
                        //输出文件到本地
                        std::ofstream file(local_path, std::ofstream::binary);
                        file.write((char*)binaryData, size);
                        file.close();
                        delete[] binaryData;

                        vMediaFiles.emplace_back(local_path);
                    }
                    catch (const std::exception& e) {
                        LOGGER_ERROR("Extract",
                                     "Extract Media to {} Failed, Error:{}",
                                     local_path,
                                     e.what());
                    }
                }
            }
            else {
                //TODO 其他图片类型
            }
        }

        return true;
    }

    static void TraverseAllFile(fs::path root_path, ZipArchive* zip)
    {
        if (fs::exists(root_path)) {
            for (auto const& dir_entry : std::filesystem::recursive_directory_iterator{root_path}) {
                fs::path p(dir_entry);
                auto pos = root_path.string().size();
                auto entry = p.string().substr(pos);

                if (fs::is_directory(dir_entry)) {
                    zip->addEntry(entry);
                }
                else {
                    zip->addFile(entry, p.string());
                }
            }
        }
    }

    static bool CreateZipFile(const std::string& zipName, const std::string& appendDir)
    {
        ZipArchive zf(zipName);
        zf.open(ZipArchive::New);

        //遍历appendDir目录
        TraverseAllFile(appendDir, &zf);
        zf.close();

        auto count = zf.getEntries().size();
        return count > 0;
    }

private:
    std::unique_ptr<ZipArchive> zip_;
};
} // namespace MBase