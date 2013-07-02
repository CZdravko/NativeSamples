//----------------------------------------------------------------------------------
// File:        native_globe/jni/GlobeApp.cpp
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

#include "GlobeApp.h"

#include "stdio.h"
#include "stdlib.h"
#include <android/log.h>
#include <GLES2/gl2.h>
#include <nv_math/nv_matrix.h>
#include <nv_math/nv_quat.h>
#include <nv_shader/nv_shader.h>

#include "GlobeDefines.h"
#include "GlobeUtil.h"

///////////////////////////////////////////////////////////////////////////////
//DEFINITIONS

//This flag determines whether we will use the time to compute the appropriate
//amount of the earth's rotation, or if we will rotate a set amount of time per
//rendered frame (useful for debugging)
#define TIME_DRIVEN_ANIMATION 1

#define DEGS_PER_REV (360.0F)

#define GLOBE_SHADER "globe"
#define CLOUD_SHADER "cloud"

#define GLOBE_SHADER_SOURCE "\n\
#define USING_RAW_DATA   (%d)\n\
#define PARALLAX_MAPPING (%d)\n\
#define NORMAL_MAPPING   (%d)\n\
#define AMBIENT_COLOR    (vec3(%f, %f, %f))\n\
#define PARALLAX_SCALE   (%f)\n\
#define PARALLAX_BIAS    (%f)\n"

#define CLOUD_SHADER_SOURCE "\n\
#define USING_RAW_DATA   (%d)\n"

#define      COLOR_TEXTURE_FILENAME "earth_color.R5G6B5.mp3"
#define  HEIGHTMAP_TEXTURE_FILENAME "earth_heightmap.RGBA8.mp3"
#define CITYLIGHTS_TEXTURE_FILENAME "earth_citylights.R5G6B5.mp3"
#define     CLOUDS_TEXTURE_FILENAME "earth_clouds.R5G6B5.mp3"

//Sizes in number of component, strides in bytes (stride = size * sizeof(type))
#define VERTEX_RAWDATA_SIZE     ((GLuint)2U)
#define VERTEX_POSITION_SIZE    ((GLuint)3U)
#define VERTEX_NORMAL_SIZE      ((GLuint)3U)
#define VERTEX_TANGENT_SIZE     ((GLuint)3U)
#define VERTEX_BINORMAL_SIZE    ((GLuint)3U)
#define VERTEX_TEXCOORD_SIZE    ((GLuint)2U)
#define VERTEX_ATTRIBUTE_SIZE   (VERTEX_POSITION_SIZE + \
								 VERTEX_TANGENT_SIZE  + \
								 VERTEX_NORMAL_SIZE   + \
								 VERTEX_BINORMAL_SIZE + \
								 VERTEX_TEXCOORD_SIZE)

///////////////////////////////////////////////////////////////////////////////
//PROTECTED METHODS

template<typename T>
void zeroMemoryA(T data) {
	memset(data, 0, sizeof(T));
}

/**
 * SET YOUR INITIAL VALUES HERE
 */
bool GlobeApp::resetValues() {
	//User-defined global parameters
	//--CHANGE VALUES HERE
	m_fRotSpeed = (GLfloat) 0.100F; /// in revolutions per second
	m_fGlobeRadius = (GLfloat) 1.000F;
	m_fCloudHeight = (GLfloat) 0.050F;
	m_fFOVy = (GLfloat) 45.000F;
	m_fZNear = (GLfloat) 0.010F;
	m_fZFar = (GLfloat) 10.000F;
	m_fvEyePos[0] = (GLfloat) 0.000F;
	m_fvEyePos[1] = (GLfloat) 0.000F;
	m_fvEyePos[2] = (GLfloat) 5.000F;
	m_fvEarthPos[0] = (GLfloat) 0.000F;
	m_fvEarthPos[1] = (GLfloat) 0.000F;
	m_fvEarthPos[2] = (GLfloat) 0.000F;
	m_fvSunPos[0] = (GLfloat) -4.000F;
	m_fvSunPos[1] = (GLfloat) 4.000F;
	m_fvSunPos[2] = (GLfloat) 6.000F;
	m_fvAmbColor[0] = (GLfloat) 0.005F;
	m_fvAmbColor[1] = (GLfloat) 0.005F;
	m_fvAmbColor[2] = (GLfloat) 0.010F;
	m_fvBgColor[0] = (GLclampf) 0.005F;
	m_fvBgColor[1] = (GLclampf) 0.005F;
	m_fvBgColor[2] = (GLclampf) 0.000F;
	m_fHeightScale = (GLfloat) 3.000F;
	m_fParallaxScale = (GLfloat) 0.010F;
	m_fParallaxBias = (GLfloat) 0.000F;
	m_bRawData = false;
	m_bParallaxMapping = true;
	m_bNormalMapping = true;
	m_bShowClouds = true;

	//Lights
	m_fvLightPos[0] = (GLfloat) -1.000F;
	m_fvLightPos[1] = (GLfloat) 1.000F;
	m_fvLightPos[2] = (GLfloat) 1.000F;

	//Other state data
	//--DO NOT CHANGE
	m_uiNumVertices = GLUINT_ZERO;
	m_uiNumIndices = GLUINT_ZERO;
	m_uiGlobeVBO = GLUINT_ZERO;
	m_uiCloudVBO = GLUINT_ZERO;
	m_uiRawVBO = GLUINT_ZERO;
	m_uiIBO = GLUINT_ZERO;

	zeroMemoryA(m_uivTextures);
	zeroMemoryA(m_uivGlobePrograms);
	zeroMemoryA(m_uivCloudPrograms);

	m_uiCurrGlobeProgIdx = GLUINT_ZERO;
	m_uiCurrCloudProgIdx = GLUINT_ZERO;

	m_fvUp[0] = GLFLOAT_ZERO;
	m_fvUp[1] = GLFLOAT_ONE;
	m_fvUp[2] = GLFLOAT_ZERO;
	for (int k = 0; k < 3; k++) {
		m_fvCurrEyePos[k] = m_fvEyePos[k];
	}

	//Used for FPS computations
	m_fStartTime = GLFLOAT_ZERO;
	m_iCurrFrame = GLINT_ZERO;

	//Used for Earth's rotation animation
	m_fCurrRotAngle = GLFLOAT_ZERO;
	m_bPauseAnim = false;
	m_fRunningAnimTime = 0.0F;

	//Used for computing the position of the camera in world space
	m_fCamRotXAngle = 0.0F;
	m_fCamRotYAngle = 0.0F;

	NvBuildTranslateMatf(m_fmModel, m_fvEarthPos[0], m_fvEarthPos[1],
			m_fvEarthPos[2]);
	NvBuildTranslateMatf(m_fmInvModel, -m_fvEarthPos[0], -m_fvEarthPos[1],
			-m_fvEarthPos[2]);
	NvBuildIdentityMatf(m_fmView);
	NvBuildIdentityMatf(m_fmProj);
	NvBuildIdentityMatf(m_fmViewProj);

	NvBuildTranslateMatf(m_fmLightModel, m_fvLightPos[0], m_fvLightPos[1],
			m_fvLightPos[2]);

	m_bInitialized = true;

	return true;
}

