#pragma once

/**
 * Examples:
 * --------------------------------------------------------
 * //Demo1: 
 * MBase::DllLoader shell32(L"Shell32.dll");
 * decltype(ShellAboutW)* shellAbout = shell32["ShellAboutW"];
 * if(shellAbout) {
 *     shellAbout(NULL, L"hello", L"world", NULL);
 *  }
 *  else {
 *     wprintf(L"Could not load Shell32.dll, Error #%d.\n", GetLastError());
 * }
 * 
 * //Demo2
 * MBase::DllLoader kernel32(L"kernel32.dll");
 * // Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
 * SYSTEM_INFO si;
 * ZeroMemory(&si, sizeof(SYSTEM_INFO));
 * decltype(GetNativeSystemInfo)* getSysInfo = kernel32["GetNativeSystemInfo"];
 * if (getSysInfo) { getSysInfo(&si); } else { GetSystemInfo(&si); }
 * 
 * //Demo3
 * class Advapi32
 * {
 *     MBase::DllLoader dll_{L"Advapi32.dll"};
 * public:
 *     decltype(GetUserNameW)* getUserName = dll_["GetUserNameW"];
 * };
 * 
 * #define INFO_BUFFER_SIZE 256
 * TCHAR infoBuf[INFO_BUFFER_SIZE] = {};
 * DWORD bufCharCount = INFO_BUFFER_SIZE;
 * 
 * Advapi32 adv;
 * adv.getUserName(infoBuf, &bufCharCount);
 * wprintf(L"Username:  %s\n", infoBuf);
 * --------------------------------------------------------
 */

#include <type_traits>
#include <windows.h>

namespace MBase
{
class ProcPtr
{
public:
    explicit ProcPtr(FARPROC ptr)
        : ptr_(ptr)
    {
    }

    template <typename T, typename = std::enable_if_t<std::is_function_v<T>>>
    operator T*() const
    {
        return reinterpret_cast<T*>(ptr_);
    }

private:
    FARPROC ptr_;
};

class DllLoader
{
public:
    explicit DllLoader(LPCTSTR filename)
        : hModule_(LoadLibraryEx(filename, NULL, NULL))
    {
    }

    ~DllLoader() { FreeLibrary(hModule_); }

    HMODULE GetHModule() { return hModule_; }

    ProcPtr operator[](LPCSTR proc_name) const
    {
        return ProcPtr(GetProcAddress(hModule_, proc_name));
    }

    ProcPtr GetInterface(LPCSTR proc_name) const
    {
        return ProcPtr(GetProcAddress(hModule_, proc_name));
    }

private:
    HMODULE hModule_ = nullptr;
};

} // namespace MBase
