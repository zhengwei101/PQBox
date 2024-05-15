#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>

#include <ctime>

#include <array>
#include <string>
#include <random>
#include <chrono>
#include <algorithm>

#include "String.hpp"
#include "File.hpp"
#include "Md5.hpp"
#include "Url.hpp"

struct InstanceInfo
{
    HWND hWnd = nullptr;
    std::wstring mutex_name;
    std::wstring class_name;
    std::wstring window_name;
};

namespace MBase {

namespace {
GUID CreateGuid()
{
    GUID guid;
    CoCreateGuid(&guid);
    return guid;
}

std::string GuidToString(const GUID& guid)
{
    char buf[64] = {0};
    _snprintf_s(buf,
                sizeof(buf),
                "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X",
                guid.Data1,
                guid.Data2,
                guid.Data3,
                guid.Data4[0],
                guid.Data4[1],
                guid.Data4[2],
                guid.Data4[3],
                guid.Data4[4],
                guid.Data4[5],
                guid.Data4[6],
                guid.Data4[7]);
    return std::string(buf);
}
} // namespace

class Process
{
public:
    static std::wstring GetUUID()
    {
        std::string uuid = GuidToString(CreateGuid());
        if (!uuid.empty()) {
            return String::A2W(uuid);
        }
        return L"";
    }

    template <class T>
    static std::wstring GetDecimal(T in_Value, int in_DecimalSeparator)
    {
        //! Return a decimal in base of decimal separator
        std::wstringstream ss;
        ss << std::setiosflags(std::ios::fixed | std::ios::showpoint | std::ios::right);
        ss << std::setprecision(in_DecimalSeparator) << in_Value;
        return ss.str();
    }

    static std::wstring GetTimeMd5()
    {
        std::time_t ms = std::time(nullptr);
        std::string strTime = std::to_string(ms);
        std::string md5 = Md5::BufferDigest(strTime);
        return String::A2W(md5);
    }

    static bool IsHaveInstance(HWND hWnd,
                               std::wstring mutexName,
                               std::wstring className,
                               std::wstring windowName)
    {
        InstanceInfo i;
        i.hWnd = hWnd;
        i.mutex_name = mutexName;
        i.class_name = className;
        i.window_name = windowName;
        return IsHaveInstance(i);
    }

    static bool IsHaveInstance(const InstanceInfo& info)
    {
        // µ¥ÊµÀý¼ì²â
        bool bExist = false;
        HWND hWnd = nullptr;

        do {
            if (info.hWnd && IsWindow(info.hWnd)) {
                bExist = true;
                hWnd = info.hWnd;
                break;
            }

            HANDLE hMutex = ::CreateMutexW(NULL, TRUE, info.mutex_name.c_str());
            if (hMutex != NULL && GetLastError() == ERROR_ALREADY_EXISTS) {
                bExist = true;
                hWnd = FindWindowW(info.class_name.c_str(), info.window_name.c_str());
                ::ReleaseMutex(hMutex);
                break;
            }

        } while (0);

        if (bExist && hWnd) {
            ShowWindow(hWnd, SW_RESTORE);
            BringWindowToTop(hWnd);
            SetForegroundWindow(hWnd);
        }

        return bExist;
    }

    static void StartExe(const std::wstring& exePath, const std::wstring& cmdLine = L"")
    {
        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        PROCESS_INFORMATION pi;
        ZeroMemory(&pi, sizeof(pi));

        GetStartupInfo(&si);
        si.wShowWindow = SW_SHOW;
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES | STARTF_RUNFULLSCREEN;

        // Start the child process.
        BOOL bRet = CreateProcess(exePath.c_str(),
                                  (LPWSTR)cmdLine.c_str(),
                                  NULL,  // Process handle not inheritable
                                  NULL,  // Thread handle not inheritable
                                  FALSE, // Set handle inheritance to FALSE
                                  0,     // No creation flags
                                  NULL,  // Use parent's environment block
                                  NULL,  // Use parent's starting directory
                                  &si,   // Pointer to STARTUPINFO structure
                                  &pi    // Pointer to PROCESS_INFORMATION structure
        );

        // Wait until child process exits.
        ::WaitForSingleObject(pi.hProcess, INFINITE);

        // Close process and thread handles.
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }

    static void KillProcessByName(const std::wstring& name)
    {
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
        PROCESSENTRY32W pEntry{};
        pEntry.dwSize = sizeof(pEntry);
        BOOL hRes = Process32FirstW(hSnapShot, &pEntry);
        while (hRes) {
            std::wstring exe_file{pEntry.szExeFile};
            std::transform(exe_file.begin(), exe_file.end(), exe_file.begin(), ::tolower);
            if (exe_file == name) {
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
                if (hProcess != NULL && pEntry.th32ProcessID != GetCurrentProcessId()) {
                    TerminateProcess(hProcess, 9);
                    CloseHandle(hProcess);
                }
            }
            hRes = Process32NextW(hSnapShot, &pEntry);
        }
        CloseHandle(hSnapShot);
    }

    static bool IsValidSuffix(const std::wstring& s)
    {
        auto target = String::ToLower(s);
        const static std::array<std::wstring, 11> suffixs{L"ppt",
                                                          L"pptx",
                                                          L"doc",
                                                          L"docx",
                                                          L"mp3",
                                                          L"mp4",
                                                          L"jpg",
                                                          L"jpeg",
                                                          L"png",
                                                          L"swf",
                                                          L"pdf"};

        auto got = std::find_if(suffixs.begin(), suffixs.end(), [&](const std::wstring& suffix) {
            return suffix == target;
        });

        return (got != std::end(suffixs));
    }
};
} // namespace MBase