#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__
#pragma once

#include <set>
#include <string>

#include "include/cef_version.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"
#include "include/wrapper/cef_resource_manager.h"

#define ClientWindowHandle CefWindowHandle

namespace Browser
{
// Client handler abstract base class.
//Provides common functionality shared by all concrete client handler implementations.
class ClientHandler
    : public CefClient             // 浏览器客户区
    , public CefContextMenuHandler // 浏览器上下文菜单处理
    , public CefDisplayHandler     // 浏览器显示处理
    , public CefDownloadHandler    // 浏览器下载处理
    , public CefDragHandler        // 浏览器拖拽处理
    , public CefJSDialogHandler    // 浏览器JS对话框处理
    , public CefKeyboardHandler    // 浏览器键盘处理
    , public CefLifeSpanHandler    // 浏览器生命周期处理
    , public CefLoadHandler        // 浏览器加载处理
    , public CefRequestHandler     // 浏览器请求处理
{
public:
    friend class BrowserWindow;

    CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
    CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
    CefRefPtr<CefDownloadHandler> GetDownloadHandler() override { return this; }
    CefRefPtr<CefDragHandler> GetDragHandler() override { return this; }
    CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override { return this; }
    CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() override { return this; }
    CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
    CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }

    // Implement this interface to receive notification of ClientHandler events.
    // The methods of this class will be called on the main thread.
    class Delegate
    {
    public:
        // Called when the browser is created.
        virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) = 0;

        // Called when the browser is closing.
        virtual void OnBrowserClosing(CefRefPtr<CefBrowser> browser) = 0;

        // Called when the browser has been closed.
        virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) = 0;

        // Called when all browser has been closed.
        virtual void OnBrowserAllClosed() = 0;

        // Set the window URL address.
        virtual void OnSetAddress(CefRefPtr<CefBrowser> browser, const CefString& url) = 0;

        // Set the window title.
        virtual void OnSetTitle(CefRefPtr<CefBrowser> browser, const CefString& title) = 0;

        // Set fullscreen mode.
        virtual void OnSetFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen) = 0;

        // Set the loading state.
        virtual void OnSetLoadingState(CefRefPtr<CefBrowser> browser,
                                       bool isLoading,
                                       bool canGoBack,
                                       bool canGoForward) = 0;

        // Set the draggable regions.
        virtual void OnSetDraggableRegions(CefRefPtr<CefBrowser> browser,
                                           const std::vector<CefDraggableRegion>& regions) = 0;

        virtual void OnNewTab(CefRefPtr<CefBrowser> browser, const CefString& url) = 0;

    protected:
        virtual ~Delegate() {}
    };

    typedef std::set<CefMessageRouterBrowserSide::Handler*> MessageHandlerSet;

