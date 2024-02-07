#ifndef __UIVERTICALLAYOUT_H__
#define __UIVERTICALLAYOUT_H__

#pragma once

namespace DuiLib {
class UILIB_API CVerticalLayoutUI : public CContainerUI
{
    DECLARE_DUICONTROL(CVerticalLayoutUI)
public:
    CVerticalLayoutUI();

    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    UINT GetControlFlags() const override;

    void SetSepHeight(int iHeight);
    int GetSepHeight() const;
    void SetSepImmMode(bool bImmediately);
    bool IsSepImmMode() const;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
    void DoEvent(TEventUI& event);

    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void DoPostPaint(HDC hDC, const RECT& rcPaint);

    RECT GetThumbRect(bool bUseNew = false) const;

protected:
    int m_iSepHeight;
    UINT m_uButtonState;
    POINT m_ptLastMouse;
    RECT m_rcNewPos;
    bool m_bImmMode;
};
} // namespace DuiLib
#endif // __UIVERTICALLAYOUT_H__
