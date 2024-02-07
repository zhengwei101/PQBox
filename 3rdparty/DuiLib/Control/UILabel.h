#ifndef __UILABEL_H__
#define __UILABEL_H__

#pragma once

namespace DuiLib {
class UILIB_API CLabelUI : public CControlUI
{
    DECLARE_DUICONTROL(CLabelUI)
public:
    CLabelUI();
    ~CLabelUI();

    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    UINT GetControlFlags() const override;

    void SetTextStyle(UINT uStyle);
    UINT GetTextStyle() const;
    void SetTextColor(DWORD dwTextColor);
    DWORD GetTextColor() const;
    void SetDisabledTextColor(DWORD dwTextColor);
    DWORD GetDisabledTextColor() const;
    void SetFont(int index);
    int GetFont() const;
    RECT GetTextPadding() const;
    void SetTextPadding(RECT rc);
    bool IsShowHtml();
    void SetShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable) override;
    void DoEvent(TEventUI& event) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;
    void PaintText(HDC hDC) override;

    virtual bool GetAutoCalcWidth() const;
    virtual void SetAutoCalcWidth(bool bAutoCalcWidth);
    virtual bool GetAutoCalcHeight() const;
    virtual void SetAutoCalcHeight(bool bAutoCalcHeight);

    void SetText(LPCTSTR pstrText) override;

protected:
    UINT m_uTextStyle;
    DWORD m_dwTextColor = 0;
    DWORD m_dwDisabledTextColor = 0;
    int m_iFont = -1;

    bool m_bShowHtml = false;
    bool m_bAutoCalcWidth = false;
    bool m_bAutoCalcHeight = false;
    bool m_bNeedEstimateSize = false;

    RECT m_rcTextPadding = {};
    SIZE m_szAvailableLast = {};
    SIZE m_cxyFixedLast = {};

};
} // namespace DuiLib

#endif // __UILABEL_H__