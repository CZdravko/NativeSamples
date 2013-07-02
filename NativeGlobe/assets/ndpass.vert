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
attribute vec3 nor_attr; // Normal
attribute vec3 tan_attr; // Tangent
attribute vec3 bin_attr; // Binormal
attribute vec2 tex_attr; // Texcoords

precision mediump float;


uniform mat4 MVPMat;    // Model-View-Projection matrix
uniform mat4 ModelMat; //ModelMatrix
uniform mat4 ModelViewMat
varying mat3 tbn;
varying vec3 pos;
varying highp vec2 tex_var;
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
	pos = vec3 (ModelViewMat * position); / / transformed point to world space
    
    	mat3 NORMALMATRIX = mat3(ModelMat);

	vec3 nor = normalize(NORMALMATRIX * normal); // Normal 
	vec3 tan = normalize(NORMALMATRIX * tangent); // Tangent
	vec3 bin = normalize(NORMALMATRIX * binormal); 

    //Change-of-basis matrix from object to tangent space
    //NOTE 1: It's inverse of change-of-basis matrix TBN from tangent to object
    //NOTE 2: Vectors are orthonormal, inverse = transpose
    //NOTE 3: mat3 takes elements in column-major order  
    tbn = mat3(tan.x, bin.x, nor.x,  //1st col
    					  tan.y, bin.y, nor.y,  //2nd col
    					  tan.z, bin.z, nor.z); //3rd col
    
}
