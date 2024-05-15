#pragma once

#ifdef _WIN32
#include <io.h>
#define access _access_s
#else
#include <unistd.h>
#endif

#include <tchar.h>
#include <objbase.h>
#include <tlhelp32.h>
#include <ctime>
#include <cctype>

#include <tlhelp32.h>
#include <errno.h>
#include <string.h>

#include <string>
#include <sstream>
#include <fstream>

#include <filesystem>
namespace fs = std::filesystem;

#include <format>

#if __cplusplus >= 201703L || defined(_MSC_VER) && _MSC_VER >= 1911
#define SUPPORT_STD_FILESYSTEM
#endif

#include "MBase/String.hpp"

namespace MBase {

enum class SizeUnit
{
    Byte,
    KB,
    MB,
    GB
};

enum class CopyType
{
    BothFile,
    BothDir,
    SrcFileDstDir
};

struct FilePathDetail
{
    std::wstring fullname;
    std::wstring name;
    std::wstring ext;
    std::wstring dir;
    std::wstring parent_dir;
};

class MFile
{
public:
    static FilePathDetail GetFilePathDetail(const std::wstring& filePath)
    {
        fs::path p(filePath);
        FilePathDetail d;
        d.fullname = p.filename().wstring();
        d.name = p.stem().wstring();
        d.ext = p.extension().wstring();
        d.dir = p.parent_path().wstring();

        fs::path pDir(d.dir);
        d.parent_dir = pDir.parent_path().wstring();
        return d;
    }

    template <typename T>
    inline static bool CheckFileExist(T s)
    {
        return false;
    }

    template <typename T, typename... Args>
    inline static bool CheckFileExist(T t, Args... args)
    {
        if (!CheckFileExist(t)) {
            return false;
        }
        return CheckFileExist(args...);
    }

    static bool CheckFileExist(const std::wstring& s)
    {
        std::string path(MBase::String::W2A(s, ATYPE::GBK));
        return access(path.c_str(), 0) == 0;
    }

    static bool CheckFileExist(const std::string& s)
    {
        std::wstring ws = MBase::String::A2W(s);
        return CheckFileExist(ws);
    }

    static std::wstring GetCurrentExeDir()
    {
        //std::filesystem::path cwd = fs::current_path();
        wchar_t buffer[MAX_PATH] = {0};
        ::GetModuleFileNameW(NULL, buffer, MAX_PATH);

        auto pos = std::wstring(buffer).find_last_of(L"\\/");
        if (pos != std::wstring::npos) {
            return std::wstring(buffer).substr(0, pos);
        }
        return {};
    }

