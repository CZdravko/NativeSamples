//----------------------------------------------------------------------------------
// File:        native_globe/jni/GlobeUtil.cpp
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

#include "GlobeUtil.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <stdlib.h>
#include <nv_math/nv_matrix.h>
//#include <nv_event/nv_event.h>
#include <nv_glesutil/nv_images.h>
#include <nv_hhdds/nv_hhdds.h>
#include "GlobeDefines.h"

/**
 * Draw this with GL_TRIANGLES
 */
bool createSphere(FullVertex **fullAttribs, RawVertex **rawAttribs,
				  GLushort **elements, GLuint *numVertices, GLuint *numIndices,
				  const GLfloat radius, const GLuint slices)
{
	//Input sanitation
	if((radius == GLFLOAT_ZERO) || (slices == GLUINT_ZERO))
		return false;

	GLuint uiStacks      = slices / (GLuint)2U;
	GLuint uiNumVertices = (slices + GLUINT_ONE) * (uiStacks + GLUINT_ONE);
	GLuint uiNumIndices  = (GLuint)6U * slices * uiStacks;
	//It is a quad mesh; each vertex defines 2 triangles. See diagram below.

	GLfloat fvRaw[2], fvPos[3], fvNor[3], fvTan[3], fvBin[3], fvTex[2];
	GLushort *pElemPtr = NULL;

	//Allocate heap memory
	if(fullAttribs)
	{
		(*fullAttribs) = new FullVertex[uiNumVertices];
		if(!(*fullAttribs))
			return false;
	}
	if(rawAttribs)
	{
		(*rawAttribs) = new RawVertex[uiNumVertices];
		if(!(*rawAttribs))
			return false;
	}
	if(elements)
	{
		(*elements) = new GLushort[uiNumIndices];
		if(!(*elements))
			return false;
		pElemPtr = (*elements);
	}

	//Discretization of sphere
	float delta = (2.0F * M_PI) / (float)slices;

	//Quick precompute of trig functions for longitudes
	GLfloat fSinX[slices + 1];
	GLfloat fCosX[slices + 1];
	for(GLuint lon = GLUINT_ZERO; lon <= slices; lon++)
	{
		fSinX[lon] = (GLfloat)sinf((float)lon * delta);
		fCosX[lon] = (GLfloat)cosf((float)lon * delta);
	}

	//Parallel intervals start at the top, not at the equator
	for(GLuint lat = GLUINT_ZERO; lat <= uiStacks; lat++)
	{
		//PER-LATITUDE COMPUTATIONS

		//Compute trig functions for latitudes (reused in next loop)
		GLfloat sinY = (GLfloat)sinf((float)lat * delta);
		GLfloat cosY = (GLfloat)cosf((float)lat * delta);

		//Latitude values are constant across iteration of longitudes;
		//compute just once per latitude
		fvRaw[1] = (GLfloat)lat / (GLfloat)slices;
		fvNor[1] = cosY;
		fvTex[1] = GLFLOAT_ONE - (GLfloat)lat / (GLfloat)uiStacks;

		for(GLuint lon = GLUINT_ZERO; lon <= slices; lon++)
		{
			//PER-LONGITUDE AND FINAL COMPUTATIONS

			//Spherical coordinate
			fvRaw[0] = (GLfloat)lon / (GLfloat)slices;
			//fvRaw[1] already computed

			//Spherical coords to Cartesian
			GLfloat sinX = fSinX[lon];
			GLfloat cosX = fCosX[lon];

			//Vertex normal
			fvNor[0] = sinY * sinX;
			//fvNor[1] already computed
			fvNor[2] = sinY * cosX;

			//Vertex tangent
			//Normal flattened to XZ plane and rotated 90 degrees around Y axis
			fvTan[0] = -cosX;
			fvTan[1] =  GLFLOAT_ZERO;
			fvTan[2] =  sinX;

			//Vextex binormal
			//Cross-product between normal and tangent
			NvCrossProductf(fvBin, fvNor, fvTan);

			//Vertex position
			fvPos[0] = fvNor[0] * radius;
			fvPos[1] = fvNor[1] * radius;
			fvPos[2] = fvNor[2] * radius;

			//Vertex texture coordinates
			fvTex[0] = (GLfloat)lon / (GLfloat)slices;
			//fvTex[1] already computed


			//FITTING INTO VBO
			GLuint attrIdx = lat * (slices + GLUINT_ONE) + lon;
			if(rawAttribs)
			{
				(*rawAttribs)[attrIdx].rawdata[0]  = fvRaw[0];
				(*rawAttribs)[attrIdx].rawdata[1]  = fvRaw[1];
			}
			if(fullAttribs)
			{
				(*fullAttribs)[attrIdx].position[0] = fvPos[0];
				(*fullAttribs)[attrIdx].position[1] = fvPos[1];
				(*fullAttribs)[attrIdx].position[2] = fvPos[2];
				(*fullAttribs)[attrIdx].normal[0]   = fvNor[0];
				(*fullAttribs)[attrIdx].normal[1]   = fvNor[1];
				(*fullAttribs)[attrIdx].normal[2]   = fvNor[2];
				(*fullAttribs)[attrIdx].tangent[0]  = fvTan[0];
				(*fullAttribs)[attrIdx].tangent[1]  = fvTan[1];
				(*fullAttribs)[attrIdx].tangent[2]  = fvTan[2];
				(*fullAttribs)[attrIdx].binormal[0] = fvBin[0];
				(*fullAttribs)[attrIdx].binormal[1] = fvBin[1];
				(*fullAttribs)[attrIdx].binormal[2] = fvBin[2];
				(*fullAttribs)[attrIdx].texcoord[0] = fvTex[0];
				(*fullAttribs)[attrIdx].texcoord[1] = fvTex[1];
			}

			//GENERATE INDICES
			if(elements && lat < uiStacks && lon < slices)
			{
				//For each iteration, we generate the indices for a quad.
				//
				//  1----4
				//  | \  |
				//  |  \ |
				//  2----3
				//
				// (we perform this computation for vertex 1 only. That is
				//  why we skip the last row and las column)

				//First triangle
				*(pElemPtr++) = (GLushort)( lat               *
							   (slices + GLUINT_ONE) +  lon              ); //1
				*(pElemPtr++) = (GLushort)((lat + GLUINT_ONE) *
							   (slices + GLUINT_ONE) +  lon              ); //2
				*(pElemPtr++) = (GLushort)((lat + GLUINT_ONE) *
							   (slices + GLUINT_ONE) + (lon + GLUINT_ONE)); //3

				//Second triangle
				*(pElemPtr++) = (GLushort)( lat               *
							   (slices + GLUINT_ONE) +  lon              ); //1
				*(pElemPtr++) = (GLushort)((lat + GLUINT_ONE) *
							   (slices + GLUINT_ONE) + (lon + GLUINT_ONE)); //3
				*(pElemPtr++) = (GLushort)( lat               *
							   (slices + GLUINT_ONE) + (lon + GLUINT_ONE)); //4
			}
		}
	}

	if(numVertices)
		*numVertices = uiNumVertices;
	if(numIndices)
		*numIndices  = uiNumIndices;

	return true;
}

