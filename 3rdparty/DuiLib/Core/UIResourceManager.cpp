#include "StdAfx.h"
#include "UIResourceManager.h"

namespace DuiLib {

CResourceManager::CResourceManager()
{
    m_pQuerypInterface = nullptr;
}

CResourceManager::~CResourceManager()
{
    ResetResourceMap();
}

BOOL CResourceManager::LoadResource(STRINGorID xml, LPCTSTR type)
{
    if (HIWORD(xml.m_lpstr) != NULL)
    {
        if (*(xml.m_lpstr) == _T('<'))
        {
            if (!m_xml.Load(xml.m_lpstr)){
                return false;
            }
                
        }
        else
        {
            if (!m_xml.LoadFromFile(xml.m_lpstr)){
                return false;
            }
        }
    }
    else
    {
        HRSRC hResource = ::FindResource(
            CPaintManagerUI::GetResourceDll(), xml.m_lpstr, type);

        if (hResource == NULL)
            return false;

        HGLOBAL hGlobal =
            ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
        if (hGlobal == NULL)
        {
            FreeResource(hResource);
            return false;
        }

        if (!m_xml.LoadFromMem(
                (BYTE*)::LockResource(hGlobal),
                ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource)))
        {
            return false;
        }

        ::FreeResource(hGlobal);
    }

    return LoadResource(m_xml.GetRoot());
}

BOOL CResourceManager::LoadResource(CMarkupNode Root)
{
    if (!Root.IsValid())
        return FALSE;

    int nAttributes = 0;
    LPCTSTR pstrClass = nullptr;
    LPCTSTR pstrName = nullptr;
    LPCTSTR pstrValue = nullptr;
    //LPTSTR pstr = nullptr;

    //����ͼƬ��Դ
    LPCTSTR pstrId = nullptr;
    LPCTSTR pstrPath = nullptr;

    for (CMarkupNode node = Root.GetChild(); node.IsValid();
         node = node.GetSibling())
    {
        pstrClass = node.GetName();
        CMarkupNode ChildNode = node.GetChild();
        if (ChildNode.IsValid())
            LoadResource(node);
        else if ((_tcsicmp(pstrClass, _T("Image")) == 0) &&
                 node.HasAttributes())
        {
            //����ͼƬ��Դ
            nAttributes = node.GetAttributeCount();
            for (int i = 0; i < nAttributes; i++)
            {
                pstrName = node.GetAttributeName(i);
                pstrValue = node.GetAttributeValue(i);

                if (_tcsicmp(pstrName, _T("id")) == 0)
                {
                    pstrId = pstrValue;
                }
                else if (_tcsicmp(pstrName, _T("path")) == 0)
                {
                    pstrPath = pstrValue;
                }
            }

            if (pstrId == NULL || pstrPath == NULL)
                continue;

            CDuiString* pstrFind =
                static_cast<CDuiString*>(m_mImageHashMap.Find(pstrId));
            if (pstrFind)
                continue;

            m_mImageHashMap.Insert(pstrId, (LPVOID) new CDuiString(pstrPath));
        }
        else if (_tcsicmp(pstrClass, _T("Xml")) == 0 && node.HasAttributes())
        {
            //����XML�����ļ�
            nAttributes = node.GetAttributeCount();
            for (int i = 0; i < nAttributes; i++)
            {
                pstrName = node.GetAttributeName(i);
                pstrValue = node.GetAttributeValue(i);

                if (_tcsicmp(pstrName, _T("id")) == 0)
                {
                    pstrId = pstrValue;
                }
                else if (_tcsicmp(pstrName, _T("path")) == 0)
                {
                    pstrPath = pstrValue;
                }
            }

            if (pstrId == NULL || pstrPath == NULL)
                continue;

            CDuiString* pstrFind =
                static_cast<CDuiString*>(m_mXmlHashMap.Find(pstrId));

            if (pstrFind)
                continue;

            m_mXmlHashMap.Insert(pstrId, (LPVOID) new CDuiString(pstrPath));
        }
        else
            continue;
    }
    return TRUE;
}

LPCTSTR CResourceManager::GetImagePath(LPCTSTR lpstrId)
{
    CDuiString* lpStr = static_cast<CDuiString*>(m_mImageHashMap.Find(lpstrId));
    return lpStr == nullptr ? nullptr : lpStr->GetData();
}

LPCTSTR CResourceManager::GetXmlPath(LPCTSTR lpstrId)
{
    CDuiString* lpStr = static_cast<CDuiString*>(m_mXmlHashMap.Find(lpstrId));
    return lpStr == nullptr ? nullptr : lpStr->GetData();
}

