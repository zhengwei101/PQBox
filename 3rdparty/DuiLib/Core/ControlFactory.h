#pragma once

#include <map>

namespace DuiLib {

using CreateClass = CControlUI* (*)();

class UILIB_API CControlFactory
{
public:
    CControlUI* CreateControl(CDuiString strClassName);
    void RegistControl(CDuiString strClassName, CreateClass pFunc);

    static CControlFactory* GetInstance();
    void Release();

private:
    CControlFactory();
    virtual ~CControlFactory();

private:
    std::map<CDuiString, CreateClass> m_mapControl;
};

#define DECLARE_DUICONTROL(class_name)                                         \
public:                                                                        \
    static CControlUI* CreateControl();

#define IMPLEMENT_DUICONTROL(class_name)                                       \
    CControlUI* class_name::CreateControl()                                    \
    {                                                                          \
        return new class_name;                                                 \
    }

#define REGIST_DUICONTROL(class_name)                                          \
    CControlFactory::GetInstance()->RegistControl(                             \
        _T(#class_name), (CreateClass)class_name::CreateControl);

#define INNER_REGISTER_DUICONTROL(class_name)                                  \
    RegistControl(_T(#class_name), (CreateClass)class_name::CreateControl);

#define INNER_REGISTER_DUICONTROL_EX(class_name, class_type)                   \
    {                                                                          \
        CDuiString sRegClassName;                                              \
        sRegClassName.Format(_T("C%sUI"), class_name);                         \
        RegistControl(sRegClassName, (CreateClass)class_type::CreateControl);  \
    }
} // namespace DuiLib