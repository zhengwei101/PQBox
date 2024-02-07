#ifndef __UIBUTTON_H__
#define __UIBUTTON_H__

#pragma once

namespace DuiLib {
class UILIB_API CButtonUI : public CLabelUI
{
    DECLARE_DUICONTROL(CButtonUI)

public:
    CButtonUI();

    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    UINT GetControlFlags() const override;

    bool Activate() override;
    void SetEnabled(bool bEnable = true) override;
    void DoEvent(TEventUI& event) override;

    virtual LPCTSTR GetNormalImage();
    virtual void SetNormalImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetHotImage();
    virtual void SetHotImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetPushedImage();
    virtual void SetPushedImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetFocusedImage();
    virtual void SetFocusedImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetDisabledImage();
    virtual void SetDisabledImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetHotForeImage();
    virtual void SetHotForeImage(LPCTSTR pStrImage);
    virtual LPCTSTR GetPushedForeImage();
    virtual void SetPushedForeImage(LPCTSTR pStrImage);
    void SetStateCount(int nCount);
    int GetStateCount() const;
    virtual LPCTSTR GetStateImage();
    virtual void SetStateImage(LPCTSTR pStrImage);

    void BindTabIndex(int _BindTabIndex);
    void BindTabLayoutName(LPCTSTR _TabLayoutName);
    void BindTriggerTabSel(int _SetSelectIndex = -1);
    void RemoveBindTabIndex();
    int GetBindTabLayoutIndex();
    LPCTSTR GetBindTabLayoutName();

    void SetHotFont(int index);
    int GetHotFont() const;
    void SetPushedFont(int index);
    int GetPushedFont() const;
    void SetFocusedFont(int index);
    int GetFocusedFont() const;

    void SetHotBkColor(DWORD dwColor);
    DWORD GetHotBkColor() const;
    void SetPushedBkColor(DWORD dwColor);
    DWORD GetPushedBkColor() const;
    void SetDisabledBkColor(DWORD dwColor);
    DWORD GetDisabledBkColor() const;
    void SetHotTextColor(DWORD dwColor);
    DWORD GetHotTextColor() const;
    void SetPushedTextColor(DWORD dwColor);
    DWORD GetPushedTextColor() const;
    void SetFocusedTextColor(DWORD dwColor);
    DWORD GetFocusedTextColor() const;

    void SetHotBorderColor(DWORD dwColor);
    DWORD GetHotBorderColor() const;
    void SetPushedBorderColor(DWORD dwColor);
    DWORD GetPushedBorderColor() const;
    void SetDisabledBorderColor(DWORD dwColor);
    DWORD GetDisabledBorderColor() const;

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    void PaintText(HDC hDC) override;

    void PaintBkColor(HDC hDC) override;
    void PaintStatusImage(HDC hDC) override;
    void PaintBorder(HDC hDC) override;
    void PaintForeImage(HDC hDC) override;

    void DrawBorder(HDC hDC,
                    const RECT& rcItem,
                    const DWORD& dwBorderColor,
                    const int& nBorderSize,
                    const RECT& rcBorderSize,
                    const SIZE& cxyBorderRound,
                    const int& nBorderStyle);

protected:
    UINT m_uButtonState = 0;

    int m_iHotFont = -1;
    int m_iPushedFont = -1;
    int m_iFocusedFont = -1;
    int m_iBindTabIndex = -1;

    DWORD m_dwHotBkColor = 0;
    DWORD m_dwPushedBkColor = 0;
    DWORD m_dwDisabledBkColor = 0;
    DWORD m_dwHotTextColor = 0;
    DWORD m_dwPushedTextColor = 0;
    DWORD m_dwFocusedTextColor = 0;
    DWORD m_dwHotBorderColor = 0;
    DWORD m_dwPushedBorderColor = 0;
    DWORD m_dwDisabledBorderColor = 0;
    
    int m_nStateCount = 0;

    CDuiString m_sNormalImage;
    CDuiString m_sHotImage;
    CDuiString m_sHotForeImage;
    CDuiString m_sPushedImage;
    CDuiString m_sPushedForeImage;
    CDuiString m_sFocusedImage;
    CDuiString m_sDisabledImage;
    CDuiString m_sStateImage;
    CDuiString m_sBindTabLayoutName;
};

} // namespace DuiLib

#endif // __UIBUTTON_H__