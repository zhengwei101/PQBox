#ifndef __UITEXT_H__
#define __UITEXT_H__

#pragma once

namespace DuiLib {
class UILIB_API CTextUI : public CLabelUI
{
    DECLARE_DUICONTROL(CTextUI)
public:
    CTextUI();
    ~CTextUI();

    LPCTSTR GetClass() const override;
    UINT GetControlFlags() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;

    CDuiString* GetLinkContent(int iIndex);

    void DoEvent(TEventUI& event) override;
    SIZE EstimateSize(SIZE szAvailable) override;

    void PaintText(HDC hDC) override;

protected:
    enum
    {
        MAX_LINK = 8
    };

    int m_nLinks = 0;
    int m_nHoverLink = -1;

    RECT m_rcLinks[MAX_LINK];
    CDuiString m_sLinks[MAX_LINK];
    
};

} // namespace DuiLib

#endif //__UITEXT_H__