#include "stdafx.h"
#include "TempWindow.h"
#include "include/base/cef_logging.h"

namespace Browser
{
namespace
{
const wchar_t kWndClass[] = L"Client_TempWindow";

// Create the temp window.
HWND CreateTempWindow()
{
    HINSTANCE hInstance = ::GetModuleHandle(NULL);

    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = kWndClass;
    RegisterClassEx(&wc);

    // Create a 1x1 pixel hidden window.
    return CreateWindow(kWndClass,
                        0,
                        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                        0,
                        0,
                        1,
                        1,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);
}

TempWindow* g_pTempWindow = nullptr;
} // namespace

TempWindow::TempWindow()
{
    DCHECK(!g_pTempWindow);
    g_pTempWindow = this;
    m_hWnd = CreateTempWindow();
    CHECK(m_hWnd);
}

TempWindow::~TempWindow()
{
    g_pTempWindow = nullptr;
    DCHECK(m_hWnd);
    DestroyWindow(m_hWnd);
}

// static
CefWindowHandle TempWindow::GetWindowHandle()
{
    DCHECK(g_pTempWindow);
    return g_pTempWindow->m_hWnd;
}
} // namespace Browser