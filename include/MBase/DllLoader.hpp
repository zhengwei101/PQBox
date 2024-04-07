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
 *     Advapi32()
 *         : m_dll(L"Advapi32.dll")
 *     {
 *         getUserName = advapi32["GetUserNameW"];
 *     }
 *     decltype(GetUserNameW)* getUserName;
 * private: 
 *     MBase::DllLoader m_dll;
 * };
 * 
 * Advapi32 adv;
 * #define INFO_BUFFER_SIZE 32767
 * TCHAR infoBuf[INFO_BUFFER_SIZE] = {};
 * DWORD bufCharCount = INFO_BUFFER_SIZE;
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
        : m_ptr(ptr)
    {
    }

    template <typename T, typename = std::enable_if_t<std::is_function_v<T>>>
    operator T*() const
    {
        return reinterpret_cast<T*>(m_ptr);
    }

private:
    FARPROC m_ptr;
};

class DllLoader
{
public:
    explicit DllLoader(LPCTSTR filename)
        : m_hModule(LoadLibraryEx(filename, NULL, NULL))
    {
    }

    ~DllLoader() { FreeLibrary(m_hModule); }

    HMODULE GetHModule() { return m_hModule; }

    ProcPtr operator[](LPCSTR proc_name) const
    {
        return ProcPtr(GetProcAddress(m_hModule, proc_name));
    }

    ProcPtr GetInterface(LPCSTR proc_name) const
    {
        return ProcPtr(GetProcAddress(m_hModule, proc_name));
    }

private:
    HMODULE m_hModule = nullptr;
};

} // namespace MBase