/**
 *
 */
bool GlobeApp::initTexturesGL(void) {
	bool tex0 = createHHDDSTextureGL(m_uivTextures[0], 0, GL_TEXTURE0, true,
			true, COLOR_TEXTURE_FILENAME);
	bool tex1 = createNormalMapGL(m_uivTextures[1], 1, GL_TEXTURE1,
			m_fHeightScale, HEIGHTMAP_TEXTURE_FILENAME);
	bool tex2 = createHHDDSTextureGL(m_uivTextures[2], 2, GL_TEXTURE2, true,
			true, CITYLIGHTS_TEXTURE_FILENAME);
	bool tex3 = createHHDDSTextureGL(m_uivTextures[3], 3, GL_TEXTURE3, true,
			true, CLOUDS_TEXTURE_FILENAME);
	return tex0 && tex1 && tex2 && tex3;
}

/**
 *
 */
bool GlobeApp::initShadersGL(void) {
	char ucBuffer[1024];

	// 8 shaders for globe mesh:
	// * 0-3 using interleaved data in VBO, 4-7 using raw data VBO and vtx math
	// * 2-3, 6-7 have parallax mapping, the rest does not
	// * even numbers use regular normals, odds use normal mapping
	// Using bits of integer k to classify shaders
	for (int k = 0; k < (int) dimof(m_uivGlobePrograms); k++) {
		//Fill in the #defines to be included in the shader source code before
		//what is in the text file
		memset(ucBuffer, '\0', 1024);
		sprintf(ucBuffer, GLOBE_SHADER_SOURCE, (k >> 2) & 1, (k >> 1) & 1,
				k & 1, m_fvAmbColor[0], m_fvAmbColor[1], m_fvAmbColor[2],
				m_fParallaxScale, m_fParallaxBias);
		m_uivGlobePrograms[k] = nv_load_program(GLOBE_SHADER, ucBuffer);

		if (k >= 4) {
			glBindAttribLocation(m_uivGlobePrograms[k], 0, "raw_attr");
		} else {
			glBindAttribLocation(m_uivGlobePrograms[k], 1, "pos_attr");
			glBindAttribLocation(m_uivGlobePrograms[k], 2, "nor_attr");
			glBindAttribLocation(m_uivGlobePrograms[k], 3, "tan_attr");
			glBindAttribLocation(m_uivGlobePrograms[k], 4, "bin_attr");
			glBindAttribLocation(m_uivGlobePrograms[k], 5, "tex_attr");
		}
		glLinkProgram(m_uivGlobePrograms[k]);

		glUseProgram(m_uivGlobePrograms[k]);

		//Uniforms that need to be updated every frame
		m_iMVPMatUnifLocGlobe[k] = glGetUniformLocation(m_uivGlobePrograms[k],
				"MVPMat");
		m_iSunPosObjUnifLocGlobe[k] = glGetUniformLocation(
				m_uivGlobePrograms[k], "SunPosObj");
		m_iEyePosObjUnifLocGlobe[k] = glGetUniformLocation(
				m_uivGlobePrograms[k], "EyePosObj");
		m_iRadiusUnifLocGlobe[k] = glGetUniformLocation(m_uivGlobePrograms[k],
				"Radius");

		//Uniforms that need to be set once
		glUniform1i(glGetUniformLocation(m_uivGlobePrograms[k], "ColorTex"),
				(GLint) 0);
		glUniform1i(glGetUniformLocation(m_uivGlobePrograms[k], "NormalMapTex"),
				(GLint) 1);
		glUniform1i(
				glGetUniformLocation(m_uivGlobePrograms[k], "CityLightsTex"),
				(GLint) 2);
	}

	//Uniforms that need to be updated every frame
	for (int k = 0; k < (int) dimof(m_uivCloudPrograms); k++) {
		//Fill in the #defines to be included in the shader source code before
		//what is in the text file
		memset(ucBuffer, '\0', 1024);
		sprintf(ucBuffer, CLOUD_SHADER_SOURCE, k);
		m_uivCloudPrograms[k] = nv_load_program(CLOUD_SHADER, ucBuffer);

		if (k > 0) {
			glBindAttribLocation(m_uivCloudPrograms[k], 0, "raw_attr");
		} else {
			glBindAttribLocation(m_uivCloudPrograms[k], 1, "pos_attr");
			glBindAttribLocation(m_uivCloudPrograms[k], 2, "nor_attr");
			glBindAttribLocation(m_uivCloudPrograms[k], 3, "tan_attr");
			glBindAttribLocation(m_uivCloudPrograms[k], 4, "bin_attr");
			glBindAttribLocation(m_uivCloudPrograms[k], 5, "tex_attr");
		}
		glLinkProgram(m_uivCloudPrograms[k]);

		glUseProgram(m_uivCloudPrograms[k]);

		//Uniforms that need to be updated every frame
		m_iMVPMatUnifLocCloud[k] = glGetUniformLocation(m_uivCloudPrograms[k],
				"MVPMat");
		m_iSunPosObjUnifLocCloud[k] = glGetUniformLocation(
				m_uivCloudPrograms[k], "SunPosObj");
		m_iEyePosObjUnifLocCloud[k] = glGetUniformLocation(
				m_uivCloudPrograms[k], "EyePosObj");
		m_iRadiusUnifLocCloud[k] = glGetUniformLocation(m_uivCloudPrograms[k],
				"Radius");

		//Uniforms that need to be set once
		glUniform1i(glGetUniformLocation(m_uivCloudPrograms[k], "CloudsTex"),
				(GLint) 3);
	}

	glUseProgram(0);

	return true;
}

