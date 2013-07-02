//----------------------------------------------------------------------------------
// File:        native_globe/assets/cloud.vert
// SDK Version: v10.10 
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

precision mediump float;

varying highp vec2 tex_var;
varying lowp  vec3 eye_dir_var;
//varying lowp float NdotL;

//uniform mat4 MVPMat;    // Model-View-Projection matrix
uniform vec3 LightPosObj; // Light position in OBJECT space
uniform vec3 EyePosObj; // Eye position in OBJECT space
uniform float Radius;

/**
 *	Rigid transformations only
 */
void main()
{


	#define normal   (nor_attr)
	#define tangent  (tan_attr)
	#define binormal (bin_attr)
	#define position (pos_attr)
	#define texcoord (tex_attr)
    

	gl_Position = MVPMat * vec4(position, 1.0);
    tex_var     = texcoord; 
    
     //Per-vertex data to vary across fragments
    eye_dir_var = InvTBNMat * normalize(EyePosObj - position);
    
    //Trick: I can compute direction of the light in tangent space.
    //       For the cloud, I can just use (0, 0, 1) as the normal.
	NdotL = (InvTBNMat * normalize(SunPosObj - position)).z;
}
