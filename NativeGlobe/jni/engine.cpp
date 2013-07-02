//----------------------------------------------------------------------------------
// File:        native_globe/jni/engine.cpp
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
#include "engine.h"

#include <EGL/egl.h>
#include <EGL/eglplatform.h>
#include <GLES2/gl2.h>

#include <android/log.h>
#include <nv_and_util/nv_native_app_glue.h>
#include <nv_egl_util/nv_egl_util.h>
#include <nv_bitfont/nv_bitfont.h>
#include <nv_glesutil/nv_images.h>
#include <nv_shader/nv_shader.h>


Engine::Engine(NvEGLUtil& egl, struct android_app* app) :
	mEgl(egl)
{
    mApp = app;

	mGlobeApp = NULL;

	mSwapCount = 0;
	mResizePending = false;

	mCurrentMode = MODE_STARTUP_SCREEN;

	mUIHalfSize = 0;
	mUICenterX = 0;
	mUICenterY = 0;
    mLastX = mLastY = 0;
	mForceRender = 4;

    mTimeVal = 0.0;

	mGlobeApp = new GlobeApp();

	m_music = NvSLESPlayer::create(app->activity->assetManager, "music.m4a");

    app->userData = this;
	app->onAppCmd = &Engine::handleCmdThunk;
    app->onInputEvent = &Engine::handleInputThunk;

	for (int i = 0; i < MODE_COUNT; i++)
		m_uiModeTextures[i] = 0;
	m_uiInitialized = false;

	mDrawRect = NULL;

	nv_shader_init(app->activity->assetManager);

	m_clockText = NULL;
}

Engine::~Engine()
{
	NVBFTextFree(m_clockText);
	NVBFCleanup();

	delete mDrawRect;

	if (mGlobeApp)
	{
		mGlobeApp->cleanup();
		delete mGlobeApp;
	}

	delete m_music;
}

bool Engine::initUI()
{
	if (m_uiInitialized)
		return true;

	m_uiModeTextures[MODE_STARTUP_SCREEN] 
		= NvCreateTextureFromDDSEx("splash_screen.dds", 0, 0, NULL, NULL, NULL, NULL);
	m_uiModeTextures[MODE_AUTOPAUSE] 
		= NvCreateTextureFromDDSEx("pause_screen.dds", 0, 0, NULL, NULL, NULL, NULL);
	m_uiModeTextures[MODE_QUIT_CONFIRM] 
		= NvCreateTextureFromDDSEx("quit_screen.dds", 0, 0, NULL, NULL, NULL, NULL);

	mDrawRect = new NvDrawRect();

	#define NUM_FONTS	1
	static NvBool fontsSplit[NUM_FONTS] = {1}; /* all are split */
	static const char *fontFiles[NUM_FONTS] = {
	    "courier+lucida_256.dds"};
	if (NVBFInitialize(NUM_FONTS, (const char**)fontFiles, fontsSplit, 0))
	{
		LOGW("Could not initialize NvBitFont");
		return false;
	}

	m_clockText = NVBFTextAlloc();
	NVBFTextSetFont(m_clockText, 1); // should look up by font file name.
	NVBFTextSetSize(m_clockText, 32);
    NVBFTextSetColor(m_clockText, NV_PC_PREDEF_WHITE);
	NVBFTextSetString(m_clockText, "000:00.00");

	m_uiInitialized = true;

	return true;
}

int Engine::getUIClickResponse(int iX, int iY, int action)
{
	if (mCurrentMode == MODE_GAMEPLAY)
		return UI_NO_HIT;

	if (action != AMOTION_EVENT_ACTION_UP)
		return UI_NO_HIT;

	if ((iX < (mUICenterX - mUIHalfSize)) ||
		(iX > (mUICenterX + mUIHalfSize)) ||
		(iY < (mUICenterY - mUIHalfSize)) ||
		(iY > (mUICenterY + mUIHalfSize)))
		return UI_NO_HIT;

	if (iY > mUICenterY)
	{
		return (iX < mUICenterX) ? UI_HIT_LEFT : UI_HIT_RIGHT;
	}

	return UI_HIT_OTHER;
}

