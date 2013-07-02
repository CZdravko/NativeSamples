//----------------------------------------------------------------------------------
// File:        native_globe/jni/GlobeUtil.h
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

#ifndef GLOBEUTIL_H_
#define GLOBEUTIL_H_

#include <GLES2/gl2.h>

typedef struct FullVertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat tangent[3];
	GLfloat binormal[3];
	GLfloat texcoord[2];
} FullVertex;

typedef struct RawVertex
{
	GLfloat rawdata[2];
} RawVertex;

bool createSphere(FullVertex **fullAttribs, RawVertex **rawAttribs,
				  GLushort **elements, GLuint *numVertices, GLuint *numIndices,
				  const GLfloat radius, const GLuint slices);

bool createHHDDSTextureGL(GLuint &textureID, const unsigned int index,
						  const GLenum textureUnit, const bool flipVertical,
						  const bool genMipmaps, const char *filename);

bool createNormalMapGL(GLuint &textureID, const unsigned int index,
					   const GLenum textureUnit, const GLfloat heightScale,
					   const char *filename);

bool createCircle(FullVertex **fullAttribs,
				  GLushort **elements, GLuint *numVertices, GLuint *numIndices,
				  const GLfloat radius, const GLuint slices);

#endif /* GLOBEUTIL_H_ */
