#ifndef WIN_IMPL_BASE_HPP
#define WIN_IMPL_BASE_HPP

#pragma once

namespace DuiLib
{
class UILIB_API WindowImplBase
    : public CWindowWnd
    , public CNotifyPump
    , public INotifyUI
    , public IMessageFilterUI
    , public IDialogBuilderCallback
    , public IQueryControlText
{
public:
    WindowImplBase(){};
    virtual ~WindowImplBase(){};

    // 只需主窗口重写（初始化资源与多语言接口）
    virtual void InitResource(){};

    // 每个窗口都可以重写
    virtual void Init(){};
    virtual void InitWindow(){};

    void OnFinalMessage(HWND hWnd) override;
    void Notify(TNotifyUI& msg) override;

    DUI_DECLARE_MESSAGE_MAP()
    virtual void OnClick(TNotifyUI& msg);
    virtual void OnWindowInit(TNotifyUI& msg);
    virtual BOOL IsInStaticControl(CControlUI* pControl);

protected:
    virtual CDuiString GetSkinType();
    virtual CDuiString GetSkinFile() = 0;
    LPCTSTR GetWindowClassName() const override = 0;
    virtual LPCTSTR GetManagerName();
    virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam);
    CPaintManagerUI m_paintMgr;

public:
    UINT GetClassStyle() const override;
    CControlUI* CreateControl(LPCTSTR pstrClass) override;
    LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) override;

    LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled) override;

    virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

#if defined(WIN32) && !defined(UNDER_CE)
    virtual LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
#endif

    virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnChar(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    virtual LONG GetStyle();
};
} // namespace DuiLib

#endif // WIN_IMPL_BASE_HPP
