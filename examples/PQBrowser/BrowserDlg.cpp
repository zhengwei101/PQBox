#include "stdafx.h"
#include "BrowserDlg.h"
#include "BrowserDlgManager.h"
#include "MainContext.h"
#include "TitleUI.h"
#include "resource.h"

namespace Browser
{

namespace
{
const int FRAME_LEFT = 2;
const int FRAME_TOP = 1;
const int FRAME_RIGHT = 2;
const int FRAME_BOTTOM = 2;
const int TITLE_HEIGHT = 32;
const int TOOLBAR_HEIGHT = 28;
} // namespace

BrowserDlg::BrowserDlg()
{
}

BrowserDlg::~BrowserDlg()
{
    DCHECK(m_bWindowDestroyed);
    DCHECK(m_bBrowserDestroyed);
    //PostQuitMessage(0);
}

LPCTSTR BrowserDlg::GetWindowClassName() const
{
    return _T("BrowserDlg");
}

void BrowserDlg::InitWindow()
{
    SetIcon(IDI_BROWSER);
    m_uiTabs = static_cast<DuiLib::CHorizontalLayoutUI*>(m_paintMgr.FindControl(_T("uiTabs")));
    m_tabNew = static_cast<DuiLib::CButtonUI*>(m_paintMgr.FindControl(_T("tabNew")));
    m_uiToolbar = static_cast<DuiLib::CControlUI*>(m_paintMgr.FindControl(_T("uiToolbar")));
    m_btnBackward = static_cast<DuiLib::CButtonUI*>(m_paintMgr.FindControl(_T("btnBackward")));
    m_btnForward = static_cast<DuiLib::CButtonUI*>(m_paintMgr.FindControl(_T("btnForward")));
    m_editUrl = static_cast<DuiLib::CEditUI*>(m_paintMgr.FindControl(_T("editUrl")));
    m_editKeyword = static_cast<DuiLib::CEditUI*>(m_paintMgr.FindControl(_T("editKeyword")));

    m_pBrowserUI = static_cast<Browser::CBrowserUI*>(m_paintMgr.FindControl(_T("webBrowser")));
    if (m_pBrowserUI)
    {
        m_pBrowserUI->SetCtrl(m_pBrowserHandler.get());
    }

    if (m_uiTabs == NULL || m_tabNew == NULL || m_uiToolbar == NULL || m_editUrl == NULL ||
        m_editKeyword == NULL)
    {
        MessageBox(NULL, _T("加载资源文件失败"), _T("Browser"), MB_OK | MB_ICONERROR);
        return;
    }

    if (m_bWithControls)
    {
        m_uiToolbar->SetVisible(true);
        if (m_btnBackward)
            m_btnBackward->SetEnabled(false);
        if (m_btnForward)
            m_btnForward->SetEnabled(false);
    }
    else
    {
        m_uiToolbar->SetVisible(false);
    }
    if (m_bIsPopup)
    {
        m_tabNew->SetVisible(false);
        m_uiToolbar->SetVisible(false);
    }
    else
    {
        m_tabNew->SetVisible(true);
    }
}

void BrowserDlg::OnFinalMessage(HWND hWnd)
{
    WindowImplBase::OnFinalMessage(hWnd);
    //delete this;
    m_bWindowDestroyed = true;
    NotifyDestroyedIfDone();
}

DuiLib::CDuiString BrowserDlg::GetSkinFile()
{
    return _T("BrowserDlg.xml");
}

LRESULT BrowserDlg::ResponseDefaultKeyEvent(WPARAM wParam)
{
    if (wParam == VK_RETURN)
    {
        return FALSE;
    }
    else if (wParam == VK_ESCAPE)
    {
        return TRUE;
    }
    return FALSE;
}

DuiLib::CControlUI* BrowserDlg::CreateControl(LPCTSTR pstrClass)
{
    if (_tcsicmp(pstrClass, _T("WebBrowser")) == 0)
    {
        return new Browser::CBrowserUI(m_hWnd);
    }
    else if (_tcsicmp(pstrClass, _T("Title")) == 0)
    {
        return new TitleUI();
    }
    return nullptr;
}

LRESULT BrowserDlg::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            if (wParam == 0L)
                return 0L;
            break;
        default: break;
    }
    return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

