//----------------------------------------------------------------------------------
// File:        native_globe/jni/NvSLESPlayer.h
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

#ifndef _NV_SLES_PLAYER_H
#define _NV_SLES_PLAYER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <sys/types.h>
#include <android/asset_manager.h>

class NvSLESPlayer {
public:
	// Creates an OpenSL ES player for the given file
	// The AssetManager instance can be queried from NativeActivity
	// The filename is assumed to be in the app's APK assets
	// Returns NULL on failure
	static NvSLESPlayer* create(AAssetManager* assetManager, const char* filename);
	~NvSLESPlayer();

	// returns true if the audio is playing, false if not
	bool isPlaying() { return mIsPlaying; }
	
	// starts playback on true, stops on false
	// Playback is currently looped
	bool play(bool play);

protected:
	NvSLESPlayer()
	{
		mIsPlaying = false;

		m_engineObject = NULL;
		m_engineEngine = NULL;
		m_outputMixObject = NULL;

		m_playerObject = NULL;
		m_playerPlay = NULL;
		m_playerSeek = NULL;
		m_playerMuteSolo = NULL;
		m_playerVolume = NULL;
	}

	bool mIsPlaying;

	SLObjectItf m_engineObject;
	SLEngineItf m_engineEngine;
	SLObjectItf m_outputMixObject;

	SLObjectItf m_playerObject;
	SLPlayItf m_playerPlay;
	SLSeekItf m_playerSeek;
	SLMuteSoloItf m_playerMuteSolo;
	SLVolumeItf m_playerVolume;
};


#endif // _NV_SLES_PLAYER_H