public:
    // Constructor may be called on any thread.
    // |delegate| must outlive this object or DetachDelegate() must be called.
    ClientHandler(Delegate* delegate, const CefString& startup_url);

    // This object may outlive the Delegate object so it's necessary for the
    // Delegate to detach itself before destruction.
    void DetachDelegate();

    //////////////////////////////////////////////////////////////////////////
    // CefClient methods

    bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                  CefRefPtr<CefFrame> frame,
                                  CefProcessId source_process,
                                  CefRefPtr<CefProcessMessage> message) override;

    //////////////////////////////////////////////////////////////////////////
    // CefContextMenuHandler methods
    void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
                             CefRefPtr<CefFrame> frame,
                             CefRefPtr<CefContextMenuParams> params,
                             CefRefPtr<CefMenuModel> model) override;

    bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefContextMenuParams> params,
                              int command_id,
                              EventFlags event_flags) override;

    //////////////////////////////////////////////////////////////////////////
    // CefDisplayHandler methods
    void OnAddressChange(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         const CefString& url) override;

    void OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title) override;

    void OnFaviconURLChange(CefRefPtr<CefBrowser> browser,
                            const std::vector<CefString>& icon_urls) override;

    void OnFullscreenModeChange(CefRefPtr<CefBrowser> browser, bool fullscreen) override;

    bool OnConsoleMessage(CefRefPtr<CefBrowser> browser,
                          cef_log_severity_t level,
                          const CefString& message,
                          const CefString& source,
                          int line) override;

    bool OnTooltip(CefRefPtr<CefBrowser> browser, CefString& text) override;

    void OnStatusMessage(CefRefPtr<CefBrowser> browser, const CefString& value) override;

    //////////////////////////////////////////////////////////////////////////
    // CefDownloadHandler methods
    void OnBeforeDownload(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefDownloadItem> download_item,
                          const CefString& suggested_name,
                          CefRefPtr<CefBeforeDownloadCallback> callback) override;

    void OnDownloadUpdated(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefDownloadItem> download_item,
                           CefRefPtr<CefDownloadItemCallback> callback) override;

    //////////////////////////////////////////////////////////////////////////
    // CefDragHandler methods
    bool OnDragEnter(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefDragData> dragData,
                     CefDragHandler::DragOperationsMask mask) override;

    void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   const std::vector<CefDraggableRegion>& regions) override;

    //////////////////////////////////////////////////////////////////////////
    // CefKeyboardHandler methods
    bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                       const CefKeyEvent& event,
                       CefEventHandle os_event,
                       bool* is_keyboard_shortcut) override;

    //////////////////////////////////////////////////////////////////////////
    // CefLifeSpanHandler methods
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               const CefString& target_url,
                               const CefString& target_frame_name,
                               cef_window_open_disposition_t target_disposition,
                               bool user_gesture,
                               const CefPopupFeatures& popupFeatures,
                               CefWindowInfo& windowInfo,
                               CefRefPtr<CefClient>& client,
                               CefBrowserSettings& settings,
                               CefRefPtr<CefDictionaryValue>& extra_info,
                               bool* no_javascript_access) override;

    void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;

    bool DoClose(CefRefPtr<CefBrowser> browser) override;

    void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

    //////////////////////////////////////////////////////////////////////////
    // CefLoadHandler methods
    void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
                              bool isLoading,
                              bool canGoBack,
                              bool canGoForward) override;

    void OnLoadStart(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     TransitionType transition_type) override;

    void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                   CefRefPtr<CefFrame> frame,
                   int httpStatusCode) override;

    void OnLoadError(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefFrame> frame,
                     ErrorCode errorCode,
                     const CefString& errorText,
                     const CefString& failedUrl) override;

    //////////////////////////////////////////////////////////////////////////
    // CefRequestHandler methods
    bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefRequest> request,
                        bool user_gesture,
                        bool is_redirect) override;

    bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          const CefString& target_url,
                          cef_window_open_disposition_t target_disposition,
                          bool user_gesture) override;

    bool OnCertificateError(CefRefPtr<CefBrowser> browser,
                            ErrorCode cert_error,
                            const CefString& request_url,
                            CefRefPtr<CefSSLInfo> ssl_info,
                            CefRefPtr<CefCallback> callback) override;

    void OnRenderProcessTerminated(CefRefPtr<CefBrowser> browser,
                                   TerminationStatus status) override;

    // Returns the Delegate.
    Delegate* delegate() const { return m_Delegate; }

    // Returns the Browser.
    CefRefPtr<CefBrowser> GetBrowser(int nBrowserId = -1);

    // Show a new DevTools popup window.
    void ShowDevTools(CefRefPtr<CefBrowser> browser, const CefPoint& inspect_element_at);

    // Close the existing DevTools popup window, if any.
    void CloseDevTools(CefRefPtr<CefBrowser> browser);

    // Returns the startup URL.
    std::string StartupUrl() const { return m_sStartupUrl; }

private:
    bool CreatePopupWindow(CefRefPtr<CefBrowser> browser,
                           bool is_devtools,
                           const CefPopupFeatures& popupFeatures,
                           CefWindowInfo& windowInfo,
                           CefRefPtr<CefClient>& client,
                           CefBrowserSettings& settings);

    // Execute Delegate notifications on the main thread.
    void NotifyBrowserCreated(CefRefPtr<CefBrowser> browser);
    void NotifyBrowserClosing(CefRefPtr<CefBrowser> browser);
    void NotifyBrowserClosed(CefRefPtr<CefBrowser> browser);
    void NotifyBrowserAllClosed();
    void NotifyAddress(CefRefPtr<CefBrowser> browser, const CefString& url);
    void NotifyTitle(CefRefPtr<CefBrowser> browser, const CefString& title);
    void NotifyFullscreen(CefRefPtr<CefBrowser> browser, bool fullscreen);
    void NotifyLoadingState(CefRefPtr<CefBrowser> browser,
                            bool isLoading,
                            bool canGoBack,
                            bool canGoForward);
    void NotifyDraggableRegions(CefRefPtr<CefBrowser> browser,
                                const std::vector<CefDraggableRegion>& regions);
    void NotifyNewTab(CefRefPtr<CefBrowser> browser, const CefString& url);

private:
    Delegate* m_Delegate;
    std::vector<CefRefPtr<CefBrowser>> m_BrowserList;
    int m_nDefaultBrowserId;

    // Handles the browser side of query routing. The renderer side is handled
    // in client_renderer.cc.
    CefRefPtr<CefMessageRouterBrowserSide> m_MessageRouter;

    // Manages the registration and delivery of resources.
    CefRefPtr<CefResourceManager> m_ResourceManager;

    // Set of Handlers registered with the message router.
    MessageHandlerSet m_MessageHandlerSet;

    // True if this handler uses off-screen rendering.
    //const bool is_osr_;

    // The startup URL.
    const std::string m_sStartupUrl;

    // Console logging state.
    const std::string m_sConsoleLogFile;
    bool m_bFirstConsoleMessage;

    // True if an editable field currently has focus.
    bool m_bFocusOnEditableField;

    // True if mouse cursor change is disabled.
    bool m_bMouseCursorChangeDisabled;

    IMPLEMENT_REFCOUNTING(ClientHandler);
    //IMPLEMENT_LOCKING(ClientHandler);
    DISALLOW_COPY_AND_ASSIGN(ClientHandler);
};
} // namespace Browser

#endif
