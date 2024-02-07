#ifndef __UICOMBO_H__
#define __UICOMBO_H__

#pragma once

namespace DuiLib {


class CComboWnd;

class UILIB_API CComboUI : public CContainerUI, public IListOwnerUI
{
    DECLARE_DUICONTROL(CComboUI)
    friend class CComboWnd;

public:
    CComboUI();

    LPCTSTR GetClass() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void DoInit();
    UINT GetControlFlags() const;

    CDuiString GetText() const;
    void SetEnabled(bool bEnable = true);

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
    bool IsShowShadow();
    void SetShowShadow(bool bShow = true);

    CDuiString GetDropBoxAttributeList();
    void SetDropBoxAttributeList(LPCTSTR pstrList);
    SIZE GetDropBoxSize() const;
    void SetDropBoxSize(SIZE szDropBox);
    RECT GetDropBoxInset() const;
    void SetDropBoxInset(RECT szDropBox);
    void SetDropBoxPadding(RECT rcPadding);
    RECT GetDropBoxPadding() const;
    LPCTSTR GetDropBoxBkImage();
    void SetDropBoxBkImage(LPCTSTR pStrImage);

    UINT GetListType();
    TListInfoUI* GetListInfo();
    int GetCurSel() const;
    bool SelectItem(int iIndex, bool bTakeFocus = false);
    bool SelectMultiItem(int iIndex, bool bTakeFocus = false);
    bool UnSelectItem(int iIndex, bool bOthers = false);
    bool SetItemIndex(CControlUI* pControl, int iIndex);

    bool Add(CControlUI* pControl);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);
    bool RemoveAt(int iIndex);
    void RemoveAll();

    bool Activate();

    LPCTSTR GetNormalImage() const;
    void SetNormalImage(LPCTSTR pStrImage);
    LPCTSTR GetHotImage() const;
    void SetHotImage(LPCTSTR pStrImage);
    LPCTSTR GetPushedImage() const;
    void SetPushedImage(LPCTSTR pStrImage);
    LPCTSTR GetFocusedImage() const;
    void SetFocusedImage(LPCTSTR pStrImage);
    LPCTSTR GetDisabledImage() const;
    void SetDisabledImage(LPCTSTR pStrImage);

    bool GetScrollSelect();
    void SetScrollSelect(bool bScrollSelect);

    void SetItemFont(int index);
    void SetItemTextStyle(UINT uStyle);
    RECT GetItemTextPadding() const;
    void SetItemTextPadding(RECT rc);
    DWORD GetItemTextColor() const;
    void SetItemTextColor(DWORD dwTextColor);
    DWORD GetItemBkColor() const;
    void SetItemBkColor(DWORD dwBkColor);
    LPCTSTR GetItemBkImage() const;
    void SetItemBkImage(LPCTSTR pStrImage);
    bool IsAlternateBk() const;
    void SetAlternateBk(bool bAlternateBk);
    DWORD GetSelectedItemTextColor() const;
    void SetSelectedItemTextColor(DWORD dwTextColor);
    DWORD GetSelectedItemBkColor() const;
    void SetSelectedItemBkColor(DWORD dwBkColor);
    LPCTSTR GetSelectedItemImage() const;
    void SetSelectedItemImage(LPCTSTR pStrImage);
    DWORD GetHotItemTextColor() const;
    void SetHotItemTextColor(DWORD dwTextColor);
    DWORD GetHotItemBkColor() const;
    void SetHotItemBkColor(DWORD dwBkColor);
    LPCTSTR GetHotItemImage() const;
    void SetHotItemImage(LPCTSTR pStrImage);
    DWORD GetDisabledItemTextColor() const;
    void SetDisabledItemTextColor(DWORD dwTextColor);
    DWORD GetDisabledItemBkColor() const;
    void SetDisabledItemBkColor(DWORD dwBkColor);
    LPCTSTR GetDisabledItemImage() const;
    void SetDisabledItemImage(LPCTSTR pStrImage);
    DWORD GetItemLineColor() const;
    void SetItemLineColor(DWORD dwLineColor);
    bool IsItemShowHtml();
    void SetItemShowHtml(bool bShowHtml = true);

    SIZE EstimateSize(SIZE szAvailable);
    void SetPos(RECT rc, bool bNeedInvalidate = true);
    void Move(SIZE szOffset, bool bNeedInvalidate = true);
    void DoEvent(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
    void PaintText(HDC hDC);
    void PaintStatusImage(HDC hDC);

public:
    void SortItems();
    BOOL SortItems(PULVCompareFunc pfnCompare, UINT_PTR dwData);

protected:
    static int __cdecl ItemComareFunc(void* pvlocale,
                                      const void* item1,
                                      const void* item2);

    int __cdecl ItemComareFunc(const void* item1, const void* item2);

protected:
    PULVCompareFunc m_pCompareFunc = nullptr;
    UINT_PTR m_compareData = NULL;

protected:
    CComboWnd* m_pWindow = nullptr;

    UINT m_uTextStyle = DT_VCENTER | DT_SINGLELINE;
    DWORD m_dwTextColor = 0;
    DWORD m_dwDisabledTextColor = 0;
    int m_iFont = -1;
    int m_iCurSel = -1;
    UINT m_uButtonState = 0;
    bool m_bShowHtml = false;
    bool m_bShowShadow = false;
    bool m_bScrollSelect = true;

    RECT m_rcTextPadding;

    SIZE m_szDropBox;
    RECT m_rcDropBox;
    RECT m_rcDropBoxPadding;

    CDuiString m_sDropBoxAttributes;
    CDuiString m_sNormalImage;
    CDuiString m_sHotImage;
    CDuiString m_sPushedImage;
    CDuiString m_sFocusedImage;
    CDuiString m_sDisabledImage;
    CDuiString m_sDropBoxBkImage;

    TListInfoUI m_ListInfo;
};

} // namespace DuiLib

#endif // __UICOMBO_H__