/**
 *
 */
bool GlobeApp::initMeshesGL(void) {
	FullVertex *fvGlobeAttribs = NULL, *fvCloudAttribs = NULL;
	RawVertex *fvRawAttribs = NULL;
	GLushort *usvElements = NULL;

	//Get information for the globe VBO, the raw VBO and the IBO
	if (!(createSphere(&fvGlobeAttribs, &fvRawAttribs, &usvElements,
			&(m_uiNumVertices), &(m_uiNumIndices), m_fGlobeRadius,
			m_uiVertSlices)) || !usvElements)
		return false;

	//This time we only need data for the cloud VBO
	if (!(createSphere(&fvCloudAttribs, NULL, NULL, NULL, NULL,
			m_fGlobeRadius + m_fCloudHeight, m_uiVertSlices)))
		return false;

	//Generate buffer objects
	glGenBuffers((GLsizei) 1, &(m_uiGlobeVBO));
	glGenBuffers((GLsizei) 1, &(m_uiCloudVBO));
	glGenBuffers((GLsizei) 1, &(m_uiRawVBO));
	glGenBuffers((GLsizei) 1, &(m_uiIBO));

	//Create globe vertex attribute VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_uiGlobeVBO);
	glBufferData(GL_ARRAY_BUFFER, m_uiNumVertices * sizeof(FullVertex),
			fvGlobeAttribs, GL_STATIC_DRAW);

	//Create cloud vertex attribute VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_uiCloudVBO);
	glBufferData(GL_ARRAY_BUFFER, m_uiNumVertices * sizeof(FullVertex),
			fvCloudAttribs, GL_STATIC_DRAW);

	//Create raw vertex attribute VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_uiRawVBO);
	glBufferData(GL_ARRAY_BUFFER, m_uiNumVertices * sizeof(RawVertex),
			fvRawAttribs, GL_STATIC_DRAW);

	//Create IBO (for all VBOs)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_uiNumIndices * sizeof(GLushort),
			usvElements, GL_STATIC_DRAW);

	//3) Cleanup
	if (fvGlobeAttribs)
		delete fvGlobeAttribs;
	if (fvCloudAttribs)
		delete fvCloudAttribs;
	if (fvRawAttribs)
		delete fvRawAttribs;
	if (usvElements)
		delete usvElements;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

/**
 *
 */
bool GlobeApp::initStateGL(void) {
	DEBUG("INIT:\tGL_VENDOR: %s", glGetString(GL_VENDOR));
	DEBUG("INIT:\tGL_VERSION: %s", glGetString(GL_VERSION));
	DEBUG("INIT:\tGL_RENDERER: %s", glGetString(GL_RENDERER));
	DEBUG("INIT:\tGL_EXTENSIONS:");

	const GLubyte* ext = glGetString(GL_EXTENSIONS);
	char *extTok = strdup((const char*) ext);

	extTok = strtok(extTok, " ");
	while (extTok != NULL) {
		DEBUG("INIT:\t\t%s", extTok);
		extTok = strtok(NULL, " ");
	}

	glEnable(GL_DEPTH_TEST); //depth function is GL_LESS
	glEnable(GL_CULL_FACE); //cull face is GL_BACK
	glEnable(GL_BLEND); //we are leaving blending on
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(m_fvBgColor[0], m_fvBgColor[1], m_fvBgColor[2],
			(GLclampf) 1.0f);

	return true;
}

/**
 *const int width, const int height
 */
bool GlobeApp::resizeGL() {
	glViewport((GLint) 0, (GLint) 0, (GLsizei) (m_iWinWidth),
			(GLsizei) (m_iWinHeight));

	GLfloat fRightEdge = (GLfloat) (m_fZNear * tan(m_fFOVy * M_PI / 360.0));
	GLfloat fAspectRatio = ((GLfloat) m_iWinHeight) / ((GLfloat) m_iWinWidth);

	// Fill the smaller dimension of the screen.
	if (fAspectRatio > 1.0f) {
		NvBuildFrustumMatf(m_fmProj, -fRightEdge / fAspectRatio,
				fRightEdge / fAspectRatio, -fRightEdge, fRightEdge, m_fZNear,
				m_fZFar);
	} else {
		NvBuildFrustumMatf(m_fmProj, -fRightEdge, fRightEdge,
				-fRightEdge * fAspectRatio, fRightEdge * fAspectRatio, m_fZNear,
				m_fZFar);
	}

	//Update View-Projection matrix (when changing camera pos. or viewport)
	NvMultMatf(m_fmViewProj, m_fmProj, m_fmView);

	DEBUG("RESIZE:\tAspect ratio: %.2f", fAspectRatio);
	DEBUG("GlobalAppState::update:\t(zNear, zFar): (%.2f, %.2f)",
			m_fZNear, m_fZFar);
	DEBUG("RESIZE:\tViewport: (%d, %d)", m_iWinWidth, m_iWinHeight);

	return switchProgramGL();
}