bool Engine::checkUIHandledMotion(int iX, int iY, int action)
{
	if (mCurrentMode == MODE_GAMEPLAY)
		return false;

	if (mCurrentMode == MODE_STARTUP_SCREEN)
	{
		int response = getUIClickResponse(iX, iY, action);
		if (response != UI_NO_HIT)
			setAppUIMode(MODE_GAMEPLAY);
	} 
	else if (mCurrentMode == MODE_AUTOPAUSE) 
	{
		int response = getUIClickResponse(iX, iY, action);
		if (response == UI_HIT_LEFT)
			setAppUIMode(MODE_GAMEPLAY);
		else if (response == UI_HIT_RIGHT)
			setAppUIMode(MODE_QUIT_CONFIRM);
	}
	else if (mCurrentMode == MODE_QUIT_CONFIRM) 
	{
		int response = getUIClickResponse(iX, iY, action);
		if (response == UI_HIT_LEFT)
			setAppUIMode(MODE_AUTOPAUSE);
		else if (response == UI_HIT_RIGHT)
			ANativeActivity_finish(mApp->activity);
	}

	return true;
}

bool Engine::checkUIHandledKey(int code, int action, int& returnCode)
{
    if (code == AKEYCODE_BACK)
	{
		if (action == AKEY_EVENT_ACTION_DOWN)
		{
			returnCode = 1;
		}
		else if (mCurrentMode == MODE_GAMEPLAY)
		{
			setAppUIMode(MODE_AUTOPAUSE);
			returnCode = 1;
		}
		else if ((mCurrentMode == MODE_AUTOPAUSE) ||
				 (mCurrentMode == MODE_STARTUP_SCREEN))
		{
			setAppUIMode(MODE_QUIT_CONFIRM);
			returnCode = 1;
		}
		else if (mCurrentMode == MODE_QUIT_CONFIRM)
		{
			returnCode = 0;
		}
		else
		{
			returnCode = 1;
		}
	}
	else
    {
		returnCode = 0; 
    }

	return true;
}

void Engine::setAppUIMode(unsigned int mode)
{
	if (mCurrentMode != mode)
		requestForceRender();

	mCurrentMode = mode;
}

bool Engine::playLoopedMusic(bool play)
{
	return m_music ? m_music->play(play) : false;
}

bool Engine::checkWindowResized()
{
	if (mEgl.checkWindowResized())
	{
		mResizePending = true;
		requestForceRender();
		LOGI("Window size change %dx%d", mEgl.getWidth(), mEgl.getHeight()); 
		return true;
	}

	return false;
}

bool Engine::resizeIfNeeded()
{
	if (!mResizePending)
		return false;

	int w = mEgl.getWidth();
	int h = mEgl.getHeight();
	mGlobeApp->resize(w, h);
	mLastX = w / 2;
	mLastY = h / 2;

	NVBFSetScreenRes(w, h); // 16:9 ws as default.
	if (m_clockText)
	{
		int height = (w > h) ? (h / 16) : (w / 16);
		NVBFTextSetSize(m_clockText, height);
		NVBFTextCursorAlign(m_clockText, NVBF_ALIGN_LEFT, NVBF_ALIGN_TOP);
		NVBFTextCursorPos(m_clockText, 10, 10);
	}
	mResizePending = false;

	return true;
}