void CResourceManager::ResetResourceMap()
{
    CDuiString* lpStr;
    for (int i = 0; i < m_mImageHashMap.GetSize(); i++)
    {
        if (LPCTSTR key = m_mImageHashMap.GetAt(i))
        {
            lpStr = static_cast<CDuiString*>(m_mImageHashMap.Find(key));
            delete lpStr;
            lpStr = nullptr;
        }
    }
    for (int i = 0; i < m_mXmlHashMap.GetSize(); i++)
    {
        if (LPCTSTR key = m_mXmlHashMap.GetAt(i))
        {
            lpStr = static_cast<CDuiString*>(m_mXmlHashMap.Find(key));
            delete lpStr;
            lpStr = nullptr;
        }
    }
    for (int i = 0; i < m_mTextResourceHashMap.GetSize(); i++)
    {
        if (LPCTSTR key = m_mTextResourceHashMap.GetAt(i))
        {
            lpStr = static_cast<CDuiString*>(m_mTextResourceHashMap.Find(key));
            delete lpStr;
            lpStr = nullptr;
        }
    }
}

BOOL CResourceManager::LoadLanguage(LPCTSTR pstrXml)
{
    CMarkup xml;
    if (*(pstrXml) == _T('<'))
    {
        if (!xml.Load(pstrXml))
            return FALSE;
    }
    else
    {
        if (!xml.LoadFromFile(pstrXml))
            return FALSE;
    }

    CMarkupNode Root = xml.GetRoot();
    if (!Root.IsValid())
        return FALSE;

    int nAttributes = 0;
    LPCTSTR pstrClass = nullptr;
    LPCTSTR pstrName = nullptr;
    LPCTSTR pstrValue = nullptr;
    //LPTSTR pstr = nullptr;

    //����ͼƬ��Դ
    LPCTSTR pstrId = nullptr;
    LPCTSTR pstrText = nullptr;

    for (CMarkupNode node = Root.GetChild(); node.IsValid();
         node = node.GetSibling())
    {
        pstrClass = node.GetName();
        if ((_tcsicmp(pstrClass, _T("Text")) == 0) && node.HasAttributes())
        {
            //����ͼƬ��Դ
            nAttributes = node.GetAttributeCount();
            for (int i = 0; i < nAttributes; i++)
            {
                pstrName = node.GetAttributeName(i);
                pstrValue = node.GetAttributeValue(i);

                if (_tcsicmp(pstrName, _T("id")) == 0)
                {
                    pstrId = pstrValue;
                }
                else if (_tcsicmp(pstrName, _T("value")) == 0)
                {
                    pstrText = pstrValue;
                }
            }

            if (pstrId == nullptr || pstrText == nullptr)
                continue;

            CDuiString* lpstrFind =
                static_cast<CDuiString*>(m_mTextResourceHashMap.Find(pstrId));
            if (lpstrFind != nullptr)
            {
                lpstrFind->Assign(pstrText);
            }
            else
            {
                m_mTextResourceHashMap.Insert(
                    pstrId, (LPVOID) new CDuiString(pstrText));
            }
        }
        else
            continue;
    }

    return TRUE;
}

CDuiString CResourceManager::GetText(LPCTSTR lpstrId, LPCTSTR lpstrType)
{
    if (lpstrId == nullptr)
        return _T("");

    CDuiString* lpstrFind =
        static_cast<CDuiString*>(m_mTextResourceHashMap.Find(lpstrId));
    if (lpstrFind == nullptr && m_pQuerypInterface)
    {
        LPCTSTR lpText =
            m_pQuerypInterface->QueryControlText(lpstrId, lpstrType);
        if (lpText != nullptr)
        {
            lpstrFind = new CDuiString(lpText);
            m_mTextResourceHashMap.Insert(lpstrId, (LPVOID)lpstrFind);
        }
    }

    return lpstrFind == nullptr ? lpstrId : lpstrFind->GetData();
}

void CResourceManager::ReloadText()
{
    if (m_pQuerypInterface == nullptr)
        return;

    //������������
    LPCTSTR lpstrId = nullptr;
    LPCTSTR lpstrText = nullptr;
    for (int i = 0; i < m_mTextResourceHashMap.GetSize(); i++)
    {
        lpstrId = m_mTextResourceHashMap.GetAt(i);
        if (lpstrId == nullptr)
            continue;
        lpstrText = m_pQuerypInterface->QueryControlText(lpstrId, NULL);
        if (lpstrText != nullptr)
        {
            CDuiString* lpStr =
                static_cast<CDuiString*>(m_mTextResourceHashMap.Find(lpstrId));
            lpStr->Assign(lpstrText);
        }
    }
}
void CResourceManager::ResetTextMap()
{
    CDuiString* lpStr;
    for (int i = 0; i < m_mTextResourceHashMap.GetSize(); i++)
    {
        if (LPCTSTR key = m_mTextResourceHashMap.GetAt(i))
        {
            lpStr = static_cast<CDuiString*>(m_mTextResourceHashMap.Find(key));
            delete lpStr;
        }
    }
}

} // namespace DuiLib