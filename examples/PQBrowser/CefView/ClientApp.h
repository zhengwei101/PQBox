#ifndef __CLIENT_APP_H__
#define __CLIENT_APP_H__
#pragma once

#include <vector>
#include "include/cef_version.h"
#include "include/cef_app.h"

namespace Browser {
// Base class for customizing process-type-based behavior.
class ClientApp : public CefApp
{
public:
    ClientApp();

    enum ProcessType
    {
        BrowserProcess,
        RendererProcess,
        ZygoteProcess,
        OtherProcess,
    };

    static bool IsBrowser(CefRefPtr<CefCommandLine> command_line);
    static bool IsRenderer(CefRefPtr<CefCommandLine> command_line);
    static ProcessType GetProcessType(CefRefPtr<CefCommandLine> command_line);

protected:
    // Schemes that will be registered with the global cookie manager.
    std::vector<CefString> cookie_schemes;

private:
    DISALLOW_COPY_AND_ASSIGN(ClientApp);
};

// Client app implementation for other process types.
class ClientAppOther : public ClientApp
{
public:
    ClientAppOther() {}

private:
    IMPLEMENT_REFCOUNTING(ClientAppOther);
    DISALLOW_COPY_AND_ASSIGN(ClientAppOther);
};
} // namespace Browser

#endif