/**
 *
 */
bool GlobeApp::renderGL(const float current_time) {
	GLfloat fmCurrModel[4][4], fmInvCurrModel[4][4];
	GLfloat fmMVP[4][4];
	GLfloat m_fvSunPosObj[3], m_fvEyePosObj[3];

	//We use the same IBO for all VBOs, bind it once
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBO);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_uivTextures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uivTextures[1]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uivTextures[2]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_uivTextures[3]);
	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);

	//2. MODEL MATRIX
	//2.1. Create current model matrix (from stored matrix with translation)
	NvMultRotYDegMatf(fmCurrModel, m_fmModel, m_fCurrRotAngle);
	//2.1. Store inverted copy of current model matrix
	NvBuildRotYDegMatf(fmInvCurrModel, -(m_fCurrRotAngle));
	NvMultMatf(fmInvCurrModel, m_fmInvModel, fmInvCurrModel);

	//3. MODEL-VIEW-PROJECTION MATRIX
	NvMultMatf(fmMVP, m_fmViewProj, fmCurrModel);

	//4. SUN POSITION (world space -> object space)
	NvTransformPointf(m_fvSunPosObj, fmInvCurrModel, m_fvSunPos);

	//5. EYE POSITION (world space -> object space)
	NvTransformPointf(m_fvEyePosObj, fmInvCurrModel, m_fvCurrEyePos);

	//6. DRAW GLOBE (in VBOs)
	glUseProgram(m_uivGlobePrograms[m_uiCurrGlobeProgIdx]);

	glUniformMatrix4fv(m_iMVPMatUnifLocGlobe[m_uiCurrGlobeProgIdx], (GLsizei) 1,
			GL_FALSE, &(fmMVP[0][0]));
	glUniform3fv(m_iSunPosObjUnifLocGlobe[m_uiCurrGlobeProgIdx], (GLsizei) 1,
			m_fvSunPosObj);
	glUniform3fv(m_iEyePosObjUnifLocGlobe[m_uiCurrGlobeProgIdx], (GLsizei) 1,
			m_fvEyePosObj);
	glUniform1f(m_iRadiusUnifLocGlobe[m_uiCurrGlobeProgIdx], m_fGlobeRadius);

	if (m_uiCurrGlobeProgIdx < 4U) {
		glBindBuffer(GL_ARRAY_BUFFER, m_uiGlobeVBO);

		GLfloat *ptr = NULL;
		glVertexAttribPointer(1U, VERTEX_POSITION_SIZE, GL_FLOAT, GL_FALSE,
				sizeof(FullVertex), ptr);
		ptr += VERTEX_POSITION_SIZE;
		glVertexAttribPointer(2U, VERTEX_NORMAL_SIZE, GL_FLOAT, GL_FALSE,
				sizeof(FullVertex), ptr);
		ptr += VERTEX_NORMAL_SIZE;
		glVertexAttribPointer(3U, VERTEX_TANGENT_SIZE, GL_FLOAT, GL_FALSE,
				sizeof(FullVertex), ptr);
		ptr += VERTEX_TANGENT_SIZE;
		glVertexAttribPointer(4U, VERTEX_BINORMAL_SIZE, GL_FLOAT, GL_FALSE,
				sizeof(FullVertex), ptr);
		ptr += VERTEX_BINORMAL_SIZE;
		glVertexAttribPointer(5U, VERTEX_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE,
				sizeof(FullVertex), ptr);
		glDisableVertexAttribArray(0U);
		glEnableVertexAttribArray(1U);
		glEnableVertexAttribArray(2U);
		glEnableVertexAttribArray(3U);
		glEnableVertexAttribArray(4U);
		glEnableVertexAttribArray(5U);
	}

	glDrawElements(GL_TRIANGLES, m_uiNumIndices, GL_UNSIGNED_SHORT,
			(const GLvoid*) NULL);

	//7. DRAW CLOUDS (in VBOs)
	if (m_bShowClouds) {
		glUseProgram(m_uivCloudPrograms[m_uiCurrCloudProgIdx]);

		glUniformMatrix4fv(m_iMVPMatUnifLocCloud[m_uiCurrCloudProgIdx],
				(GLsizei) 1, GL_FALSE, &(fmMVP[0][0]));
		glUniform3fv(m_iSunPosObjUnifLocCloud[m_uiCurrCloudProgIdx],
				(GLsizei) 1, m_fvSunPosObj);
		glUniform3fv(m_iEyePosObjUnifLocCloud[m_uiCurrCloudProgIdx],
				(GLsizei) 1, m_fvEyePosObj);
		glUniform1f(m_iRadiusUnifLocCloud[m_uiCurrCloudProgIdx],
				(m_fGlobeRadius + m_fCloudHeight));

		if (m_uiCurrCloudProgIdx == 0U) {
			glBindBuffer(GL_ARRAY_BUFFER, m_uiCloudVBO);

			GLfloat *ptr = NULL;
			glVertexAttribPointer(1U, VERTEX_POSITION_SIZE, GL_FLOAT, GL_FALSE,
					sizeof(FullVertex), ptr);
			ptr += VERTEX_POSITION_SIZE;
			glVertexAttribPointer(2U, VERTEX_NORMAL_SIZE, GL_FLOAT, GL_FALSE,
					sizeof(FullVertex), ptr);
			ptr += VERTEX_NORMAL_SIZE;
			glVertexAttribPointer(3U, VERTEX_TANGENT_SIZE, GL_FLOAT, GL_FALSE,
					sizeof(FullVertex), ptr);
			ptr += VERTEX_TANGENT_SIZE;
			glVertexAttribPointer(4U, VERTEX_BINORMAL_SIZE, GL_FLOAT, GL_FALSE,
					sizeof(FullVertex), ptr);
			ptr += VERTEX_BINORMAL_SIZE;
			glVertexAttribPointer(5U, VERTEX_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE,
					sizeof(FullVertex), ptr);
			glDisableVertexAttribArray(0U);
			glEnableVertexAttribArray(1U);
			glEnableVertexAttribArray(2U);
			glEnableVertexAttribArray(3U);
			glEnableVertexAttribArray(4U);
			glEnableVertexAttribArray(5U);
		}

		glDrawElements(GL_TRIANGLES, m_uiNumIndices, GL_UNSIGNED_SHORT,
				(const GLvoid*) NULL);
	}

	glDisable(GL_BLEND);

	//8. Earth rotation animation
	if (!m_bPauseAnim) {
#if TIME_DRIVEN_ANIMATION
		//Rotating according to time since last function call
		if (m_fRunningAnimTime > 0.0F) {
			GLfloat fAngle = (m_fRotSpeed)
					* (GLfloat) ((current_time - m_fRunningAnimTime)
							* DEGS_PER_REV);
			(m_fCurrRotAngle) += fAngle;
		}
#else
		//Rotating a set amount of rotation per frame, it gives me an idea of
		//how consistent the framerate is
		(m_fCurrRotAngle) += (m_fRotSpeed);
#endif
	}
	m_fRunningAnimTime = current_time;

	//9. Compute framerate
	(m_iCurrFrame)++;
	if (m_iCurrFrame == (5 * 60)) {
		DEBUG("%f fps", m_iCurrFrame / (current_time - m_fStartTime));
		m_iCurrFrame = 0;
		m_fStartTime = current_time;
	}

	return true;
}

