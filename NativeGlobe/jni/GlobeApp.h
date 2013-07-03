//----------------------------------------------------------------------------------
// File:        native_globe/jni/GlobeApp.h
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

#ifndef GLOBEAPP_H_
#define GLOBEAPP_H_

#include <GLES2/gl2.h>

/**
 * Class that holds all of the application's state data. Its methods
 * need to be called from a driving application loop, such as NativeActivity
 */
class GlobeApp
{
protected:
	//Protected class members

	//User-defined global parameters
	//--DEFINE IN GlobeApp::resetValues
	GLuint   m_uiVertSlices; //Must be multiple of 2, up to 360
	GLfloat  m_fGlobeRadius;
	GLfloat  m_fCloudHeight;
	GLfloat  m_fRotSpeed;    // In rotations for second
	GLfloat  m_fFOVy, m_fZNear, m_fZFar;
	GLfloat  m_fvEyePos[3];
	GLfloat  m_fvEarthPos[3];
	GLfloat  m_fvSunPos[3];
	GLfloat  m_fvAmbColor[3];
	GLclampf m_fvBgColor[3];
	GLfloat  m_fHeightScale;
	GLfloat  m_fParallaxScale, m_fParallaxBias;

	//Other state data
	int32_t m_iWinWidth, m_iWinHeight;

	//Geometry Globe
	GLuint m_uiNumVertices, m_uiNumIndices;
	GLuint m_uiRawVBO;
	GLuint m_uiGlobeVBO;
	GLuint m_uiCloudVBO;
	GLuint m_uiIBO;

	//Geometry Lights
	GLuint m_uiLightNumVerts, m_uiLightNumIndices;
	GLuint m_uiLightVBO;
	GLuint m_uiLightIBO;


	//Textures
	GLuint m_uivTextures[4];     // color, height map, clouds and city lights

	GLuint m_renderTexture;

	//Globe Programs
	GLuint m_uivGlobePrograms[8]; // raw data * normal mapping * parallax mapping
	GLuint m_uivCloudPrograms[2]; // raw data

	//Globe Uniforms
	GLint m_iMVPMatUnifLocGlobe[8];
	GLint m_iSunPosObjUnifLocGlobe[8];
	GLint m_iEyePosObjUnifLocGlobe[8];
	GLint m_iRadiusUnifLocGlobe[8];
	GLint m_iMVPMatUnifLocCloud[2];
	GLint m_iSunPosObjUnifLocCloud[2];
	GLint m_iEyePosObjUnifLocCloud[2];
	GLint m_iRadiusUnifLocCloud[2];

	//Matrices
	GLfloat m_fmModel[4][4], m_fmInvModel[4][4];
	GLfloat m_fmView[4][4];
	GLfloat m_fmProj[4][4];
	GLfloat m_fmViewProj[4][4];

	//Lights
	GLfloat m_fmLightModel[4][4];
	GLfloat  m_fvLightPos[3];

	GLfloat m_fvUp[3];
	GLfloat m_fvCurrEyePos[3];
	GLfloat m_fCurrRotAngle;
	float m_fCamRotXAngle, m_fCamRotYAngle;
	float m_fRunningAnimTime;
	float m_fStartTime;
	int m_iCurrFrame;
	unsigned int m_uiCurrGlobeProgIdx, m_uiCurrCloudProgIdx;
	bool m_bRawData, m_bParallaxMapping, m_bNormalMapping;
	bool m_bShowClouds;
	bool m_bPauseAnim;
	bool m_bInitialized;

	//Light Prepass
	//Shader Programs
	GLuint m_shprogGeometry;
	GLuint m_shprogLight;
	GLuint m_shprogMaterial;

	//Attributes
	/*
	attribute vec3 pos_attr; // Position
	attribute vec3 nor_attr; // Normal
	attribute vec3 tan_attr; // Tangent
	attribute vec3 bin_attr; // Binormal
	attribute vec2 tex_attr; // Texcoords
	*/

	//Uniforms
	/*
	uniform mat4 MVPMat;    // Model-View-Projection matrix
	uniform mat4 ModelMat; //ModelMatrix
	uniform mat4 ModelViewMat

	uniform sampler2D NormalMapTex;
	*/
	GLint m_iMVPMatUnifLoc;
	GLint m_iModelViewMatUnifLoc;
	GLint m_iModelMatUnifLoc;

	//Lights
	GLint m_iLightModelViewMatUnifLoc;
	GLint m_iLightPosViewLoc;
	GLint m_ilightColorLoc;

	//Protected methods
	bool resetValues(void);
	bool initTexturesGL(void);
	bool initShadersGL(void);
	bool initMeshesGL(void);
	bool initStateGL(void);
	bool resizeGL(void);
	bool renderGL(const float current_time);
	bool positionCameraGL(void);
	bool switchProgramGL(void);
	bool cleanupGL(void);


	bool initGeometryShaders(void);
	bool initLightShaders(void);
	bool initLightGeometry(void);
	bool initMaterialShader(void);
	bool drawNormalDepth(void);
	bool renderGeometry(void);
	bool renderLights(void);
	bool renderMaterial(void);

public:
	//Ctor/Dtor
	GlobeApp(void);
	virtual ~GlobeApp(void);

	//Public methods
	bool init();
	bool changeGeometryRes(unsigned int vertSlices);
	bool resize(const int width, const int height);
	bool render(const float current_time);
	bool toggleClouds(void);
	bool toggleRawData(void);
	bool toggleNormalMapping(void);
	bool toggleParallaxMapping(void);
	bool positionCamera(void);
	bool cleanup(void);
	bool restart();

	inline unsigned int getWinWidth(void)  const {return m_iWinWidth;}
	inline unsigned int getWinHeight(void) const {return m_iWinHeight;}
	inline void pauseAnim(bool doPause)   {m_bPauseAnim = doPause;}
	inline void setRotation(const float rotX, const float rotY)
		{m_fCamRotXAngle  = rotX; m_fCamRotYAngle  = rotY;}
	inline void addRotation(const float rotX, const float rotY)
		{m_fCamRotXAngle += rotX; m_fCamRotYAngle += rotY;}
	inline bool isInitialized() { return m_bInitialized; }
};

#endif /* GLOBEAPP_H_ */
