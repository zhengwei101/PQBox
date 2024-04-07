// Browser.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Browser.h"

#include <objbase.h>

#include <delayimp.h>
#pragma comment(lib, "delayimp")

#include <format>

#include "ClientAppBrowser.h"
#include "ClientAppRenderer.h"
#include "ClientRunner.h"
#include "ClientSwitches.h"
#include "MainContext.h"
#include "BrowserDlgManager.h"
using namespace Browser;

#include "MBase/file_tools.hpp"
using namespace MBase;



static int CheckDelayException(int exception_value)
{
    if (exception_value == VcppException(ERROR_SEVERITY_ERROR, ERROR_MOD_NOT_FOUND) ||
        exception_value == VcppException(ERROR_SEVERITY_ERROR, ERROR_PROC_NOT_FOUND))
    {
        // This example just executes the handler.
        return EXCEPTION_EXECUTE_HANDLER;
    }
    // Don't attempt to handle other errors
    return EXCEPTION_CONTINUE_SEARCH;
}

static bool TryDelayLoadAllImports(LPCSTR szDll)
{
    __try
    {
        return SUCCEEDED(__HrLoadAllImportsForDll(szDll));
    }
#pragma warning(suppress : 6320)
    __except (CheckDelayException(GetExceptionCode()))
    {
        return false;
    }

    //Failed to delay load functions
    return false;
}

static bool LoadZipResource()
{
    HRSRC hResource = ::FindResource(
        CPaintManagerUI::GetResourceDll(), MAKEINTRESOURCE(IDR_ZIPRES1), _T("ZIPRES"));
    if (hResource)
    {
        HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
        if (hGlobal)
        {
            DWORD dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
            if (dwSize > 0)
            {
                CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
            }
            ::FreeResource(hGlobal);
            return (dwSize > 0);
        }
    }

    return false;
}

#ifdef CEF_USE_SANDBOX
// The cef_sandbox.lib static library may not link successfully with all VS versions.
#pragma comment(lib, "cef_sandbox.lib")
#endif