/**
 * Function that, given a current state (reflected in the toggle booleans), then
 * selects the appropriate program for each mesh. Also, in the case of raw data,
 * it bind the raw data VBO once (there is no need to change it between renders)
 *
 * Programs are indexed by a integer (i.e. m_uiCurrGlobeProgIdx) from 0 to 7.
 * The different boolean flags modify the index in a bitfield-like manner.
 * Starting with the least significant bit, they are ordered: normal mapping,
 * parallax mapping and raw data (just passing raw spherical coordinates, and
 * having the vertex shader derive all the other vertex attributes)
 */
bool GlobeApp::switchProgramGL(void) {
	m_uiCurrGlobeProgIdx = 0U;
	m_uiCurrCloudProgIdx = 0U;

	if (m_bRawData) {
		m_uiCurrGlobeProgIdx = 4U;
		m_uiCurrCloudProgIdx = 1U;

		glBindBuffer(GL_ARRAY_BUFFER, m_uiRawVBO);

		glVertexAttribPointer(0U, VERTEX_RAWDATA_SIZE, GL_FLOAT, GL_FALSE,
				sizeof(RawVertex), NULL);

		glEnableVertexAttribArray(0U);
		glDisableVertexAttribArray(1U);
		glDisableVertexAttribArray(2U);
		glDisableVertexAttribArray(3U);
		glDisableVertexAttribArray(4U);
		glDisableVertexAttribArray(5U);
	}

	if (m_bParallaxMapping)
		m_uiCurrGlobeProgIdx += 2U;

	if (m_bNormalMapping)
		m_uiCurrGlobeProgIdx += 1U;

	DEBUG(
			"Shader state: rawData=%d, parallax=%d, normal=%d. \
		   Using globe program %d and cloud program %d",
			m_bRawData, m_bParallaxMapping, m_bNormalMapping, m_uiCurrGlobeProgIdx, m_uiCurrCloudProgIdx);

	return true;
}

/**
 *
 */
bool GlobeApp::positionCameraGL(void) {
	GLfloat fvRotXMat[4][4], fvRotYMat[4][4], fvRotFinalMat[4][4];
	NvBuildRotYDegMatf(fvRotYMat, -(m_fCamRotYAngle));
	NvBuildRotXDegMatf(fvRotXMat, -(m_fCamRotXAngle));
	NvMultMatf(fvRotFinalMat, fvRotXMat, fvRotYMat);

	NvTransformPointf(m_fvCurrEyePos, fvRotFinalMat, m_fvEyePos);

	NvBuildLookatMatf(m_fmView, m_fvCurrEyePos, m_fvEarthPos, m_fvUp);

	//Update View-Projection matrix (when changing camera pos. or viewport)
	NvMultMatf(m_fmViewProj, m_fmProj, m_fmView);

	return true;
}

/**
 *
 */