    static std::vector<std::wstring> GetFilesFromDir(const std::wstring& dirPath)
    {
        std::vector<std::wstring> vFilePath;
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_directory()) {
                continue;
            }
            vFilePath.emplace_back(entry.path().wstring());
        }
        return vFilePath;
    }

    static bool CreateDir(const char* file_path, std::string& err_msg)
    {
        fs::path p(file_path);
        if (fs::exists(p)) {
            return true;
        }

        std::error_code ec;
        fs::create_directories(p, ec);
        if (!ec && fs::is_directory(p)) {
            return true;
        }

        err_msg = ec.message();
        return false;
    }

    static bool RenameFileExtension(const std::string& srcFile, const std::string& ext)
    {
        if (!MFile::CheckFileExist(srcFile)) {
            return false;
        }

        fs::path p(srcFile);

        auto dir = p.parent_path().string();
        auto filename = p.stem().string();

        auto newFile = std::format("{}/{}{}", dir, filename, ext);

        auto src = fs::path(srcFile);
        auto dst = fs::path(newFile);

        std::error_code ec;
        fs::rename(src, dst, ec);
        if (!ec) {
            return true;
        }
        return false;
    }

    static std::wstring CopyFile(const std::wstring& src,
                                 const std::wstring& dst,
                                 CopyType type = CopyType::BothFile,
                                 bool bMoved = false)
    {
        auto srcPath = fs::path(src);
        auto dstPath = fs::path(dst);
        if (srcPath.compare(dstPath) == 0) {
            return L"源文件与目标文件路径相同";
        }

        if (!fs::exists(src)) {
            return L"源文件不存在";
        }

        std::error_code ec;
        if (type == CopyType::BothFile) {
            auto p_path = dstPath.parent_path();
            fs::create_directories(p_path, ec);
            if (!ec) {
                fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec);
            }
        }
        else if (type == CopyType::SrcFileDstDir) {
            fs::create_directories(dst, ec);
            if (!ec && fs::is_directory(dstPath)) {
                auto filename = srcPath.filename().wstring();
                std::string dst_file
                    = std::format("{}/{}", MBase::String::W2A(dst), MBase::String::W2A(filename));
                fs::copy_file(src, dst_file, fs::copy_options::overwrite_existing, ec);
            }
        }
        else if (type == CopyType::BothDir) {
            fs::create_directories(dst, ec);
            if (!ec && fs::is_directory(dst)) {
                const auto copyOptions
                    = fs::copy_options::update_existing | fs::copy_options::recursive;
                fs::copy(src, dst, copyOptions);
            }
        }

        if (ec) {
            //failed
            auto msg = std::format("copy_file failed, src:{}, dst:{}, err_msg:{}",
                                   MBase::String::W2A(src),
                                   MBase::String::W2A(dst),
                                   ec.message());
            return MBase::String::A2W(msg);
        }

        return L"";
    }

    static void CopyFileByPath(const std::wstring& srcFile,
                               const std::wstring& dstFile,
                               std::string& errMsg)
    {
        auto src = fs::path(srcFile);
        auto dst = fs::path(dstFile);
        if (src.compare(dst) == 0) {
            return;
        }

        auto p_path = dst.parent_path();
        std::error_code ec;
        if (fs::exists(src)) {
            fs::create_directories(p_path, ec);
            if (!ec) {
                fs::copy_file(src, dst, fs::copy_options::overwrite_existing, ec);
                if (!ec) {
                    //success
                    return;
                }
            }
        }

        //failed
        errMsg = std::format("copy_file failed, src:{}, dst:{}, err_msg:{}",
                             MBase::String::W2A(srcFile),
                             MBase::String::W2A(dstFile),
                             ec.message());
    }

    static void CopyFileToDir(const std::wstring& srcFile,
                              const std::wstring& dstDir,
                              std::string& errMsg)
    {
        std::error_code ec;
        fs::create_directories(dstDir, ec);
        if (!ec && fs::is_directory(dstDir)) {
            auto detail = MFile::GetFilePathDetail(srcFile);
            std::wstring dstFile = std::format(L"{}/{}", dstDir, detail.fullname);
            CopyFileByPath(srcFile, dstFile, errMsg);
        }
    }

    //TODO Need Test
    static void MoveFileToPath(const std::wstring& srcFile,
                               const std::wstring& dstFile,
                               std::string& errMsg)
    {
        auto src = fs::path(srcFile);
        auto dst = fs::path(dstFile);
        auto p_path = dst.parent_path();

        std::error_code ec;
        if (fs::exists(src)) {
            fs::create_directories(p_path, ec);
            if (!ec) {
                fs::rename(src, dst, ec);
                if (!ec) {
                    //success
                    return;
                }
            }
        }

        //failed
        errMsg = std::format("MoveFile failed src:{}, dst:{}, errMsg:{}",
                             MBase::String::W2A(srcFile),
                             MBase::String::W2A(dstFile),
                             ec.message());
    }

    static bool CreateDirectories(const std::wstring& dir)
    {
        return CreateDirectoryTree(dir.c_str(), nullptr);
    }

    static bool CreateDirectoryTree(LPCTSTR szPathTree, LPSECURITY_ATTRIBUTES sa)
    {
        if (CreateDirectory(szPathTree, sa)) {
            return true;
        }

        bool bSuccess = false;
        DWORD dwLastError = GetLastError();

        switch (dwLastError) {
            case ERROR_ALREADY_EXISTS:
                bSuccess = true;
                break;
            case ERROR_PATH_NOT_FOUND:
            {
                TCHAR szPrev[MAX_PATH] = {0};
                LPCTSTR szLast = _tcsrchr(szPathTree, '\\');
                _tcsnccpy_s(szPrev, MAX_PATH, szPathTree, (int)(szLast - szPathTree));
                if (CreateDirectoryTree(szPrev, sa)) {
                    bSuccess = CreateDirectory(szPathTree, sa) != 0;
                    if (!bSuccess) {
                        bSuccess = (GetLastError() == ERROR_ALREADY_EXISTS);
                    }
                }
                else {
                    bSuccess = false;
                }
            } break;
            default:
                break;
        }

        return bSuccess;
    }

    static std::string ReadFile(const std::wstring& filepath)
    {
        std::ifstream ifs(filepath);
        if (ifs) {
            std::stringstream buffer;
            buffer << ifs.rdbuf();
            std::string content = buffer.str();
            ifs.close();
            return content;
        }
        return "";
    }

    static void WriteFile(const std::wstring& filepath, const std::wstring& text)
    {
        if (CreateDirectories(filepath.c_str())) {
            std::wofstream output(filepath, std::ios::out | std::ios::trunc);
            if (output) {
                output << text;
                output.close();
            }
        }
    }

    static std::wstring GetAppDirectory(const HINSTANCE& instance = NULL)
    {
        // 资源路径
        TCHAR buffer[MAX_PATH] = {0};
        ::GetModuleFileName(instance, buffer, MAX_PATH);

        auto pos = std::wstring(buffer).find_last_of(L"\\/");
        if (pos != std::wstring::npos) {
            return std::wstring(buffer).substr(0, pos);
        }
        return {};
    }

    static int DeleteDirectory(const std::wstring& refcstrRootDirectory,
                               bool bDeleteSubdirectories /* = true*/)
    {
        bool bSubdirectory = false; // Flag, indicating whether

        // subdirectories have been found
        std::wstring strFilePath;        // Filepath

        WIN32_FIND_DATA FileInformation; // File information

        //Pattern
        std::wstring strPattern = refcstrRootDirectory + L"\\*.*";

        // Handle to directory
        HANDLE hFile = ::FindFirstFile(strPattern.c_str(), &FileInformation);
        if (hFile != INVALID_HANDLE_VALUE) {
            do {
                if (FileInformation.cFileName[0] != '.') {
                    strFilePath.erase();
                    strFilePath = refcstrRootDirectory + L"\\" + FileInformation.cFileName;

                    if (FileInformation.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                        if (bDeleteSubdirectories) {
                            // Delete subdirectory
                            int iRC = DeleteDirectory(strFilePath, bDeleteSubdirectories);
                            if (iRC)
                                return iRC;
                        }
                        else
                            bSubdirectory = true;
                    }
                    else {
                        // Set file attributes
                        if (::SetFileAttributes(strFilePath.c_str(), FILE_ATTRIBUTE_NORMAL)
                            == FALSE)
                            return ::GetLastError();

                        // Delete file
                        if (::DeleteFile(strFilePath.c_str()) == FALSE)
                            return ::GetLastError();
                    }
                }
            } while (::FindNextFile(hFile, &FileInformation) == TRUE);

            // Close handle
            ::FindClose(hFile);

            DWORD dwError = ::GetLastError();
            if (dwError != ERROR_NO_MORE_FILES)
                return dwError;
            else {
                if (!bSubdirectory) {
                    // Set directory attributes
                    if (::SetFileAttributes(refcstrRootDirectory.c_str(), FILE_ATTRIBUTE_NORMAL)
                        == FALSE)
                        return ::GetLastError();

                    // Delete directory
                    if (::RemoveDirectory(refcstrRootDirectory.c_str()) == FALSE)
                        return ::GetLastError();
                }
            }
        }

        return 0;
    }

    static bool FilePathIsExist(LPCWSTR filepath_in, bool is_directory)
    {
        const DWORD file_attr = ::GetFileAttributesW(filepath_in);
        if (file_attr != INVALID_FILE_ATTRIBUTES) {
            if (is_directory)
                return (file_attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
            else
                return true;
        }
        return false;
    }
};

} // namespace MBase