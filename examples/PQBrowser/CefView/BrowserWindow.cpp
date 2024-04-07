#include "stdafx.h"
#include "MainContext.h"
#include "BrowserWindow.h"
#include "include/base/cef_bind.h"

namespace Browser
{
BrowserWindow::BrowserWindow(Delegate* del, const CefString& startup_url)
    : m_handlerDelegate(del)
{
    DCHECK(m_handlerDelegate);
    client_handler_ = new ClientHandler(this, startup_url);
}

void BrowserWindow::CreateBrowser(CefWindowHandle parent_handle,
                                  const CefString& url,
                                  const CefRect& rect,
                                  const CefBrowserSettings& settings,
                                  CefRefPtr<CefRequestContext> request_context)
{
    DCHECK(CefCurrentlyOn(TID_UI));

    CefWindowInfo window_info;
    CefRect wnd_rect = {rect.x, rect.y, rect.x + rect.width, rect.y + rect.height};
    window_info.SetAsChild(parent_handle, wnd_rect);

    CefBrowserHost::CreateBrowser(window_info,
                                  client_handler_,
                                  url.empty() ? client_handler_->StartupUrl() : url.ToString(),
                                  settings,
                                  nullptr,
                                  request_context);
}

void BrowserWindow::GetPopupConfig(CefWindowHandle temp_handle,
                                   CefWindowInfo& windowInfo,
                                   CefRefPtr<CefClient>& client,
                                   CefBrowserSettings& settings)
{
    CEF_REQUIRE_UI_THREAD();

    // The window will be properly sized after the browser is created.
    windowInfo.SetAsChild(temp_handle, CefRect());

    // Don't activate the hidden browser window on creation.
    windowInfo.ex_style |= WS_EX_NOACTIVATE;

    client = client_handler_;
}

void BrowserWindow::ShowPopup(int nBrowserId,
                              CefWindowHandle hParentWnd,
                              int x,
                              int y,
                              size_t width,
                              size_t height)
{
    DCHECK(CefCurrentlyOn(TID_UI));

    auto pBrowser = client_handler_->GetBrowser(nBrowserId);
    if (pBrowser)
    {
        HWND hWnd = pBrowser->GetHost()->GetWindowHandle();
        if (hWnd)
        {
            SetParent(hWnd, hParentWnd);
            SetWindowPos(hWnd,
                         NULL,
                         x,
                         y,
                         static_cast<int>(width),
                         static_cast<int>(height),
                         SWP_NOZORDER | SWP_NOACTIVATE);

            const bool no_activate = ::GetWindowLongPtr(hWnd, GWL_EXSTYLE) & WS_EX_NOACTIVATE;
            ::ShowWindow(hWnd, no_activate ? SW_SHOWNOACTIVATE : SW_SHOW);
        }
    }
}

void BrowserWindow::ShowBrowser(int nBrowserId, int x, int y, size_t width, size_t height)
{
    DCHECK(CefCurrentlyOn(TID_UI));

    //每个CefBrowser都有一个窗口句柄，所以我们可以通过切换标签的时候隐藏没有选中的窗口，显示已经选中的窗口
    auto pBrowser = client_handler_->GetBrowser(nBrowserId);
    auto item = client_handler_->m_BrowserList.begin();
    for (; item != client_handler_->m_BrowserList.end(); item++)
    {
        if (*item)
        {
            HWND hWnd = (*item)->GetHost()->GetWindowHandle();
            if (hWnd)
            {
                if ((*item)->IsSame(pBrowser))
                {
                    SetWindowPos(hWnd,
                                 NULL,
                                 x,
                                 y,
                                 static_cast<int>(width),
                                 static_cast<int>(height),
                                 SWP_NOZORDER);
                    ::ShowWindow(hWnd, SW_SHOW);
                    (*item)->GetHost()->SetFocus(true);
                }
                else
                {
                    ::ShowWindow(hWnd, SW_HIDE);
                }
            }
        }
    }
}

CefWindowHandle BrowserWindow::GetWindowHandle(int nBrowserId) const
{
    DCHECK(CefCurrentlyOn(TID_UI));

    auto pBrowser = client_handler_->GetBrowser(nBrowserId);
    if (pBrowser)
    {
        return pBrowser->GetHost()->GetWindowHandle();
    }
    return nullptr;
}

CefRefPtr<CefBrowser> BrowserWindow::GetBrowser(int nBrowserId) const
{
    DCHECK(CefCurrentlyOn(TID_UI));
    return client_handler_->GetBrowser(nBrowserId);
    ;
}

void BrowserWindow::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
    DCHECK(CefCurrentlyOn(TID_UI));

    m_handlerDelegate->OnBrowserCreated(browser);
}

void BrowserWindow::OnBrowserClosing(CefRefPtr<CefBrowser> browser)
{
    DCHECK(CefCurrentlyOn(TID_UI));
}

void BrowserWindow::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
{
    DCHECK(CefCurrentlyOn(TID_UI));

    m_handlerDelegate->OnBrowserClosed(browser);
}

void BrowserWindow::OnBrowserAllClosed()
{
    DCHECK(CefCurrentlyOn(TID_UI));

    client_handler_->DetachDelegate();
    client_handler_ = nullptr;

    // |this| may be deleted.
    m_handlerDelegate->OnBrowserAllClosed();
}

void BrowserWindow::OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url)
{
    DCHECK(CefCurrentlyOn(TID_UI));
    m_handlerDelegate->OnSetAddress(browser, url);
}

void BrowserWindow::OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    DCHECK(CefCurrentlyOn(TID_UI));
    m_handlerDelegate->OnSetTitle(browser, title);
}

void BrowserWindow::OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
    DCHECK(CefCurrentlyOn(TID_UI));
    m_handlerDelegate->OnSetFullscreen(browser, fullscreen);
}

void BrowserWindow::OnSetLoadingState(CefRefPtr<CefBrowser> browser,
                                      bool isLoading,
                                      bool canGoBack,
                                      bool canGoForward)
{
    DCHECK(CefCurrentlyOn(TID_UI));
    m_handlerDelegate->OnSetLoadingState(browser, isLoading, canGoBack, canGoForward);
}

void BrowserWindow::OnSetDraggableRegions(CefRefPtr<CefBrowser> browser,
                                          const std::vector<CefDraggableRegion>& regions)
{
    DCHECK(CefCurrentlyOn(TID_UI));
    m_handlerDelegate->OnSetDraggableRegions(browser, regions);
}

void BrowserWindow::OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url)
{
    m_handlerDelegate->OnNewTab(browser, url);
}
} // namespace Browser
