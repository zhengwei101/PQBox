#include "stdafx.h"
#include "ClientAppBrowser.h"
#include "ClientSwitches.h"
#include "include/base/cef_logging.h"
#include "include/cef_cookie.h"

namespace Browser
{
ClientAppBrowser::ClientAppBrowser()
{
    CreateDelegates(m_delegates);
}

// static
void ClientAppBrowser::PopulateSettings(CefRefPtr<CefCommandLine> command_line,
                                        CefSettings& settings)
{
#if (defined(OS_WIN) || defined(OS_LINUX))
    settings.multi_threaded_message_loop =
        command_line->HasSwitch(Switches::kMultiThreadedMessageLoop);
#endif

    if (!settings.multi_threaded_message_loop)
    {
        settings.external_message_pump = command_line->HasSwitch(Switches::kExternalMessagePump);
    }

    std::vector<std::string> cookieable_schemes;
    RegisterCookieableSchemes(cookieable_schemes);
    if (!cookieable_schemes.empty())
    {
        std::string list_str;
        for (const auto& scheme : cookieable_schemes)
        {
            if (!list_str.empty())
                list_str += ",";
            list_str += scheme;
        }
        CefString(&settings.cookieable_schemes_list) = list_str;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// CefApp methods.
void ClientAppBrowser::OnBeforeCommandLineProcessing(const CefString& process_type,
                                                     CefRefPtr<CefCommandLine> command_line)
{
    // Pass additional command-line flags to the browser process.
    if (process_type.empty())
    {
        // Pass additional command-line flags when off-screen rendering is enabled.
        if (command_line->HasSwitch(Switches::kOffScreenRenderingEnabled) &&
            !command_line->HasSwitch(Switches::kSharedTextureEnabled))
        {
            // Use software rendering and compositing (disable GPU) for increased FPS
            // and decreased CPU usage. This will also disable WebGL so remove these
            // switches if you need that capability.
            // See https://bitbucket.org/chromiumembedded/cef/issues/1257 for details.
            if (!command_line->HasSwitch(Switches::kEnableGPU))
            {
                command_line->AppendSwitch("disable-gpu");
                command_line->AppendSwitch("disable-gpu-compositing");
            }
        }

        command_line->AppendSwitch("no-sandbox");

        if (command_line->HasSwitch(Switches::kUseViews) &&
            !command_line->HasSwitch("top-chrome-md"))
        {
            // Use non-material mode on all platforms by default. Among other things
            // this causes menu buttons to show hover state. See usage of
            // MaterialDesignController::IsModeMaterial() in Chromium code.
            command_line->AppendSwitchWithValue("top-chrome-md", "non-material");
        }

        if (!command_line->HasSwitch(Switches::kCachePath) &&
            !command_line->HasSwitch("disable-gpu-shader-disk-cache"))
        {
            // Don't create a "GPUCache" directory when cache-path is unspecified.
            command_line->AppendSwitch("disable-gpu-shader-disk-cache");
        }

        // Disable popup blocking for the chrome runtime.
        command_line->AppendSwitch("disable-popup-blocking");
    }

    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it)
    {
        (*it)->OnBeforeCommandLineProcessing(this, command_line);
    }
}

void ClientAppBrowser::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar)
{
    // Default schemes that support cookies.
    cookie_schemes.push_back("http");
    cookie_schemes.push_back("https");
}

//////////////////////////////////////////////////////////////////////////////////////////
// CefBrowserProcessHandler methods.
void ClientAppBrowser::OnRegisterCustomPreferences(cef_preferences_type_t type,
                                                   CefRawPtr<CefPreferenceRegistrar> registrar)
{
    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it)
    {
        (*it)->OnRegisterCustomPreferences(this, type, registrar);
    }
}

void ClientAppBrowser::OnContextInitialized()
{
    CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager(nullptr);
    DCHECK(manager.get());

    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it)
    {
        (*it)->OnContextInitialized(this);
    }
}

void ClientAppBrowser::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    //if (!command_line->HasSwitch(Switches::kEnableGPU))
    //{
    //    command_line->AppendSwitch("disable-gpu");
    //    command_line->AppendSwitch("disable-gpu-compositing");
    //    command_line->AppendSwitch("enable-begin-frame-scheduling");
    //}

    //command_line->AppendSwitch("no-sandbox");

    DelegateSet::iterator it = m_delegates.begin();
    for (; it != m_delegates.end(); ++it)
    {
        (*it)->OnBeforeChildProcessLaunch(this, command_line);
    }
}

void ClientAppBrowser::OnScheduleMessagePumpWork(int64 delay)
{
    // Only used when `--external-message-pump` is passed via the command-line.
    //MainMessageLoopExternalPump* message_pump = MainMessageLoopExternalPump::Get();
    //if (message_pump) message_pump->OnScheduleMessagePumpWork(delay);
}

// static
void ClientAppBrowser::CreateDelegates(DelegateSet& delegates)
{
}
} // namespace Browser