bool GlobeApp::cleanupGL(void) {
	DEBUG("CLEANUP");

	for (int k = 0; k < (int) dimof(m_uivTextures); k++) {
		if (m_uivTextures[k] && glIsTexture(m_uivTextures[k])) {
			glDeleteTextures((GLsizei) 1, &(m_uivTextures[k]));
			m_uivTextures[k] = GLUINT_ZERO;
		} else {
			DEBUG("ERROR:\tdeleting non-existent texture.");
			return false;
		}
	}

	for (int k = 0; k < (int) dimof(m_uivGlobePrograms); k++) {
		if (m_uivGlobePrograms[k] && glIsProgram(m_uivGlobePrograms[k])) {
			glDeleteProgram(m_uivGlobePrograms[k]);
			m_uivGlobePrograms[k] = GLUINT_ZERO;
		} else {
			DEBUG("ERROR:\tdeleting non-existent globe program.");
			return false;
		}
	}

	for (int k = 0; k < (int) dimof(m_uivCloudPrograms); k++) {
		if (m_uivCloudPrograms[k] && glIsProgram(m_uivCloudPrograms[k])) {
			glDeleteProgram(m_uivCloudPrograms[k]);
			m_uivCloudPrograms[k] = GLUINT_ZERO;
		} else {
			DEBUG("ERROR:\tdeleting non-existent cloud program.");
			return false;
		}
	}

	if (m_uiGlobeVBO && glIsBuffer(m_uiGlobeVBO)) {
		glDeleteBuffers((GLsizei) 1, &(m_uiGlobeVBO));
		m_uiGlobeVBO = GLUINT_ZERO;
	} else {
		DEBUG("ERROR:\tdeleting non-existent globe VBO.");
		return false;
	}

	if (m_uiCloudVBO && glIsBuffer(m_uiCloudVBO)) {
		glDeleteBuffers((GLsizei) 1, &(m_uiCloudVBO));
		m_uiCloudVBO = GLUINT_ZERO;
	} else {
		DEBUG("ERROR:\tdeleting non-existent cloud VBO.");
		return false;
	}

	if (m_uiRawVBO && glIsBuffer(m_uiRawVBO)) {
		glDeleteBuffers((GLsizei) 1, &(m_uiRawVBO));
		m_uiRawVBO = GLUINT_ZERO;
	} else {
		DEBUG("ERROR:\tdeleting non-existent raw VBO.");
		return false;
	}

	if (m_uiIBO && glIsBuffer(m_uiIBO)) {
		glDeleteBuffers((GLsizei) 1, &(m_uiIBO));
		m_uiIBO = GLUINT_ZERO;
	} else {
		DEBUG("ERROR:\tdeleting non-existent IBO.");
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
//PUBLIC METHODS

/**
 *	Constructor
 */
GlobeApp::GlobeApp(void) {
	m_bInitialized = false;
	m_uiVertSlices = (GLuint) 256;
	m_iWinWidth = 640;
	m_iWinHeight = 480;
}

/**
 * Destructor
 */
GlobeApp::~GlobeApp(void) {
	if (m_bInitialized)
		DEBUG("ERROR:\tdestructor called while not cleaned up properly.");
}

/**
 *
 */
bool GlobeApp::init() {
	if (m_bInitialized) {
		DEBUG("ERROR:\tinit method called while not cleaned up properly.");
		return false;
	}

	//INITIALIZE VALUES
	if (!(resetValues()))
		return false;

	//CREATE TEXTURES
	if (!(initTexturesGL()))
		return false;

	//CREATE GPU PROGRAMS
	if (!(initShadersGL()))
		return false;

	//CREATE GEOMETRY
	if (!(initMeshesGL()))
		return false;

	//SETTING OPENGL STATE
	if (!(initStateGL()))
		return false;

	//FINAL SETUP WORK
	if (!(positionCameraGL()))
		return false;
	if (!(switchProgramGL()))
		return false;

	if (!resizeGL())
		return false;

	m_bInitialized = true;

	return true;
}

/**
 *
 */
bool GlobeApp::resize(const int width, const int height) {
	m_iWinWidth = (GLint) width;
	m_iWinHeight = (GLint) height;

	if (m_bInitialized) {
		return resizeGL();
	}

	return true;
}

/**
 *
 */
bool GlobeApp::render(const float current_time) {
	if (m_bInitialized) {
		return renderGL(current_time);
	}

	DEBUG("ERROR:\trender method called while not initialized properly.");
	return false;
}

/**
 *
 */
bool GlobeApp::toggleClouds(void) {
	if (m_bInitialized) {
		m_bShowClouds = !m_bShowClouds;
		DEBUG("Show clouds = %d", m_bShowClouds ? 1 : 0);
		return true;
	}

	DEBUG("ERROR:\ttoggleClouds method called while not initialized properly.");
	return false;
}

/**
 *
 */
bool GlobeApp::toggleRawData(void) {
	if (m_bInitialized) {
		m_bRawData = !m_bRawData;
		return switchProgramGL();
	}

	DEBUG(
			"ERROR:\ttoggleRawData method called while not initialized properly.");
	return false;
}

/**
 *
 */
bool GlobeApp::toggleNormalMapping(void) {
	if (m_bInitialized) {
		m_bNormalMapping = !m_bNormalMapping;
		return switchProgramGL();
	}

	DEBUG(
			"ERROR:\ttoggleNormalMapping method called while not initialized properly.");
	return false;
}

/**
 *
 */
bool GlobeApp::toggleParallaxMapping(void) {
	if (m_bInitialized) {
		m_bParallaxMapping = !m_bParallaxMapping;
		return switchProgramGL();
	}

	DEBUG(
			"ERROR:\ttoggleParallaxMapping method called while not initialized properly.");
	return false;
}

/**
 *
 */
bool GlobeApp::positionCamera(void) {
	if (m_bInitialized) {
		return positionCameraGL();
	}

	DEBUG(
			"ERROR:\tpositionCamera method called while not initialized properly.");
	return false;
}

/**
 *
 */
bool GlobeApp::cleanup(void) {
	if (m_bInitialized) {
		m_bInitialized = false;
		return cleanupGL();
	}

	// Not an error to cleanup if we're uninitialized
	return true;
}

/**
 *
 */
bool GlobeApp::restart() {
	return cleanup() && init();
}

bool GlobeApp::changeGeometryRes(unsigned int vertSlices) {
	m_uiVertSlices = vertSlices;
	if (m_bInitialized)
		restart();
}

bool GlobeApp::initGeometryShaders(void) {
	char ucBuffer[1024];

	//Fill in the #defines to be included in the shader source code before
	//what is in the text file
	memset(ucBuffer, '\0', 1024);

	m_shprogGeometry = nv_load_program("ndpass", ucBuffer);

	glBindAttribLocation(m_shprogGeometry, 1, "pos_attr");
	glBindAttribLocation(m_shprogGeometry, 2, "nor_attr");
	glBindAttribLocation(m_shprogGeometry, 3, "tan_attr");
	glBindAttribLocation(m_shprogGeometry, 4, "bin_attr");
	glBindAttribLocation(m_shprogGeometry, 5, "tex_attr");

	glLinkProgram(m_shprogGeometry);

	glUseProgram(m_shprogGeometry);

	//Uniforms that need to be updated every frame
	m_iMVPMatUnifLoc = glGetUniformLocation(m_shprogGeometry, "MVPMat");
	m_iModelMatUnifLoc = glGetUniformLocation(m_shprogGeometry, "ModelMat");
	m_iModelViewMatUnifLoc = glGetUniformLocation(m_shprogGeometry,
			"ModelViewMat");

	//Uniforms that need to be set once

	glUniform1i(glGetUniformLocation(m_shprogGeometry, "NormalMapTex"),
			(GLint) 1);

	glUseProgram(0);

	return true;
}

bool GlobeApp::initLightGeometry(void) {
	FullVertex *fvLightAttribs = NULL;
	GLushort *usvElements = NULL;

	//Get information for the globe VBO, the raw VBO and the IBO
	if (!(createCircle(&fvLightAttribs, &usvElements, &(m_uiLightNumVerts),
			&(m_uiLightNumIndices), 0.2F, 32)) || !usvElements)
		return false;

	//Generate buffer objects
	glGenBuffers((GLsizei) 1, &(m_uiLightVBO));
	glGenBuffers((GLsizei) 1, &(m_uiLightIBO));

	//Create globe vertex attribute VBO
	glBindBuffer(GL_ARRAY_BUFFER, m_uiLightVBO);
	glBufferData(GL_ARRAY_BUFFER, m_uiLightNumVerts * sizeof(FullVertex),
			fvLightAttribs, GL_STATIC_DRAW);

	//Create IBO (for all VBOs)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiLightIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			m_uiLightNumIndices * sizeof(GLushort), usvElements,
			GL_STATIC_DRAW);

	//3) Cleanup
	if (fvLightAttribs)
		delete fvLightAttribs;
	if (usvElements)
		delete usvElements;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool GlobeApp::drawNormalDepth(void) {

	GLuint framebuffer;
	GLuint depthRenderbuffer;

	GLint texWidth = m_iWinWidth, texHeight = m_iWinHeight;
	GLint maxRenderbufferSize;

	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxRenderbufferSize);
	// check if GL_MAX_RENDERBUFFER_SIZE is >= texWidth and texHeight
	if ((maxRenderbufferSize <= texWidth)
			|| (maxRenderbufferSize <= texHeight)) {
		// cannot use framebuffer objects as we need to create
		// a depth buffer as a renderbuffer object
		// return with appropriate error
		return false;
	}
	// generate the framebuffer, renderbuffer, and texture object names
	glGenFramebuffers(1, &framebuffer);
	glGenRenderbuffers(1, &depthRenderbuffer);
	glGenTextures(1, &m_renderTexture);
	// bind texture and load the texture mip-level 0
	// texels are RGBA 8888 Hopefully
	// no texels need to be specified as we are going to draw into
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA,
			GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// bind renderbuffer and create a 16-bit depth buffer
	// width and height of renderbuffer = width and height of
	// the texture
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, texWidth,
			texHeight);
	// bind the framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// specify texture as color attachment
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
			m_renderTexture, 0);
	// specify depth_renderbufer as depth attachment
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_RENDERBUFFER, depthRenderbuffer);
	// check for framebuffer complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
		// render to texture using FBO
		// clear color and depth buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// load uniforms for vertex and fragment shader
		// used to render to FBO. The vertex shader is the // ES 1.1 vertex shader described as Example 8-8 in // Chapter 8. The fragment shader outputs the color // computed by vertex shader as fragment color and // is described as Example 1-2 in Chapter 1. set_fbo_texture_shader_and_uniforms();
		// drawing commands to the framebuffer object
		renderGeometry();
		// render to window system provided framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		// Use texture to draw to window system provided framebuffer
		// We draw a quad that is the size of the viewport. //
		// The vertex shader outputs the vertex position and texture
		// coordinates passed as inputs.
		//
		// The fragment shader uses the texture coordinate to sample
		// the texture and uses this as the per-fragment color value. set_screen_shader_and_uniforms();
		//draw_screen_quad();
	}
	// cleanup
	//glDeleteRenderbuffers(1, &depthRenderbuffer); glDeleteFramebuffers(1, &framebuffer); glDeleteTextures(1, &texture);

	return false;

}

