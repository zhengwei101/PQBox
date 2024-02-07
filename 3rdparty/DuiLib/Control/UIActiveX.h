#ifndef __UIACTIVEX_H__
#define __UIACTIVEX_H__

#pragma once

struct IOleObject;

namespace DuiLib {

class CActiveXCtrl;

template<class T>
class CSafeRelease
{
public:
    CSafeRelease(T* p)
        : m_p(p){};

    ~CSafeRelease()
    {
        if (m_p)
            m_p->Release();
    };

    T* Detach()
    {
        T* t = m_p;
        m_p = nullptr;
        return t;
    };

    T* m_p = nullptr;
};

class UILIB_API CActiveXUI : public CControlUI, public IMessageFilterUI
{
    DECLARE_DUICONTROL(CActiveXUI)

    friend class CActiveXCtrl;

public:
    CActiveXUI();
    virtual ~CActiveXUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    HWND GetHostWindow() const;

    virtual bool IsDelayCreate() const;
    virtual void SetDelayCreate(bool bDelayCreate = true);
    virtual bool IsMFC() const;
    virtual void SetMFC(bool bMFC = false);

    bool CreateControl(const CLSID clsid);
    bool CreateControl(LPCTSTR pstrCLSID);
    HRESULT GetControl(const IID iid, LPVOID* ppRet);
    CLSID GetClisd() const;
    CDuiString GetModuleName() const;
    void SetModuleName(LPCTSTR pstrText);

    void SetVisible(bool bVisible = true);
    void SetInternVisible(bool bVisible = true);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void Move(SIZE szOffset, bool bNeedInvalidate = true);
    bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    LRESULT
    MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

protected:
    virtual void ReleaseControl();
    virtual bool DoCreateControl();

protected:
    CLSID m_clsid;
    CDuiString m_sModuleName;
    bool m_bCreated = false;
    bool m_bDelayCreate = true;
    bool m_bMFC = false;
    IOleObject* m_pUnk = nullptr;
    CActiveXCtrl* m_pControl = nullptr;
    HWND m_hwndHost = NULL;
};

} // namespace DuiLib

#endif // __UIACTIVEX_H__