/**
 *
 */
bool createHHDDSTextureGL(GLuint &textureID, const unsigned int index,
						  const GLenum textureUnit, const bool flipVertical,
						  const bool useMipmaps, const char *filename)
{
    // Clear the GL error before this function, since at the end of this
    // function, we test the error and report it.  We do not want to report
    // an error that happened at some random time before.  If we want to
    // catch those, we need more general/comprehensive handling.  But having
    // this function print a GL error for something that happened in other
    // random code is confusing, especially to non-rail developers.
    // Some other code, like NVBitfont, prints error messages at the top of
    // the function and print a message that implies that the error was there
    // at the time of call.  That may make sense as an optional setting down
    // the road
    glGetError();

	glActiveTexture(textureUnit);

    NvS32 swidth = 1;
    NvS32 sheight = 1;
    textureID = NvCreateTextureFromDDSEx(filename,
									   (flipVertical) ? NV_TRUE : NV_FALSE,
									   (useMipmaps)   ? NV_TRUE : NV_FALSE,
									   &swidth, &sheight, NV_NULL, NV_NULL);
    if(!textureID)
    	return false;

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
					(useMipmaps) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	return true;
}

/**
 *
 */
bool createNormalMapGL(GLuint &textureID, const unsigned int index,
					   const GLenum textureUnit, const GLfloat heightScale,
					   const char *filename)
{

	//Temporary variables for loading pixel data
	unsigned char* pHeightPixels, *pNormalPixels;
	unsigned int uiFormat, uiType, uiWidth, uiHeight;

	NVHHDDSImage *img = NVHHDDSLoad(filename, (NvS32)1);
	if(!img)
	{
		DEBUG("ERROR createNormalMapGL: problem loading height map texture.");
		return false;
	}

	pHeightPixels = (unsigned char*)(img->data[0]);
	uiWidth       = (unsigned int)  (img->width);
	uiHeight      = (unsigned int)  (img->height);
	uiFormat      = (unsigned int)  (img->format);
	uiType        = (unsigned int)  (img->componentFormat);

	unsigned int uiFormatWidth = 4U;

	pNormalPixels = new unsigned char[uiWidth * uiHeight * uiFormatWidth];
	for(unsigned int j = 0U; j < uiHeight; j++)
		for(unsigned int i = 0U; i < uiWidth; i++)
		{
			unsigned int iCenterIdx = uiFormatWidth * (j * uiWidth + i);
			unsigned int iLeftIdx   = uiFormatWidth * ((i > 0U) ?
				( j              * uiWidth + (i       - 1U)) :
				( j              * uiWidth + (uiWidth - 1U)));
			unsigned int iRightIdx  = uiFormatWidth * ((i < uiWidth - 1U) ?
				( j              * uiWidth + (i       + 1U)) :
				( j              * uiWidth +            0U ));
			unsigned int iUpIdx     = uiFormatWidth * ((j > 0U) ?
				((j        - 1U) * uiWidth +  i            ) :
				((uiHeight - 1U) * uiWidth +  i            ));
			unsigned int iDownIdx   = uiFormatWidth * ((j < uiHeight - 1U) ?
				((j        + 1U) * uiWidth +  i            ) :
				(            0U  * uiWidth +  i            ));

			GLfloat fDx, fDy, fDz;
			//Compute gradients
			fDx  = (GLfloat)(pHeightPixels[iRightIdx]); //[   0, 255]
			fDy  = (GLfloat)(pHeightPixels[iUpIdx]);    //[   0, 255]
			fDx -= (GLfloat)(pHeightPixels[iLeftIdx]);  //[-255, 255]
			fDy -= (GLfloat)(pHeightPixels[iDownIdx]);  //[-255, 255]
			fDx /= 255.0F;                              //[  -1,   1]
			fDy /= 255.0F;                              //[  -1,   1]
			//Scale them
			fDx *= heightScale;                         //[  -H,   H]
			fDy *= heightScale;                         //[  -H,   H]
			//Putting them in vectors (1, 0, fDx) and (0, 1, fDy) and
			//doing a cross product results in (-fDx, -fDy, 1)
			//BUT, for some reason, fDx needs to be turned into its
			//negative again for normal mapping to work, so we will only
			//turn fDy into its negative
			fDy *= -1;                                  //[  -H,   H]
			//We normalize the vector
			GLfloat fDet = (GLfloat)(sqrt(fDx * fDx + fDy * fDy + 1.0));
			fDx /= fDet;   fDy /= fDet;                 //[  -1,   1]
			//Now, we have to fit them into unsigned chars. We map them from
            //[-1, 1] to [0, 255]
			fDx += 1.0F;   fDy += 1.0F;                 //[   0,   2]
			fDx *= 128.0F; fDy *= 128.0F;               //[   0, 255]

			//The z component of the vector is just 1
			//(not scaled, but normalized)
			fDz  = (GLfloat)1.0F / fDet;                //[   0,   1]
			fDz *= 255.0F;

			pNormalPixels[iCenterIdx + 0U] = (unsigned char)fDx;
			pNormalPixels[iCenterIdx + 1U] = (unsigned char)fDy;
			pNormalPixels[iCenterIdx + 2U] = (unsigned char)fDz;
			pNormalPixels[iCenterIdx + 3U] = pHeightPixels[iCenterIdx];
		}

	NVHHDDSFree(img);

	glGenTextures(1, &(textureID));
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, (GLint)0, (GLint)uiFormat,
				 (GLsizei)uiWidth, (GLsizei)uiHeight, (GLint)0,
				 (GLenum)uiFormat, (GLenum)uiType,
				 (const GLvoid*)pNormalPixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	delete [] pNormalPixels;

	return true;
}


