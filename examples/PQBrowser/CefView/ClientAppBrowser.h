#ifndef __CLIENT_APP_BROWSER_H__
#define __CLIENT_APP_BROWSER_H__
#pragma once

#include <set>

#include "ClientApp.h"
// clang-format off
namespace Browser {

// Client app implementation for the browser process.
class ClientAppBrowser
    : public ClientApp
    , public CefBrowserProcessHandler
{
public:
    // Interface for browser delegates. All Delegates must be returned via
    // CreateDelegates. Do not perform work in the Delegate
    // constructor. See CefBrowserProcessHandler for documentation.
    class Delegate : public virtual CefBaseRefCounted 
    {
    public:
        virtual void OnBeforeCommandLineProcessing(CefRefPtr<ClientAppBrowser> app,
                                                   CefRefPtr<CefCommandLine> command_line){}

        virtual void OnRegisterCustomPreferences(CefRefPtr<ClientAppBrowser> app,
                                                 cef_preferences_type_t type,
                                                 CefRawPtr<CefPreferenceRegistrar> registrar) {}

        virtual void OnContextInitialized(CefRefPtr<ClientAppBrowser> app) {}

        virtual void OnBeforeChildProcessLaunch(CefRefPtr<ClientAppBrowser> app, 
                                                CefRefPtr<CefCommandLine> command_line){}

    };

    typedef std::set<CefRefPtr<Delegate>> DelegateSet;

    ClientAppBrowser();

    // Called to populate |settings| based on |command_line| and other global state.
     static void PopulateSettings(CefRefPtr<CefCommandLine> command_line,
                                  CefSettings& settings);
private:
     // Registers cookieable schemes. 
     static void RegisterCookieableSchemes(std::vector<std::string>& cookieable_schemes) {};

    // Creates all of the Delegate objects.
    static void CreateDelegates(DelegateSet& delegates);

    // CefApp methods.
    //用来向主进程传递命令行参数
    void OnBeforeCommandLineProcessing(const CefString& process_type, 
                                       CefRefPtr<CefCommandLine> command_line) override;

    void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

    CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override
    {
        return this;
    }

    // CefBrowserProcessHandler methods.
    void OnRegisterCustomPreferences(cef_preferences_type_t type,
                                     CefRawPtr<CefPreferenceRegistrar> registrar) override;
    void OnContextInitialized() override;

    //用来向子进程传递特定的命令行参数
    void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) override;
    
    void OnScheduleMessagePumpWork(int64 delay) override;

private:
    // Set of supported Delegates.
    DelegateSet m_delegates;

    IMPLEMENT_REFCOUNTING(ClientAppBrowser);
    DISALLOW_COPY_AND_ASSIGN(ClientAppBrowser);
};
} // namespace Browser
// clang-format on
#endif