bool GlobeApp::renderGeometry(void) {
	GLfloat fmCurrModel[4][4], fmInvCurrModel[4][4];
	GLfloat fmMVP[4][4];
	GLfloat fmModelView[4][4];

	//We use the same IBO for all VBOs, bind it once
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBO);

//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, m_uivTextures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_uivTextures[1]);
//	glActiveTexture(GL_TEXTURE1);
//	glBindTexture(GL_TEXTURE_2D, m_uivTextures[2]);
//	glActiveTexture(GL_TEXTURE3);
//	glBindTexture(GL_TEXTURE_2D, m_uivTextures[3]);
	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);

	//2. MODEL MATRIX
	//2.1. Create current model matrix (from stored matrix with translation)
	NvMultRotYDegMatf(fmCurrModel, m_fmModel, m_fCurrRotAngle);
	//2.1. Store inverted copy of current model matrix
	NvBuildRotYDegMatf(fmInvCurrModel, -(m_fCurrRotAngle));
	NvMultMatf(fmInvCurrModel, m_fmInvModel, fmInvCurrModel);

	//3. MODEL-VIEW-PROJECTION MATRIX
	NvMultMatf(fmMVP, m_fmViewProj, fmCurrModel);

	//MODEL - VIEW Matrix
	NvMultMatf(fmModelView, m_fmView, fmCurrModel);

	//6. DRAW GLOBE (in VBOs)
	glUseProgram(m_shprogGeometry);

	glUniformMatrix4fv(m_iMVPMatUnifLoc, (GLsizei) 1, GL_FALSE, &(fmMVP[0][0]));

	glUniformMatrix4fv(m_iModelMatUnifLoc, (GLsizei) 1, GL_FALSE,
			&(m_fmModel[0][0]));

	glUniformMatrix4fv(m_iModelViewMatUnifLoc, (GLsizei) 1, GL_FALSE,
			&(fmModelView[0][0]));

	glBindBuffer(GL_ARRAY_BUFFER, m_uiGlobeVBO);

	GLfloat *ptr = NULL;
	glVertexAttribPointer(1U, VERTEX_POSITION_SIZE, GL_FLOAT, GL_FALSE,
			sizeof(FullVertex), ptr);
	ptr += VERTEX_POSITION_SIZE;
	glVertexAttribPointer(2U, VERTEX_NORMAL_SIZE, GL_FLOAT, GL_FALSE,
			sizeof(FullVertex), ptr);
	ptr += VERTEX_NORMAL_SIZE;
	glVertexAttribPointer(3U, VERTEX_TANGENT_SIZE, GL_FLOAT, GL_FALSE,
			sizeof(FullVertex), ptr);
	ptr += VERTEX_TANGENT_SIZE;
	glVertexAttribPointer(4U, VERTEX_BINORMAL_SIZE, GL_FLOAT, GL_FALSE,
			sizeof(FullVertex), ptr);
	ptr += VERTEX_BINORMAL_SIZE;
	glVertexAttribPointer(5U, VERTEX_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE,
			sizeof(FullVertex), ptr);
	glDisableVertexAttribArray(0U);
	glEnableVertexAttribArray(1U);
	glEnableVertexAttribArray(2U);
	glEnableVertexAttribArray(3U);
	glEnableVertexAttribArray(4U);
	glEnableVertexAttribArray(5U);

	glDrawElements(GL_TRIANGLES, m_uiNumIndices, GL_UNSIGNED_SHORT,
			(const GLvoid*) NULL);

	glDisable(GL_BLEND);
	return false;
}