bool createCircle(FullVertex **fullAttribs,
				  GLushort **elements, GLuint *numVertices, GLuint *numIndices,
				  const GLfloat radius, const GLuint slices)
{
	float d = (2.0F * M_PI) / (float)slices;
	int t_NumIndices = 3 * slices;
	int t_NumVertices = slices + 2;
	int icenter=slices+1;
	GLushort *pElemPtr = NULL;
	//Allocate heap memory
		if(fullAttribs)
		{

			(*fullAttribs) = new FullVertex[t_NumVertices];
			if(!(*fullAttribs))
				return false;
		}
		if(elements)
		{
			(*elements) = new GLushort[t_NumIndices];
			if(!(*elements))
				return false;
			pElemPtr = (*elements);
		}

	*fullAttribs = new FullVertex[t_NumVertices];
	for(int i=0; i<=slices; i++){
		GLfloat sinY = (GLfloat)sinf((float)i * d);
		GLfloat cosY = (GLfloat)cosf((float)i * d);
		(*fullAttribs)[i].position[0] = radius * cosY;
		(*fullAttribs)[i].position[1] = radius * sinY;
		(*fullAttribs)[i].position[2] = 0;

		//Adding indices
		/*
		 * 							 *i+1
		 * 						   /
		 *						  /
		 * 				center   *-----*i
		 */

		if(i<icenter){
			*(pElemPtr++) = (GLushort)i;
			*(pElemPtr++) = (GLushort)(i+1);
			*(pElemPtr++) = (GLushort)icenter;
		}
	}

	return true;
}
