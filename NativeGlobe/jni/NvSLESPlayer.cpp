//----------------------------------------------------------------------------------
// File:        native_globe/jni/NvSLESPlayer.cpp
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
#include "NvSLESPlayer.h"
#include <android/log.h>

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG,  \
											 "NvSLESPlayer", \
											 __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR,  \
											 "NvSLESPlayer", \
											 __VA_ARGS__))

#define STATUS_ERROR_RETURN(str) \
	if (SL_RESULT_SUCCESS == result) { \
		LOGD("Success: %s (%s:%d)", str, __FUNCTION__, __LINE__); \
	} else { \
		LOGE("Failure: %s = 0x%08x (%s:%d)", str, result, __FUNCTION__, __LINE__); \
		return NULL; \
	}


NvSLESPlayer* NvSLESPlayer::create(AAssetManager* assetManager, const char* filename)
{
    SLresult result;

    // create engine
	SLObjectItf engineObject;
    result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
    STATUS_ERROR_RETURN("slCreateEngine");

    // realize the engine
    result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
	STATUS_ERROR_RETURN("engineObject::Realize");

    // get the engine interface, which is needed in order to create other objects
	SLEngineItf engineEngine;
    result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
	STATUS_ERROR_RETURN("GetInterface::SL_IID_ENGINE");

    // create output mix
	SLObjectItf outputMixObject;
    result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 0, NULL, NULL);
    STATUS_ERROR_RETURN("CreateOutputMix");

    // realize the output mix
    result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
	STATUS_ERROR_RETURN("outputMixObject::Realize");

    AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_UNKNOWN);

    // the asset might not be found
    if (!asset)
	{
		LOGE("Failure: AAssetManager_open: %p, %s", assetManager, filename);
		return NULL;
	}

    // open asset as file descriptor
    off_t start, length;
    int fd = AAsset_openFileDescriptor(asset, &start, &length);
    if (fd < 0)
	{
		LOGE("Failure: AAsset_openFileDescriptor: %d", fd);
		return NULL;
	}

    AAsset_close(asset);

    // configure audio source
    SLDataLocator_AndroidFD loc_fd = {SL_DATALOCATOR_ANDROIDFD, fd, start, length};
    SLDataFormat_MIME format_mime = {SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED};
    SLDataSource audioSrc = {&loc_fd, &format_mime};

    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};

    // create audio player
	SLObjectItf playerObject;
    const SLInterfaceID ids[3] = {SL_IID_SEEK, SL_IID_MUTESOLO, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &playerObject, &audioSrc, &audioSnk,
            3, ids, req);
    STATUS_ERROR_RETURN("CreateAudioPlayer");

    // realize the player
    result = (*playerObject)->Realize(playerObject, SL_BOOLEAN_FALSE);
	STATUS_ERROR_RETURN("playerObject::Realize");

    // get the play interface
	SLPlayItf playerPlay;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_PLAY, &playerPlay);
	STATUS_ERROR_RETURN("GetInterface::SL_IID_PLAY");

    // get the seek interface
	SLSeekItf playerSeek;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_SEEK, &playerSeek);
    STATUS_ERROR_RETURN("GetInterface::SL_IID_SEEK");

    // get the mute/solo interface
	SLMuteSoloItf playerMuteSolo;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_MUTESOLO, &playerMuteSolo);
    STATUS_ERROR_RETURN("GetInterface::SL_IID_MUTESOLO");

    // get the volume interface
	SLVolumeItf playerVolume;
    result = (*playerObject)->GetInterface(playerObject, SL_IID_VOLUME, &playerVolume);
    STATUS_ERROR_RETURN("GetInterface::SL_IID_VOLUME");

    // enable whole file looping
    result = (*playerSeek)->SetLoop(playerSeek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
    STATUS_ERROR_RETURN("SetLoop");

	NvSLESPlayer* thiz = new NvSLESPlayer;
	thiz->m_engineObject = engineObject;
	thiz->m_engineEngine = engineEngine;
	thiz->m_outputMixObject = outputMixObject;

	thiz->m_playerObject = playerObject;
	thiz->m_playerPlay = playerPlay;
	thiz->m_playerSeek = playerSeek;
	thiz->m_playerMuteSolo = playerMuteSolo;
	thiz->m_playerVolume = playerVolume;

	return thiz;
}

NvSLESPlayer::~NvSLESPlayer()
{
	if (m_playerObject)
	    (*m_playerObject)->Destroy(m_playerObject);
    
	if (m_outputMixObject)
		(*m_outputMixObject)->Destroy(m_outputMixObject);

	if (m_engineObject)
		(*m_engineObject)->Destroy(m_engineObject);

	m_engineObject = NULL;
	m_engineEngine = NULL;
	m_outputMixObject = NULL;

	m_playerObject = NULL;
	m_playerPlay = NULL;
	m_playerSeek = NULL;
	m_playerMuteSolo = NULL;
	m_playerVolume = NULL;
}

bool NvSLESPlayer::play(bool play)
{
    SLresult result;

	if (play == mIsPlaying)
		return true;

    // make sure the asset audio player was created
    if (m_playerPlay) {

        // set the player's state
        result = (*m_playerPlay)->SetPlayState(m_playerPlay, play ?
            SL_PLAYSTATE_PLAYING : SL_PLAYSTATE_PAUSED);

		if (SL_RESULT_SUCCESS == result)
		{
			mIsPlaying = play;
			return true;
		}
		else
		{
			return false;
		}
    }

	return false;
}


