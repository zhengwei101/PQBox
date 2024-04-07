#pragma once

#include <string>

#include "include/base/cef_ref_counted.h"
#include "include/base/cef_thread_checker.h"
#include "include/internal/cef_types_wrappers.h"
#include "include/cef_command_line.h"
#include "BrowserDlgManager.h"

namespace Browser
{
class BrowserDlgManager;

// Used to store global context in the browser process. The methods of this
// class are thread-safe unless otherwise indicated.
class MainContext
{
public:
    // Returns the singleton instance of this object.
    static MainContext* Inst();

    MainContext(CefRefPtr<CefCommandLine> command_line, bool terminate_when_all_windows_closed);

public:
    // Returns the full path to the console log file.
    std::string GetConsoleLogPath();

    // Returns the full path to |file_name|.
    std::string GetDownloadPath(const std::string& file_name);

    // Returns the app working directory including trailing path separator.
    std::string GetAppWorkingDirectory();

    // Returns the main application URL.
    std::wstring GetMainURL();

    // Returns the background color.
    virtual cef_color_t GetBackgroundColor();

    // Returns true if the Chrome runtime will be used.
    virtual bool UseChromeRuntime();

    // Returns true if the Views framework will be used.
    virtual bool UseViews();

    // Returns true if windowless (off-screen) rendering will be used.
    virtual bool UseWindowlessRendering();

    // Returns true if touch events are enabled.
    virtual bool TouchEventsEnabled();

    // Returns true if the default popup implementation should be used.
    virtual bool UseDefaultPopup();

    // Populate |settings| based on command-line arguments.
    virtual void PopulateSettings(CefSettings* settings);
    virtual void PopulateBrowserSettings(CefBrowserSettings* settings);
    //virtual void PopulateOsrSettings(OsrRendererSettings* settings);

    // Returns the object used to create/manage RootWindow instances.
    virtual BrowserDlgManager* GetBrowserDlgManager();

    // Initialize CEF and associated main context state. This method must be
    // called on the same thread that created this object.
    bool Initialize(const CefMainArgs& args,
                    const CefSettings& settings,
                    CefRefPtr<CefApp> application,
                    void* windows_sandbox_info);

    // Shut down CEF and associated context state. This method must be called on
    // the same thread that created this object.
    void Shutdown();

private:
    // Allow deletion via std::unique_ptr only.
    friend struct std::default_delete<MainContext>;
    virtual ~MainContext();

    // Returns true if the context is in a valid state (initialized and not yet
    // shut down).
    bool InValidState() const { return initialized_ && !shutdown_; }

    CefRefPtr<CefCommandLine> command_line_;
    const bool terminate_when_all_windows_closed_;

    // Track context state. Accessing these variables from multiple threads is
    // safe because only a single thread will exist at the time that they're set
    // (during context initialization and shutdown).
    bool initialized_ = false;
    bool shutdown_ = false;
    std::wstring main_url_;
    cef_color_t background_color_ = 0;
    cef_color_t browser_background_color_ = 0;
    bool use_windowless_rendering_;
    int windowless_frame_rate_ = 0;
    bool use_chrome_runtime_;
    bool use_views_;

    std::unique_ptr<BrowserDlgManager> root_window_manager_;

    bool shared_texture_enabled_;

    bool external_begin_frame_enabled_;

    // Used to verify that methods are called on the correct thread.
    base::ThreadChecker thread_checker_;

    DISALLOW_COPY_AND_ASSIGN(MainContext);
};
}
