#ifndef __UIRESOURCEMANAGER_H__
#define __UIRESOURCEMANAGER_H__

#pragma once

namespace DuiLib {

// 控件文字查询接口
class UILIB_API IQueryControlText
{
public:
    virtual LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType) = 0;
};

class UILIB_API CResourceManager
{
private:
    CResourceManager();
    ~CResourceManager();

public:
    static CResourceManager* GetInstance()
    {
        static CResourceManager* p = new CResourceManager;
        return p;
    };

    void Release()
    {
        delete this;
    }

public:
    BOOL LoadResource(STRINGorID xml, LPCTSTR type = NULL);
    BOOL LoadResource(CMarkupNode Root);
    void ResetResourceMap();
    LPCTSTR GetImagePath(LPCTSTR lpstrId);
    LPCTSTR GetXmlPath(LPCTSTR lpstrId);

public:
    void SetLanguage(LPCTSTR pstrLanguage)
    {
        m_sLauguage = pstrLanguage;
    }

    LPCTSTR GetLanguage()
    {
        return m_sLauguage;
    }

    BOOL LoadLanguage(LPCTSTR pstrXml);

public:
    void SetTextQueryInterface(IQueryControlText* pInterface)
    {
        m_pQuerypInterface = pInterface;
    }

    CDuiString GetText(LPCTSTR lpstrId, LPCTSTR lpstrType = NULL);

    void ReloadText();
    void ResetTextMap();

private:
    IQueryControlText* m_pQuerypInterface = nullptr;
    CStdStringPtrMap m_mTextResourceHashMap;
    CStdStringPtrMap m_mImageHashMap;
    CStdStringPtrMap m_mXmlHashMap;
    CMarkup m_xml;
    CDuiString m_sLauguage;
    CStdStringPtrMap m_mTextHashMap;
};

} // namespace DuiLib

#endif // __UIRESOURCEMANAGER_H__