void BrowserDlg::Notify(DuiLib::TNotifyUI& msg)
{
    DuiLib::CDuiString sCtrlName = msg.pSender->GetName();

    if (_tcsicmp(msg.sType, _T("click")) == 0)
    {
        if (_tcsicmp(sCtrlName, _T("btnGoto")) == 0)
        {
            //跳转
            DuiLib::CDuiString sUrl = m_editUrl->GetText();
            m_editUrl->SetText(sUrl);
            LoadURL(sUrl.GetData());
        }
        else if (_tcsicmp(sCtrlName, _T("btnSearch")) == 0)
        {
            //搜索
            DuiLib::CDuiString sUrl, sKeyword = m_editKeyword->GetText();
            sUrl.Format(_T("https://www.baidu.com/s?wd=%s"), sKeyword.GetData());
            m_editUrl->SetText(sUrl);
            LoadURL(sUrl.GetData());
        }
        else if (_tcsicmp(sCtrlName, _T("btnHome")) == 0)
        {
            //主页
            m_editUrl->SetText(MainContext::Inst()->GetMainURL().c_str());
            LoadURL(MainContext::Inst()->GetMainURL().c_str());
        }
        else if (_tcsicmp(sCtrlName, _T("btnSettings")) == 0)
        {
            DuiLib::CDuiString sUrl = _T("about:settings");
            m_editUrl->SetText(sUrl);
            LoadURL(sUrl.GetData());
        }
        else if (_tcsicmp(sCtrlName, _T("btnBackward")) == 0)
        {
            CefRefPtr<CefBrowser> browser = GetBrowser();
            if (browser)
            {
                browser->GoBack();
            }
        }
        else if (_tcsicmp(sCtrlName, _T("btnForward")) == 0)
        {
            CefRefPtr<CefBrowser> browser = GetBrowser();
            if (browser)
            {
                browser->GoForward();
            }
        }
        else if (_tcsicmp(sCtrlName, _T("tabNew")) == 0)
        {
            NewTab(L"about:blank");
        }
        else if (_tcsnicmp(sCtrlName, _T("tabClose"), 8) == 0)
        {
            CDuiString sBuffer = sCtrlName;
            sBuffer.Replace(_T("tabClose"), _T(""));
            int nBrowserId = _ttoi(sBuffer.GetData());
            sBuffer.Format(_T("tabTitle%d"), nBrowserId);
            int nTabsCount = m_uiTabs->GetCount();
            for (int idx = 0; idx < nTabsCount; idx++)
            {
                TitleUI* pItem = (TitleUI*)m_uiTabs->GetItemAt(idx);
                if (pItem)
                {
                    if (nTabsCount <= 2)
                    {
                        LoadURL(L"about:blank");
                    }
                    else
                    {
                        if (pItem->GetTag() == nBrowserId)
                        {
                            int index = (idx > 0) ? (idx - 1) : (idx + 1);
                            auto pNextItem = (TitleUI*)m_uiTabs->GetItemAt(index);
                            m_uiTabs->Remove(pItem);
                            pNextItem->Selected(true);

                            if (m_pBrowserUI)
                            {
                                m_pBrowserUI->CloseBrowser(nBrowserId);
                            }
                        }
                    }
                }
            }
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_RETURN) == 0)
    {
        if (_tcsicmp(sCtrlName, _T("editUrl")) == 0)
        {
            DuiLib::CDuiString sUrl = m_editUrl->GetText();
            m_editUrl->SetText(sUrl);
            LoadURL(sUrl.GetData());
        }
        else if (_tcsicmp(sCtrlName, _T("editKeyword")) == 0)
        {
            DuiLib::CDuiString sUrl, sKeyword = m_editKeyword->GetText();
            sUrl.Format(_T("https://www.baidu.com/s?wd=%s"), sKeyword.GetData());
            m_editUrl->SetText(sUrl);
            LoadURL(sUrl.GetData());
        }
    }
    else if (_tcsicmp(msg.sType, DUI_MSGTYPE_SELECTCHANGED) == 0)
    {
        if (_tcsnicmp(sCtrlName, _T("tabTitle"), 8) == 0)
        {
            CDuiString sBuffer = msg.pSender->GetText();
            SetWindowText(m_hWnd, sBuffer.GetData());
            sBuffer = msg.pSender->GetUserData();
            m_editUrl->SetText(sBuffer.GetData());
            if (m_pBrowserUI)
            {
                m_nCurBrowserId = static_cast<int>(msg.pSender->GetTag());
                m_pBrowserUI->ShowBrowser(m_nCurBrowserId);
            }
        }
    }
    return WindowImplBase::Notify(msg);
}

