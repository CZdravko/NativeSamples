//----------------------------------------------------------------------------------
// File:        native_globe/jni/engine.h
// SDK Version: Android NVIDIA samples 4 
// Email:       tegradev@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2007-2012, NVIDIA CORPORATION.  All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA OR ITS SUPPLIERS
// BE  LIABLE  FOR  ANY  SPECIAL,  INCIDENTAL,  INDIRECT,  OR  CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
//
//----------------------------------------------------------------------------------
#ifndef __ENGINE_H
#define __ENGINE_H

#include <nv_and_util/nv_native_app_glue.h>
#include <nv_egl_util/nv_egl_util.h>
#include <nv_glesutil/nv_draw_rect.h>

#include "GlobeDefines.h"
#include "GlobeApp.h"
#include "GlobeUtil.h"
#include "NvSLESPlayer.h"

#define APP_NAME "native_globe"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG,  \
											 APP_NAME, \
											 __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO,  \
											 APP_NAME, \
											 __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN,  \
											 APP_NAME, \
											 __VA_ARGS__))

class Engine
{
public:
	Engine(NvEGLUtil& egl, struct android_app* app);
	~Engine();

	bool isGameplayMode() { return mCurrentMode == MODE_GAMEPLAY; }

	void updateFrame(bool interactible, long deltaTime);

protected:
	enum {
		UI_NO_HIT = 0,
		UI_HIT_LEFT,
		UI_HIT_RIGHT,
		UI_HIT_OTHER
	};

	enum {
		MODE_STARTUP_SCREEN = 0,
		MODE_GAMEPLAY,
		MODE_AUTOPAUSE,
		MODE_QUIT_CONFIRM,
		MODE_COUNT
	};

	void advanceTime(long time) { mTimeVal += time * 0.001; }

	bool isForcedRenderPending() { return mForceRender > 0; }
	void requestForceRender() { mForceRender = 4; }

	bool playLoopedMusic(bool play);

	bool checkWindowResized();

	bool renderFrame(bool allocateIfNeeded);

	static void handleCmdThunk(struct android_app* app, int32_t cmd);
	static int32_t handleInputThunk(struct android_app* app, AInputEvent* event);

	void setAppUIMode(unsigned int mode);
	int getAppUIMode() { return mCurrentMode; }

	bool initUI();
	int getUIClickResponse(int iX, int iY, int action);
	bool checkUIHandledMotion(int iX, int iY, int action);
	bool checkUIHandledKey(int code, int action, int& returnCode);

	bool resizeIfNeeded();

	int handleInput(AInputEvent* event);
	void handleCommand(int cmd);

    struct android_app* mApp;

    GlobeApp *mGlobeApp;
	NvEGLUtil& mEgl;

	unsigned int mSwapCount;
	bool mResizePending;

	unsigned int mCurrentMode;

	int mUIHalfSize;
	int mUICenterX;
	int mUICenterY;
    int mLastX, mLastY;
	int mForceRender;

    double mTimeVal;

	NvSLESPlayer* m_music;

	GLint m_uiModeTextures[MODE_COUNT];
	bool m_uiInitialized;
	NvDrawRect* mDrawRect;

	void *m_clockText;
};


#endif // __ENGINE_H
