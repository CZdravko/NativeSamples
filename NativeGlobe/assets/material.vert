//----------------------------------------------------------------------------------
// File:        native_globe/assets/globe.vert
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

/**
 * This shader requires preprocessor definition for USING_RAW_DATA
 */

#define TWO_PI 6.2831344


attribute vec3 pos_attr; // Position
attribute vec2 tex_attr; // Texcoords

precision mediump float;

varying highp vec2 tex_var;
varying vec3 pos;

uniform mat4 MVPMat;    // Model-View-Projection matrix
uniform mat4 ModelViewMat;    // Model-View-Projection matrix

/**
 *	Rigid transformations only
 */
void main()
{
	#define position (pos_attr)

	gl_Position = MVPMat * vec4(position, 1.0);
    tex_var     = texcoord;
    pos = ModelViewMat * vec4(position, 1.0);
     
}