void BrowserDlg::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
    DCHECK(CefCurrentlyOn(TID_UI));

    int nBrowserId = browser->GetIdentifier();
    if (m_nCurBrowserId != nBrowserId)
    {
        m_nCurBrowserId = nBrowserId;
    }

    if (m_bIsPopup)
    {
        // For popup browsers create the root window once the browser has been created.
        CreateBrowserDlg(CefBrowserSettings());
    }
    else
    {
        if (m_pBrowserUI)
        {
            RECT rcPos = m_pBrowserUI->GetPos();
            if (m_pBrowserHandler)
            {
                m_pBrowserHandler->ShowBrowser(m_nCurBrowserId,
                                               rcPos.left,
                                               rcPos.top,
                                               rcPos.right - rcPos.left,
                                               rcPos.bottom - rcPos.top);
            }
        }
    }
}

void BrowserDlg::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
{
    DCHECK(CefCurrentlyOn(TID_UI));
}

void BrowserDlg::OnBrowserAllClosed()
{
    DCHECK(CefCurrentlyOn(TID_UI));

    m_pBrowserHandler.reset();

    if (!m_bWindowDestroyed)
    {
        // The browser was destroyed first. This could be due to the use of
        // off-screen rendering or execution of JavaScript window.close().
        // Close the RootWindow.
        Close(true);
    }

    m_bBrowserDestroyed = true;
    NotifyDestroyedIfDone();
}

void BrowserDlg::OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url)
{
    DCHECK(CefCurrentlyOn(TID_UI));
    CDuiString sBuffer;
    CDuiString sUrl;
    bool bAddTab = true;
    int nTabsCount = m_uiTabs->GetCount();
    int nBrowserId = browser->GetIdentifier();
    if (url.length() > 0)
        sUrl = url.c_str();
    for (int idx = 0; idx < nTabsCount; idx++)
    {
        DuiLib::CControlUI* pTitle = (DuiLib::CControlUI*)m_uiTabs->GetItemAt(idx);
        if (pTitle == NULL || _tcsicmp(pTitle->GetClass(), _T("ButtonUI")) == 0)
            continue;
        if (pTitle->GetTag() == nBrowserId)
        {
            bAddTab = false;
            pTitle->SetUserData(sUrl);
            sBuffer = pTitle->GetText();
            if (sBuffer.GetLength() == 0)
                pTitle->SetText(sUrl);
            break;
        }
    }
    if (nBrowserId == m_nCurBrowserId)
    {
        m_editUrl->SetText(sUrl);
    }
    if (bAddTab)
    {
        if (m_bIsPopup)
        {
            if (m_pTitle == NULL)
            {
                m_pTitle = new CLabelUI;
            }
            m_pTitle->SetText(sUrl);
            m_uiTabs->AddAt(m_pTitle, nTabsCount - 1);
            sBuffer.Format(
                _T("name=\"labTitle\" height=\"30\" floatalign=\"right\" textpadding=\"5,1,20,2\" ")
                _T("textcolor=\"FFFFFFFF\""));
            m_pTitle->ApplyAttributeList(sBuffer);
        }
        else
        {
            TitleUI* pTitle = new TitleUI;
            pTitle->SetTag(nBrowserId);
            pTitle->SetUserData(sUrl);
            m_uiTabs->AddAt(pTitle, nTabsCount - 1);

            m_nCurBrowserId = nBrowserId;
            sBuffer.Format(
                _T("name=\"tabTitle%d\" height=\"30\" minwidth=\"100\" maxwidth=\"256\" ")
                _T("floatalign=\"right\" ")
                _T("borderround=\"2,2\" textpadding=\"5,1,20,2\" bkcolor=\"FF1587D8\" ")
                _T("selectedbkcolor=\"FF3498DB\" ")
                _T("textcolor=\"FFFFFFFF\" selectedtextcolor=\"FFFFFFFF\" group=\"Titles\""),
                nBrowserId);
            pTitle->ApplyAttributeList(sBuffer);

            CControlUI* pControl = new CControlUI;
            CButtonUI* pClose = new CButtonUI;
            pTitle->Add(pControl);
            pTitle->Add(pClose);
            sBuffer.Format(
                _T("name=\"tabClose%d\" width=\"26\" height=\"28\" floatalign=\"right\" ")
                _T("normalimage=\"file='btnTabClose.png' source='0,0,11,11' dest='8,8,19,19'\" ")
                _T("hotimage=\"file='btnTabClose.png' source='11,0,22,11' dest='8,8,19,19'\" ")
                _T("pushedimage=\"file='btnTabClose.png' source='22,0,33,11' dest='8,8,19,19'\""),
                nBrowserId);
            pClose->ApplyAttributeList(sBuffer);

            pTitle->Selected(true);
            m_editUrl->SetText(sUrl);
        }
    }
}

