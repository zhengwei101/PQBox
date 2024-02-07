#ifndef __UIHORIZONTALLAYOUT_H__
#define __UIHORIZONTALLAYOUT_H__

#pragma once

namespace DuiLib {
class UILIB_API CHorizontalLayoutUI : public CContainerUI
{
    DECLARE_DUICONTROL(CHorizontalLayoutUI)
public:
    CHorizontalLayoutUI();

    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    UINT GetControlFlags() const override;

    void SetSepWidth(int iWidth);
    int GetSepWidth() const;
    void SetSepImmMode(bool bImmediately);
    bool IsSepImmMode() const;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void DoEvent(TEventUI& event);

    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void DoPostPaint(HDC hDC, const RECT& rcPaint);

    RECT GetThumbRect(bool bUseNew = false) const;

protected:
    int m_iSepWidth;
    UINT m_uButtonState;
    POINT m_ptLastMouse;
    RECT m_rcNewPos;
    bool m_bImmMode;
};
} // namespace DuiLib
#endif // __UIHORIZONTALLAYOUT_H__
