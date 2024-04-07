#pragma once

#include "BrowserUI.h"
#include "ClientApp.h"
#include "BrowserWindow.h"

namespace Browser
{
class BrowserDlg
    : public DuiLib::WindowImplBase
    , public Browser::BrowserWindow::Delegate
    , public base::RefCountedThreadSafe<BrowserDlg>
{
    friend class base::RefCountedThreadSafe<BrowserDlg>;

public:
    class Delegate
    {
    public:
        virtual CefRefPtr<CefRequestContext> GetRequestContext() = 0;
        virtual void OnBrowserDlgDestroyed(BrowserDlg* pDlg) = 0;

    protected:
        virtual ~Delegate() {}
    };
    BrowserDlg();
    ~BrowserDlg();

public:
    LPCTSTR GetWindowClassName() const;
    virtual void InitWindow();
    virtual void OnFinalMessage(HWND hWnd);
    virtual DuiLib::CDuiString GetSkinFile();
    virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
    virtual DuiLib::CControlUI* CreateControl(LPCTSTR pstrClass);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void Notify(DuiLib::TNotifyUI& msg);

private:
    // BrowserWindow::Delegate methods.
    void OnBrowserCreated(CefRefPtr<CefBrowser> browser) override;
    void OnBrowserClosed(CefRefPtr<CefBrowser> browser) override;
    void OnBrowserAllClosed() override;
    void OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url) override;
    void OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title) override;
    void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) override;
    void OnSetLoadingState(CefRefPtr<CefBrowser> browser,
                           bool isLoading,
                           bool canGoBack,
                           bool canGoForward) override;
    void OnSetDraggableRegions(CefRefPtr<CefBrowser> browser,
                               const std::vector<CefDraggableRegion>& regions) override;
    void OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url) override;

    void NotifyDestroyedIfDone();

private:
    DuiLib::CHorizontalLayoutUI* m_uiTabs = nullptr;
    DuiLib::CLabelUI* m_pTitle = nullptr;
    DuiLib::CButtonUI* m_tabNew = nullptr;
    DuiLib::CControlUI* m_uiToolbar = nullptr;
    DuiLib::CButtonUI* m_btnBackward = nullptr;
    DuiLib::CButtonUI* m_btnForward = nullptr;
    DuiLib::CEditUI* m_editUrl = nullptr;
    DuiLib::CEditUI* m_editKeyword = nullptr;
    DuiLib::CDuiString m_sPopup;

public:
    // BrowserDlg methods.
    void Init(BrowserDlg::Delegate* del,
              HWND hParent,
              bool with_controls,
              const CefRect& bounds,
              const CefBrowserSettings& settings,
              const CefString& url);

    void InitAsPopup(BrowserDlg::Delegate* del,
                     bool with_controls,
                     const CefPopupFeatures& popupFeatures,
                     CefWindowInfo& windowInfo,
                     CefRefPtr<CefClient>& client,
                     CefBrowserSettings& settings);

    void NewTab(const CefString& url);

    CefRefPtr<CefBrowser> GetBrowser();
    CefWindowHandle GetWindowHandle();
    void LoadURL(const CefString& url);

private:
    //void CreateBrowserWindow(const CefString& startup_url);
    void CreateBrowserDlg(const CefBrowserSettings& settings);

    HWND m_hParent = NULL;
    bool m_bWithControls = false;
    bool m_bIsPopup = false;
    RECT m_rcStart = {0};
    bool m_bInitialized = false;
    bool m_bWindowDestroyed = false;
    bool m_bBrowserDestroyed = false;
    int m_nCurBrowserId = 0;

    BrowserDlg::Delegate* m_pDlgDelegate = nullptr;
    Browser::CBrowserUI* m_pBrowserUI = nullptr;
    std::unique_ptr<Browser::BrowserWindow> m_pBrowserHandler;
};
}