void BrowserDlg::OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    DCHECK(CefCurrentlyOn(TID_UI));
    CDuiString sTitle;
    if (title.length() > 0)
        sTitle = title.c_str();

    if (m_bIsPopup)
    {
        if (m_pTitle == NULL)
        {
            m_pTitle = new CLabelUI;
        }
        m_pTitle->SetText(sTitle);
        SetWindowText(m_hWnd, sTitle);
    }
    else
    {
        int nTabsCount = m_uiTabs->GetCount();
        int nBrowserId = browser->GetIdentifier();
        for (int idx = 0; idx < nTabsCount; idx++)
        {
            DuiLib::CControlUI* pItem = (DuiLib::CControlUI*)m_uiTabs->GetItemAt(idx);
            if (pItem == NULL || _tcsicmp(pItem->GetClass(), _T("ButtonUI")) == 0)
                continue;
            if (pItem->GetTag() == nBrowserId)
            {
                pItem->SetText(sTitle);
            }
        }
        if (nBrowserId == m_nCurBrowserId)
        {
            SetWindowText(m_hWnd, sTitle);
        }
    }
}

void BrowserDlg::OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen)
{
    DCHECK(CefCurrentlyOn(TID_UI));
}

void BrowserDlg::OnSetLoadingState(CefRefPtr<CefBrowser> browser,
                                   bool isLoading,
                                   bool canGoBack,
                                   bool canGoForward)
{
    DCHECK(CefCurrentlyOn(TID_UI));
    if (m_btnBackward)
        m_btnBackward->SetEnabled(canGoBack);
    if (m_btnForward)
        m_btnForward->SetEnabled(canGoForward);
}

void BrowserDlg::OnSetDraggableRegions(CefRefPtr<CefBrowser> browser,
                                       const std::vector<CefDraggableRegion>& regions)
{
    DCHECK(CefCurrentlyOn(TID_UI));
}

void BrowserDlg::OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url)
{
    DCHECK(CefCurrentlyOn(TID_UI));
    NewTab(url);
}

void BrowserDlg::NotifyDestroyedIfDone()
{
    // Notify once both the window and the browser have been destroyed.
    if (m_bWindowDestroyed && m_bBrowserDestroyed)
    {
        m_pDlgDelegate->OnBrowserDlgDestroyed(this);
    }
}

void BrowserDlg::Init(BrowserDlg::Delegate* del,
                      HWND hParent,
                      bool with_controls,
                      const CefRect& bounds,
                      const CefBrowserSettings& settings,
                      const CefString& url)
{
    DCHECK(del);
    DCHECK(!m_bInitialized);

    m_pDlgDelegate = del;
    m_hParent = hParent;
    m_bWithControls = true;

    m_rcStart.left = bounds.x;
    m_rcStart.top = bounds.y;
    m_rcStart.right = bounds.x + bounds.width;
    m_rcStart.bottom = bounds.y + bounds.height;

    m_pBrowserHandler.reset(new Browser::BrowserWindow(this, url));

    m_bInitialized = true;

    // Create the native root window on the main thread.
    if (CefCurrentlyOn(TID_UI))
    {
        CreateBrowserDlg(settings);
    }
    else
    {
        CefPostTask(
            TID_UI,
            CefCreateClosureTask(base::BindOnce(&BrowserDlg::CreateBrowserDlg, this, settings)));
    }
}