bool Engine::renderFrame(bool allocateIfNeeded)
{
    if (!mEgl.isReadyToRender(allocateIfNeeded))
        return false;

	if (!initUI())
	{
		LOGW("Could not initialize UI - assets may be missing!");
		ANativeActivity_finish(mApp->activity);
		return false;
	}

    // We've gotten this far, so EGL is ready for us.  Have we loaded our assets?
    // Note that we cannot use APP_STATUS_GLES_LOADED to imply that EGL is
    // ready to render.  We can have a valid context with all GLES resources loaded
    // into it but no surface and thus the context not bound.  These are semi-
    // independent states.
    if (!mGlobeApp)
	{
        if (!allocateIfNeeded)
            return false;

		mGlobeApp = new GlobeApp;
	}

	if (!mGlobeApp->isInitialized())
    {
        if (!allocateIfNeeded)
            return false;

		// If we are initializing and we have a new, known size, set that
		// before the init
        resizeIfNeeded();

        if (!mGlobeApp->init())
            return false;
    }

	resizeIfNeeded();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mGlobeApp->render((float)mTimeVal);

	if (m_clockText)
	{
		NVBFTextRenderPrep();
		NVBFTextRender(m_clockText);
		NVBFTextRenderDone();

		int mins = mTimeVal / 60;
		float secs = (float)mTimeVal - mins*60;
		char str[32];
		sprintf(str, "%03d:%05.2f", mins, secs);
		NVBFTextSetString(m_clockText, str);
	}

	// Render UI if needed
	if (m_uiModeTextures[mCurrentMode])
	{
		int w = mEgl.getWidth();
		int h = mEgl.getHeight();
		mUIHalfSize = 300;
		mUICenterX = w/2;
		mUICenterY = h/2;
		NvDrawRect::setScreenResolution(w, h);
		static const float s_white[4] = {1.0f, 1.0f, 1.0f, 0.7f};
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		mDrawRect->draw(m_uiModeTextures[mCurrentMode], 
			mUICenterX - mUIHalfSize, mUICenterY - mUIHalfSize,
			mUICenterX + mUIHalfSize, mUICenterY + mUIHalfSize, s_white);
		glDisable(GL_BLEND);
	}

	if (mForceRender > 0)
		mForceRender--;

    mEgl.swap();

    // A debug printout every 256 frames so we can see when we're
    // actively rendering and swapping
    if (!(mSwapCount++ & 0x00ff))
    {
		DEBUG("Swap count is %d", mSwapCount);
    }

    return true;
}

void Engine::updateFrame(bool interactible, long deltaTime)
{
	if (interactible)
	{
		// Each frame, we check to see if the window has resized.  While the
		// various events we get _should_ cover this, in practice, it appears
		// that the safest move across all platforms and OSes is to check at 
		// the top of each frame
		checkWindowResized();

		// We're "interactible", i.e. focused, so we play the music
		playLoopedMusic(true);

		// Time stands still when we're auto-paused, and we don't
		// automatically render
		if (getAppUIMode() == MODE_GAMEPLAY)
		{
			// only advance the "game" timer in gameplay mode
			advanceTime(deltaTime);

			// This will try to set up EGL if it isn't set up
			// When we first set up EGL completely, we also load our GLES resources
			// If these are already set up or we succeed at setting them all up now, then
			// we go ahead and render.
			renderFrame(true);
		}
		else if (isForcedRenderPending()) 
		{
			// forced rendering when needed for UI, etc
			renderFrame(true);
		}
	}
	else
	{
		// Even if we are not interactible, we may be visible, so we
		// HAVE to do any forced renderings if we can.  We must also
		// check for resize, since that may have been the point of the
		// forced render request in the first place!
		if (isForcedRenderPending() && mEgl.isReadyToRender(false)) 
		{
			checkWindowResized();
			renderFrame(false);
		}

		// We're not "interactible", so we'd better stop the music immediately
		playLoopedMusic(false);
	}
}

