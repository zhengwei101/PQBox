#ifndef __CLIENT_SWITCHES_H__
#define __CLIENT_SWITCHES_H__
#pragma once

// Defines all of the command line switches used by cefclient.

namespace Browser {
namespace Switches {
extern const char kMultiThreadedMessageLoop[];
extern const char kExternalMessagePump[];
extern const char kCachePath[];
extern const char kUrl[];
extern const char kOffScreenRenderingEnabled[];
extern const char kOffScreenFrameRate[];
extern const char kTransparentPaintingEnabled[];
extern const char kShowUpdateRect[];
extern const char kSharedTextureEnabled[];
extern const char kExternalBeginFrameEnabled[];
extern const char kMouseCursorChangeDisabled[];
extern const char kOffline[];
extern const char kRequestContextPerBrowser[];
extern const char kRequestContextSharedCache[];
extern const char kBackgroundColor[];
extern const char kEnableGPU[];
extern const char kFilterURL[];
extern const char kUseViews[];
extern const char kUseNative[];
extern const char kHideFrame[];
extern const char kHideControls[];
extern const char kHideOverlays[];
extern const char kAlwaysOnTop[];
extern const char kHideTopMenu[];
extern const char kSslClientCertificate[];
extern const char kCRLSetsPath[];
extern const char kLoadExtension[];
extern const char kNoActivate[];
extern const char kEnableChromeRuntime[];
extern const char kShowChromeToolbar[];
extern const char kInitialShowState[];
extern const char kHideChromeStatusBubble[];
extern const char kUseDefaultPopup[];
extern const char kUseClientDialogs[];
extern const char kUseTestHttpServer[];
} // namespace Switches
} // namespace Browser

#endif