#pragma once

#include "ClientHandler.h"

#include "Duilib/UIlib.h"
using namespace DuiLib;

class BrowserDlg;
class TitleUI : public CHorizontalLayoutUI
{
    DECLARE_DUICONTROL(TitleUI)
public:
    TitleUI();
    ~TitleUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    LPCTSTR GetGroup() const;
    void SetGroup(LPCTSTR pStrGroupName = NULL);

    bool IsSelected() const;
    virtual void Selected(bool bSelected = true);

    bool Activate();
    void DoEvent(TEventUI& event);
    void SetVisible(bool bVisible = true);
    void SetEnabled(bool bEnable = true);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void SetSelectedBkColor(DWORD dwBkColor);
    DWORD GetSelectBkColor();
    void PaintBkColor(HDC hDC);

    UINT GetTextStyle() const;
    void SetTextColor(DWORD dwTextColor);
    DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
    DWORD GetDisabledTextColor() const;
    void SetFont(int index);
    int GetFont() const;
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    void PaintText(HDC hDC);

protected:
    bool m_bSelected;
    UINT m_uButtonState;
    DWORD m_dwSelectedBkColor;
    CDuiString m_sGroupName;

    int m_iFont;
    UINT m_uTextStyle;
    RECT m_rcTextPadding;
    DWORD m_dwTextColor;
    DWORD m_dwDisabledTextColor;
};
