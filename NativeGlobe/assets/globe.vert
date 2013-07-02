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

#if USING_RAW_DATA
attribute vec2 raw_attr; // Longitude and latitude
#else
attribute vec3 pos_attr; // Position
attribute vec3 nor_attr; // Normal
attribute vec3 tan_attr; // Tangent
attribute vec3 bin_attr; // Binormal
attribute vec2 tex_attr; // Texcoords
#endif

precision mediump float;

varying highp vec2 tex_var;
varying lowp  vec3 eye_dir_var;
varying lowp  vec3 lit_dir_var;

uniform mat4 MVPMat;    // Model-View-Projection matrix
uniform vec3 SunPosObj; // Light position in OBJECT space
uniform vec3 EyePosObj; // Eye position in OBJECT space
uniform float Radius;

/**
 *	Rigid transformations only
 */
void main()
{

#if USING_RAW_DATA

	vec2 angle = TWO_PI * raw_attr;
	vec2 cosine = cos(angle);
	vec2   sine = sin(angle);
	
	vec3 normal   = vec3(sine.y * sine.x, cosine.y, sine.y * cosine.x);
	vec3 tangent  = vec3(-cosine.x, 0.0, sine.x);
	vec3 binormal = cross(normal, tangent);
	vec3 position = Radius * normal;
	vec2 texcoord = vec2(raw_attr.x, 1.0 - 2.0 * raw_attr.y); 
    
#else

	#define normal   (nor_attr)
	#define tangent  (tan_attr)
	#define binormal (bin_attr)
	#define position (pos_attr)
	#define texcoord (tex_attr)
    
#endif

	gl_Position = MVPMat * vec4(position, 1.0);
    tex_var     = texcoord; 
    
    //Change-of-basis matrix from object to tangent space
    //NOTE 1: It's inverse of change-of-basis matrix TBN from tangent to object
    //NOTE 2: Vectors are orthonormal, inverse = transpose
    //NOTE 3: mat3 takes elements in column-major order  
    mat3 InvTBNMat = mat3(tangent.x, binormal.x, normal.x,  //1st col
    					  tangent.y, binormal.y, normal.y,  //2nd col
    					  tangent.z, binormal.z, normal.z); //3rd col
    
    //Per-vertex data to vary across fragments
    eye_dir_var = InvTBNMat * normalize(EyePosObj - position);
	lit_dir_var = InvTBNMat * normalize(SunPosObj - position);
}
