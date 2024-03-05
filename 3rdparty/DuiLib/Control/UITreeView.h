#ifndef UITreeView_h__
#define UITreeView_h__

#pragma once

namespace DuiLib {
class CTreeViewUI;
class CCheckBoxUI;
class CLabelUI;
class COptionUI;

class UILIB_API CTreeNodeUI : public CListContainerElementUI
{
    DECLARE_DUICONTROL(CTreeNodeUI)
public:
    CTreeNodeUI(CTreeNodeUI* _ParentNode = NULL);
    ~CTreeNodeUI(void);

public:
    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    void DoEvent(TEventUI& event);
    void Invalidate();
    bool Select(bool bSelect = true);
    bool SelectMulti(bool bSelect = true);

    bool Add(CControlUI* _pTreeNodeUI);
    bool AddAt(CControlUI* pControl, int iIndex);
    bool Remove(CControlUI* pControl);

    void SetVisibleTag(bool _IsVisible);
    bool GetVisibleTag();
    void SetItemText(LPCTSTR pstrValue);
    CDuiString GetItemText();
    void CheckBoxSelected(bool _Selected);
    bool IsCheckBoxSelected() const;
    bool IsHasChild() const;
    long GetTreeLevel();
    bool AddChildNode(CTreeNodeUI* _pTreeNodeUI);
    bool RemoveAt(CTreeNodeUI* _pTreeNodeUI);
    void SetParentNode(CTreeNodeUI* _pParentTreeNode);
    CTreeNodeUI* GetParentNode();
    long GetCountChild();
    void SetTreeView(CTreeViewUI* _CTreeViewUI);
    CTreeViewUI* GetTreeView();
    CTreeNodeUI* GetChildNode(int _nIndex);
    void SetVisibleFolderBtn(bool _IsVisibled);
    bool GetVisibleFolderBtn();
    void SetVisibleCheckBtn(bool _IsVisibled);
    bool GetVisibleCheckBtn();
    void SetItemTextColor(DWORD _dwItemTextColor);
    DWORD GetItemTextColor() const;
    void SetItemHotTextColor(DWORD _dwItemHotTextColor);
    DWORD GetItemHotTextColor() const;
    void SetSelItemTextColor(DWORD _dwSelItemTextColor);
    DWORD GetSelItemTextColor() const;
    void SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor);
    DWORD GetSelItemHotTextColor() const;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    CStdPtrArray GetTreeNodes();
    int GetTreeIndex();
    int GetNodeIndex();

public:
    CHorizontalLayoutUI* GetTreeNodeHoriznotal() const
    {
        return pHoriz;
    };
    CCheckBoxUI* GetFolderButton() const
    {
        return pFolderButton;
    };
    CLabelUI* GetDottedLine() const
    {
        return pDottedLine;
    };
    CCheckBoxUI* GetCheckBox() const
    {
        return pCheckBox;
    };
    COptionUI* GetItemButton() const
    {
        return pItemButton;
    };

private:
    CTreeNodeUI* GetLastNode();
    CTreeNodeUI* CalLocation(CTreeNodeUI* _pTreeNodeUI);

private:
    long m_iTreeLavel = 0;
    bool m_bIsVisable = TRUE;
    bool m_bIsCheckBox = FALSE;

    DWORD m_dwItemTextColor = 0x00000000;
    DWORD m_dwItemHotTextColor = 0;
    DWORD m_dwSelItemTextColor = 0;
    DWORD m_dwSelItemHotTextColor = 0;

    CTreeViewUI* pTreeView = nullptr;
    CHorizontalLayoutUI* pHoriz = nullptr;
    CCheckBoxUI* pFolderButton = nullptr;
    CLabelUI* pDottedLine = nullptr;
    CCheckBoxUI* pCheckBox = nullptr;
    COptionUI* pItemButton = nullptr;
    CTreeNodeUI* pParentTreeNode = nullptr;
    CStdPtrArray mTreeNodes;
};

class UILIB_API CTreeViewUI : public CListUI, public INotifyUI
{
    DECLARE_DUICONTROL(CTreeViewUI)
public:
    CTreeViewUI(void);
    ~CTreeViewUI(void);

public:
    LPCTSTR GetClass() const override;

    LPVOID GetInterface(LPCTSTR pstrName) override;

    ListType GetListType() override;

    virtual bool Add(CTreeNodeUI* pControl);
    virtual long AddAt(CTreeNodeUI* pControl, int iIndex);
    virtual bool AddAt(CTreeNodeUI* pControl, CTreeNodeUI* _IndexNode);
    virtual bool Remove(CTreeNodeUI* pControl);

    bool RemoveAt(int iIndex) override;
    void RemoveAll() override;

    virtual bool OnCheckBoxChanged(void* param);
    virtual bool OnFolderChanged(void* param);
    virtual bool OnDBClickItem(void* param);
    virtual bool SetItemCheckBox(bool _Selected, CTreeNodeUI* _TreeNode = NULL);
    virtual void SetItemExpand(bool _Expanded, CTreeNodeUI* _TreeNode = NULL);

    void Notify(TNotifyUI& msg) override;

    virtual void SetVisibleFolderBtn(bool _IsVisibled);
    virtual bool GetVisibleFolderBtn();
    virtual void SetVisibleCheckBtn(bool _IsVisibled);
    virtual bool GetVisibleCheckBtn();
    virtual void SetItemMinWidth(UINT _ItemMinWidth);
    virtual UINT GetItemMinWidth();
    virtual void SetItemTextColor(DWORD _dwItemTextColor);
    virtual void SetItemHotTextColor(DWORD _dwItemHotTextColor);
    virtual void SetSelItemTextColor(DWORD _dwSelItemTextColor);
    virtual void SetSelItemHotTextColor(DWORD _dwSelHotItemTextColor);

    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

private:
    UINT m_uItemMinWidth;
    bool m_bVisibleFolderBtn;
    bool m_bVisibleCheckBtn;
};
} // namespace DuiLib

#endif // UITreeView_h__