int Engine::handleInput(AInputEvent* event)
{
    //We only handle motion events (touchscreen) and key (button/key) events
	int32_t eventType = AInputEvent_getType(event);

	if (eventType == AINPUT_EVENT_TYPE_MOTION)
	{
		int32_t edgeFlags = AMotionEvent_getEdgeFlags(event);

		if(edgeFlags == AMOTION_EVENT_EDGE_FLAG_NONE)
		{
    		//The current motion event (touch) is within screen bounds
			int32_t action = AMOTION_EVENT_ACTION_MASK &
        					 AMotionEvent_getAction((const AInputEvent*)event);
    		int32_t iX = AMotionEvent_getX(event, 0);
    		int32_t iY = AMotionEvent_getY(event, 0);

			if (checkUIHandledMotion(iX, iY, action))
				return 0;

			if (AMOTION_EVENT_ACTION_UP == action)
			{
        		mGlobeApp->pauseAnim(false);
        		mLastX = mEgl.getWidth() / 2;
        		mLastY = mEgl.getHeight() / 2;
			}
			else if (AMOTION_EVENT_ACTION_DOWN == action)
			{
				mGlobeApp->pauseAnim(true);
				mLastX = iX;
				mLastY = iY;
			}
			else if (AMOTION_EVENT_ACTION_MOVE == action)
			{
				// Empirical value (how many degrees to rotate when dragging finger on screen so
				// it looks like finger is "anchored"
				const static float DEGS_PER_PIXEL_DRAG = 0.25f;

        		float fRotX = (float)(iY - mLastY) *
        						DEGS_PER_PIXEL_DRAG;
				float fRotY = (float)(iX - mLastX) *
        						DEGS_PER_PIXEL_DRAG;
				mGlobeApp->addRotation(fRotX, fRotY);
				mGlobeApp->positionCamera();
				mLastX = iX;
				mLastY = iY;
			}

			return 1;
		}
		else // if(edgeFlags == AMOTION_EVENT_EDGE_FLAG_NONE)
		{
    		//The current motion event (touch) has reached an edge.
    		//We process it the same way as if we were touching and we had lifted
    		//the finger off the screen
			mGlobeApp->pauseAnim(false);
			mLastX = mEgl.getWidth() / 2;
			mLastY = mEgl.getHeight() / 2;

    		return 1;
		}
	} else if (eventType == AINPUT_EVENT_TYPE_KEY) {
		int32_t action = AKeyEvent_getAction((const AInputEvent*)event);
		int32_t code = AKeyEvent_getKeyCode((const AInputEvent*)event);

		int returnCode = 1;
		// pass to the on-screen UI, which will return what our "handled" code should be
		// or will return false to us, which indicates that the UI is allowing default
		// behavior for the key
		if (checkUIHandledKey(code, action, returnCode))
			return returnCode;
	}

    return 0;
}

void Engine::handleCommand(int cmd)
{
    switch (cmd)
    {
		// The window is being shown, get it ready.
		// Note that on ICS, the EGL size will often be correct for the new size here
		// But on HC it will not be.  We need to defer checking the new res until the
		// first render with the new surface!
        case APP_CMD_INIT_WINDOW:
        case APP_CMD_WINDOW_RESIZED:
			mEgl.setWindow(mApp->window);
			requestForceRender();
        	break;

        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
			mEgl.setWindow(NULL);
			break;

        case APP_CMD_GAINED_FOCUS:
			requestForceRender();
			break;

        case APP_CMD_LOST_FOCUS:
        	// Move out of gameplay mode if we are in it.  But if we are
			// in another dialog mode, leave it as-is
            if (mCurrentMode == MODE_GAMEPLAY)
				setAppUIMode(MODE_AUTOPAUSE);
			requestForceRender();
            break;

		case APP_CMD_PAUSE:
            if (mCurrentMode == MODE_GAMEPLAY)
				setAppUIMode(MODE_AUTOPAUSE);
			requestForceRender();
			break;

		// ICS does not appear to require this, but on GB phones,
		// not having this causes rotation changes to be delayed or
		// ignored when we're in a non-rendering mode like autopause.
		// The forced renders appear to allow GB to process the rotation
		case APP_CMD_CONFIG_CHANGED:
			requestForceRender();
			break;
    }
}

/**
 * Process the next input event.
 */
int32_t Engine::handleInputThunk(struct android_app* app, AInputEvent* event)
{
    Engine* engine = (Engine*)app->userData;
	if (!engine)
		return 0;

	return engine->handleInput(event);
}

/**
 * Process the next main command.
 */
void Engine::handleCmdThunk(struct android_app* app, int32_t cmd)
{
    Engine* engine = (Engine*)app->userData;
	if (!engine)
		return;

	engine->handleCommand(cmd);
}
