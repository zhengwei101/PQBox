#ifndef __UITILELAYOUT_H__
#define __UITILELAYOUT_H__

#pragma once

namespace DuiLib {
class UILIB_API CTileLayoutUI : public CContainerUI
{
    DECLARE_DUICONTROL(CTileLayoutUI)
public:
    CTileLayoutUI();

    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    void SetPos(RECT rc, bool bNeedInvalidate = true) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    SIZE GetItemSize() const;
    void SetItemSize(SIZE szItem);
    int GetColumns() const;
    void SetColumns(int nCols);

protected:
    SIZE m_szItem;
    int m_nColumns = 1;
};
} // namespace DuiLib
#endif // __UITILELAYOUT_H__
