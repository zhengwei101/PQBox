#include "stdafx.h"
#include "BrowserDlgManager.h"
#include <sstream>
#include "include/base/cef_bind.h"
#include "include/base/cef_logging.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "include/cef_request_context_handler.h"

#include "MainContext.h"
#include "ClientSwitches.h"

namespace Browser
{
namespace
{
class ClientRequestContextHandler : public CefRequestContextHandler
{
public:
    ClientRequestContextHandler() {}

private:
    IMPLEMENT_REFCOUNTING(ClientRequestContextHandler);
};
} // namespace

BrowserDlgManager::BrowserDlgManager(bool terminate_when_all_windows_closed)
    : terminate_when_all_windows_closed_(terminate_when_all_windows_closed)
{
    auto command_line = CefCommandLine::GetGlobalCommandLine();
    DCHECK(command_line.get());
    request_context_per_browser_ = command_line->HasSwitch(Switches::kRequestContextPerBrowser);
    request_context_shared_cache_ = command_line->HasSwitch(Switches::kRequestContextSharedCache);
}

BrowserDlgManager::~BrowserDlgManager()
{
    // All root windows should already have been destroyed.
    DCHECK(m_BrowserDlgSet.empty());
}

CefRefPtr<BrowserDlg> BrowserDlgManager::CreateBrowserDlg(HWND hParent,
                                                          bool with_controls,
                                                          const CefRect& bounds,
                                                          const CefString& url)
{
    CefBrowserSettings settings;
    MainContext::Inst()->PopulateBrowserSettings(&settings);
    CefRefPtr<BrowserDlg> pDlg = new BrowserDlg();
    if (pDlg)
    {
        pDlg->Init(this,
                   hParent,
                   with_controls,
                   bounds,
                   settings,
                   url.empty() ? MainContext::Inst()->GetMainURL() : url.ToWString());

        // Store a reference to the root window on the main thread.
        OnBrowserDlgCreated(pDlg);
    }

    return pDlg;
}

CefRefPtr<BrowserDlg> BrowserDlgManager::CreateBrowserDlgAsPopup(
    bool with_controls,
    const CefPopupFeatures& popupFeatures,
    CefWindowInfo& windowInfo,
    CefRefPtr<CefClient>& client,
    CefBrowserSettings& settings)
{
    MainContext::Inst()->PopulateBrowserSettings(&settings);
    CefRefPtr<BrowserDlg> pDlg = new BrowserDlg();
    if (pDlg)
    {
        pDlg->InitAsPopup(this, with_controls, popupFeatures, windowInfo, client, settings);

        // Store a reference to the root window on the main thread.
        OnBrowserDlgCreated(pDlg);
    }

    return pDlg;
}

CefRefPtr<BrowserDlg> BrowserDlgManager::GetWindowForBrowser(int browser_id)
{
    DCHECK(CefCurrentlyOn(TID_UI));

    auto it = m_BrowserDlgSet.begin();
    for (; it != m_BrowserDlgSet.end(); ++it)
    {
        auto browser = (*it)->GetBrowser();
        if (browser && browser->GetIdentifier() == browser_id)
            return *it;
    }
    return nullptr;
}

void BrowserDlgManager::CloseAllWindows(bool force)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI,
                    CefCreateClosureTask(base::BindOnce(
                        &BrowserDlgManager::CloseAllWindows, base::Unretained(this), force)));
        return;
    }

    if (m_BrowserDlgSet.empty())
        return;

    auto it = m_BrowserDlgSet.begin();
    for (; it != m_BrowserDlgSet.end(); ++it)
    {
        (*it)->Close(force);
    }
}

void BrowserDlgManager::OnBrowserDlgCreated(CefRefPtr<BrowserDlg> pDlg)
{
    if (!CefCurrentlyOn(TID_UI))
    {
        // Execute this method on the main thread.
        CefPostTask(TID_UI,
                    CefCreateClosureTask(base::BindOnce(
                        &BrowserDlgManager::OnBrowserDlgCreated, base::Unretained(this), pDlg)));
        return;
    }

    m_BrowserDlgSet.insert(pDlg);
}

CefRefPtr<CefRequestContext> BrowserDlgManager::GetRequestContext()
{
    DCHECK(CefCurrentlyOn(TID_UI));

    if (request_context_per_browser_)
    {
        // Create a new request context for each browser.
        CefRequestContextSettings settings;

        auto command_line = CefCommandLine::GetGlobalCommandLine();
        if (command_line->HasSwitch(Switches::kCachePath))
        {
            if (request_context_shared_cache_)
            {
                // Give each browser the same cache path. The resulting context objects
                // will share the same storage internally.
                CefString(&settings.cache_path) =
                    command_line->GetSwitchValue(Switches::kCachePath);
            }
            else
            {
                // Give each browser a unique cache path. This will create completely
                // isolated context objects.
                std::stringstream ss;
                ss << command_line->GetSwitchValue(Switches::kCachePath).ToString() << time(NULL);
                CefString(&settings.cache_path) = ss.str();
            }
        }

        return CefRequestContext::CreateContext(settings, new ClientRequestContextHandler);
    }

    // All browsers will share the global request context.
    if (!m_SharedRequestContext.get())
    {
        m_SharedRequestContext = CefRequestContext::CreateContext(
            CefRequestContext::GetGlobalContext(), new ClientRequestContextHandler);
    }
    return m_SharedRequestContext;
}

void BrowserDlgManager::OnBrowserDlgDestroyed(BrowserDlg* pDlg)
{
    DCHECK(CefCurrentlyOn(TID_UI));

    auto it = m_BrowserDlgSet.find(pDlg);
    if (it != m_BrowserDlgSet.end())
        m_BrowserDlgSet.erase(it);

    if (terminate_when_all_windows_closed_ && m_BrowserDlgSet.empty())
    {
        // Quit the main message loop after all windows have closed.
        CefQuitMessageLoop();
        //PostQuitMessage(0);
    }
}

} // namespace Browser