static int RunMain(HINSTANCE hInstance, int nCmdShow)
{
    // 在项目属性->连接器->输入，延迟加载 libcef.dll

    TCHAR szCEFDllPath[MAX_PATH] = {0};
    TCHAR drive[_MAX_DRIVE] = {0};
    TCHAR dir[_MAX_DIR] = {0};
    //TCHAR fname[_MAX_FNAME] = {0};
    //TCHAR ext[_MAX_EXT] = {0};

    if (NULL == GetModuleFileName(NULL, szCEFDllPath, MAX_PATH))
        return false;

    //分割该路径，得到盘符，目录，文件名，后缀名
    _tsplitpath_s(szCEFDllPath, drive, _MAX_DRIVE, dir, _MAX_DIR, nullptr, 0, nullptr, 0);

    std::wstring cefDir{dir};
    cefDir += L"cef\\";

    //This is required because it seems some of CEF itself loads DLL using relative paths
    if (!SetDllDirectory(cefDir.c_str()))
    {
        const DWORD dwLastError = GetLastError();
        std::wstring sError =
            std::format(L"Failed to set the DLL search directories to include {}. Error: {}",
                        cefDir,
                        dwLastError);
        return false;
    }

    _tmakepath_s(szCEFDllPath, _MAX_PATH, drive, cefDir.c_str(), _T("libcef"), _T(".dll"));

    if (LoadLibrary(szCEFDllPath) == nullptr)
    {
        std::wstring sError = L"Could not load CEF. Please provide the binary files for CEF in a "
                              L"directory called CEF below this application's directory.";
        return false;
    }

    if (!SetDllDirectory(nullptr))
    {
        const DWORD dwLastError = GetLastError();
        std::wstring sError = std::format(
            L"Failed to restore DLL search directories to default. Error: {}", dwLastError);
        return false;
    }

    if (!TryDelayLoadAllImports("libcef.dll"))
    {
        std::wstring sError =
            L"Failed  to resolve imports for CEF. Please provide the binary files for CEF in a "
            L"directory called CEF below this application's directory.";
        return false;
    }

        // Enable High-DPI support on Windows 7 or newer.
    CefEnableHighDPISupport();

    void* sandbox_info = nullptr;



#ifdef CEF_USE_SANDBOX
    CefScopedSandboxInfo scoped_sandbox;
    sandbox_info = scoped_sandbox.sandbox_info();
#endif

    // Parse command-line arguments for use in this method.
    CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
    command_line->InitFromString(::GetCommandLineW());

    // Create a ClientApp of the correct type.
    CefRefPtr<CefApp> app;
    if (ClientApp::IsBrowser(command_line))
        app = new ClientAppBrowser();
    else if (ClientApp::IsRenderer(command_line))
        app = new ClientAppRenderer();
    else
        app = new ClientAppOther();

    // Provide CEF with command-line arguments.
    CefMainArgs main_args(hInstance);

    // Execute the secondary process, if any.
    int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
    if (exit_code >= 0)
        return exit_code;

    // Create the main context object.
    auto context = std::make_unique<MainContext>(command_line, true);

    // Specify CEF global settings here.
    CefSettings settings;

#if !defined(CEF_USE_SANDBOX)
    settings.no_sandbox = true;
#endif

    // Populate the settings based on command line arguments.
    context->PopulateSettings(&settings);

    WCHAR szBuffer[MAX_PATH];
    ::ZeroMemory(&szBuffer, MAX_PATH);
    GetTempPathW(MAX_PATH, szBuffer);
    std::wstring sBuffer = szBuffer;
    sBuffer += L"Browser";
    CefString(&settings.cache_path).FromWString(sBuffer);

    //settings.command_line_args_disabled = true;

    CefString(&settings.locale).FromASCII("zh-CN");

#ifndef _DEBUG
    settings.log_severity = LOGSEVERITY_DISABLE;
#endif

    settings.multi_threaded_message_loop = false;
    //settings.single_process = true;

    std::string url = command_line->GetSwitchValue(Switches::kUrl);
    std::string width = command_line->GetSwitchValue("width");
    std::string height = command_line->GetSwitchValue("height");

    if (url.empty())
    {
        url = "http://www.baidu.com";
        // Set the main URL.
        command_line->AppendSwitchWithValue(Switches::kUrl, url);
    }

    // Initialize CEF.
    context->Initialize(main_args, settings, app, sandbox_info);

    // Register scheme handlers.
    //ClientRunner::RegisterSchemeHandlers();

    // Create the first window.
    HRESULT Hr = ::CoInitialize(NULL);
    if (FAILED(Hr))
        return 0;

    // 初始化UI管理器
    DuiLib::CPaintManagerUI::SetInstance(hInstance);

    // 初始化资源
#ifdef _DEBUG
    CPaintManagerUI::SetResourceType(UILIB_RESTYPE::FILE);
    std::wstring uiPath = FileTools::GetAppDirectory(hInstance);
    uiPath = std::format(L"{}\\uires\\browser\\", uiPath);
    CPaintManagerUI::SetResourcePath(uiPath.c_str());
#else
    CPaintManagerUI::SetResourceType(UILIB_RESTYPE::ZIPRESOURCE);
    if (!LoadZipResource())
    {
        return false;
    }
#endif

    CefRect rect = CefRect();
    rect.width = width.empty() ? 1024 : atoi(width.c_str());
    rect.height = height.empty() ? 700 : atoi(height.c_str());
    auto pDlg = context->GetBrowserDlgManager()->CreateBrowserDlg(NULL,
                                                                  true, // Show controls.
                                                                  rect, // Use default system size.
                                                                  url); // Use default URL.

    if (pDlg && pDlg->GetHWND())
    {
        pDlg->CenterWindow();
    }

    //DuiLib::CPaintManagerUI::MessageLoop();
    CefRunMessageLoop();

    DuiLib::CPaintManagerUI::Term();

    // Shut down CEF.
    context->Shutdown();

    ::CoUninitialize();

    PostQuitMessage(0);

    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    return RunMain(hInstance, nCmdShow);
}