bool GlobeApp::initLightShaders(void) {

	char ucBuffer[1024];

	//Fill in the #defines to be included in the shader source code before
	//what is in the text file
	memset(ucBuffer, '\0', 1024);

	m_shprogGeometry = nv_load_program("light", ucBuffer);

	glBindAttribLocation(m_shprogLight, 1, "pos_attr");

	glLinkProgram(m_shprogLight);

	glUseProgram(m_shprogLight);

	//Uniforms that need to be updated every frame
	m_iLightModelViewMatUnifLoc = glGetUniformLocation(m_shprogLight,
			"ModelViewMat");
	m_iLightPosViewLoc = glGetUniformLocation(m_shprogLight, "LightPosView");
	m_ilightColorLoc = glGetUniformLocation(m_shprogLight, "lightColor");

	//Uniforms that need to be set once
	glUniform1i(glGetUniformLocation(m_shprogLight, "NDTex"), (GLint) 2);

	glUseProgram(0);

	return true;

}

bool GlobeApp::renderLights(void) {
	GLfloat fmCurrModel[4][4], fmInvCurrModel[4][4];
	GLfloat fmMVP[4][4];
	GLfloat fmModelView[4][4];

	//We use the same IBO for all VBOs, bind it once
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_uiIBO);

	//	glActiveTexture(GL_TEXTURE0);
	//	glBindTexture(GL_TEXTURE_2D, m_uivTextures[0]);
//		glActiveTexture(GL_TEXTURE1);
//		glBindTexture(GL_TEXTURE_2D, m_uivTextures[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	//	glActiveTexture(GL_TEXTURE3);
	//	glBindTexture(GL_TEXTURE_2D, m_uivTextures[3]);
	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);

	//MODEL - VIEW Matrix
	NvMultMatf(fmModelView, m_fmView, m_fmLightModel);

	//6. DRAW GLOBE (in VBOs)
	glUseProgram(m_shprogLight);

	glUniformMatrix4fv(m_iModelViewMatUnifLoc, (GLsizei) 1, GL_FALSE,
			&(fmModelView[0][0]));

	glBindBuffer(GL_ARRAY_BUFFER, m_uiLightVBO);

	GLfloat *ptr = NULL;
	glVertexAttribPointer(1U, VERTEX_POSITION_SIZE, GL_FLOAT, GL_FALSE,
			sizeof(FullVertex), ptr);
	ptr += VERTEX_POSITION_SIZE;
//		glVertexAttribPointer(2U, VERTEX_NORMAL_SIZE, GL_FLOAT, GL_FALSE,
//				sizeof(FullVertex), ptr);
	ptr += VERTEX_NORMAL_SIZE;
//		glVertexAttribPointer(3U, VERTEX_TANGENT_SIZE, GL_FLOAT, GL_FALSE,
//				sizeof(FullVertex), ptr);
	ptr += VERTEX_TANGENT_SIZE;
//		glVertexAttribPointer(4U, VERTEX_BINORMAL_SIZE, GL_FLOAT, GL_FALSE,
//				sizeof(FullVertex), ptr);
	ptr += VERTEX_BINORMAL_SIZE;
//		glVertexAttribPointer(5U, VERTEX_TEXCOORD_SIZE, GL_FLOAT, GL_FALSE,
//				sizeof(FullVertex), ptr);
	glDisableVertexAttribArray(0U);
	glEnableVertexAttribArray(1U);
//		glEnableVertexAttribArray(2U);
//		glEnableVertexAttribArray(3U);
//		glEnableVertexAttribArray(4U);
//		glEnableVertexAttribArray(5U);

	glDrawElements(GL_TRIANGLES, m_uiNumIndices, GL_UNSIGNED_SHORT,
			(const GLvoid*) NULL);

	glDisable(GL_BLEND);
	return false;
}

