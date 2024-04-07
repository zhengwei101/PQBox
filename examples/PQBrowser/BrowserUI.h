#pragma once

#include "ClientHandler.h"

#include "DuiLib/UIlib.h"
using namespace DuiLib;

class BrowserDlg;

namespace Browser {
class BrowserWindow;
class CBrowserUI : public DuiLib::CControlUI
{
    DECLARE_DUICONTROL(CBrowserUI)
public:
    CBrowserUI(HWND hParentWnd);

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void SetCtrl(BrowserWindow* pCtrl);
    void CreateBrowser(const CefString& url, CefRefPtr<CefRequestContext> request_context);
    void ShowBrowser(int nBrowserId = 0);
    void CloseBrowser(int nBrowserId = 0);

protected:
    HWND m_hParentWnd;
    BrowserWindow* m_pCEFHandler = nullptr;
    int m_nCurBrowserId = 0;

    DISALLOW_COPY_AND_ASSIGN(CBrowserUI);
};
} // namespace Browser