void BrowserDlg::InitAsPopup(BrowserDlg::Delegate* del,
                             bool with_controls,
                             const CefPopupFeatures& popupFeatures,
                             CefWindowInfo& windowInfo,
                             CefRefPtr<CefClient>& client,
                             CefBrowserSettings& settings)
{
    DCHECK(del);
    DCHECK(!m_bInitialized);

    m_pDlgDelegate = del;
    m_bWithControls = with_controls;
    m_bIsPopup = true;

    if (popupFeatures.xSet)
        m_rcStart.left = popupFeatures.x;
    if (popupFeatures.ySet)
        m_rcStart.top = popupFeatures.y;
    if (popupFeatures.widthSet)
        m_rcStart.right = m_rcStart.left + popupFeatures.width;
    if (popupFeatures.heightSet)
        m_rcStart.bottom = m_rcStart.top + popupFeatures.height;

    m_pBrowserHandler.reset(new Browser::BrowserWindow(this, ""));

    m_bInitialized = true;

    // The new popup is initially parented to a temporary window. The native root
    // window will be created after the browser is created and the popup window
    // will be re-parented to it at that time.
    m_pBrowserHandler->GetPopupConfig(TempWindow::GetWindowHandle(), windowInfo, client, settings);
}

void BrowserDlg::NewTab(const CefString& url)
{
    if (m_pBrowserHandler && m_pBrowserUI && url.length() > 0)
    {
        RECT rcPos = m_pBrowserUI->GetPos();
        CefRect cef_rect(rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top);
        CefBrowserSettings settings;
        MainContext::Inst()->PopulateBrowserSettings(&settings);
        m_pBrowserHandler->CreateBrowser(
            m_hWnd, url, cef_rect, settings, m_pDlgDelegate->GetRequestContext());
    }
}

CefRefPtr<CefBrowser> BrowserDlg::GetBrowser()
{
    DCHECK(CefCurrentlyOn(TID_UI));

    if (m_pBrowserHandler)
    {
        return m_pBrowserHandler->GetBrowser(m_nCurBrowserId);
    }
    return nullptr;
}

CefWindowHandle BrowserDlg::GetWindowHandle()
{
    return m_hWnd;
}

void BrowserDlg::LoadURL(const CefString& url)
{
    if (m_pBrowserHandler)
    {
        auto pBrowser = m_pBrowserHandler->GetBrowser(m_nCurBrowserId);
        if (pBrowser)
        {
            auto pFrame = pBrowser->GetMainFrame();
            if (pFrame)
            {
                pFrame->LoadURL(url);
            }
        }
    }
}

//void BrowserDlg::CreateBrowserWindow(const CefString& startup_url)
//{
//    m_pBrowserHandler.reset(new Browser::BrowserWindow(this, startup_url));
//}

void BrowserDlg::CreateBrowserDlg(const CefBrowserSettings& settings)
{
    DCHECK(CefCurrentlyOn(TID_UI));

    int x, y, width, height;
    if (::IsRectEmpty(&m_rcStart))
    {
        // Use the default window position/size.
        x = y = width = height = CW_USEDEFAULT;
    }
    else
    {
        RECT rcWindow = m_rcStart;
        if (m_bWithControls)
        {
            rcWindow.bottom += (FRAME_TOP + TITLE_HEIGHT + FRAME_BOTTOM);
            rcWindow.right += (FRAME_LEFT + FRAME_RIGHT);
            if (!m_bIsPopup)
            {
                rcWindow.bottom += TOOLBAR_HEIGHT;
            }
        }
        x = rcWindow.left;
        y = rcWindow.top;
        width = rcWindow.right - rcWindow.left;
        height = rcWindow.bottom - rcWindow.top;
    }

    // Create the main window.
    Create(m_hParent,
           _T("Browser"),
           UI_WNDSTYLE_FRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
           WS_EX_APPWINDOW,
           x,
           y,
           width,
           height,
           NULL);

    ::SetMenu(m_hWnd, NULL);

    if (m_pBrowserUI)
    {
        RECT rect = m_pBrowserUI->GetPos();

        if (m_bIsPopup)
        {
            // With popups we already have a browser window. Parent the browser window
            // to the root window and show it in the correct location.
            m_pBrowserHandler->ShowPopup(m_nCurBrowserId,
                                         m_hWnd,
                                         rect.left,
                                         rect.top,
                                         static_cast<size_t>(rect.right) - rect.left,
                                         static_cast<size_t>(rect.bottom) - rect.top);
        }
        else
        {
            // Create the browser window.
            CefRect cef_rect(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
            m_pBrowserHandler->CreateBrowser(
                m_hWnd, std::wstring(), cef_rect, settings, m_pDlgDelegate->GetRequestContext());
        }
    }
}

} // namespace Browser