//----------------------------------------------------------------------------------
// File:        native_globe/assets/globe.frag
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
 * This shader requires preprocessor definitions for NORMAL_MAPPING and
 * PARALLAX_MAPPING
 */

#define NORMAL_IN_TANGENT_SPACE (vec3(0, 0, 1))
#define EDGE_ANTIALIASING_CLAMP (10.0)
#define WATER_MASK_CLAMP (10.0)

precision mediump float;

varying highp vec2 tex_var;
varying vec3 pos;

uniform sampler2D ColorTex;
uniform sampler2D CityLightsTex;
uniform sampler2D NormalMapTex;
uniform sampler2D LightTex;


void main()
{
	float fEdgeAlpha = clamp(pos.z * EDGE_ANTIALIASING_CLAMP, 0.0, 1.0);

	//RGB contain the normal (B does not need to be scaled back onto range),
	//A contains the height
	vec4 vNormalMapFrag = texture2D(NormalMapTex, tex_var);
	
	vec2 tex_coord = (pos.xy + vec2(1.0, 1.0)) / 2; 
	
	vec4 vColorFrag = texture2D(ColorTex, tex_var);
	vec4 vCityLightsFrag = texture2D(CityLightsTex, tex_var);
	vec3 lightColor = texture2D(LightTex, tex_coord);
	
	float RdotV = lightColor.w;

	float fWaterMask = 1.0 - clamp(vNormalMapFrag.a * WATER_MASK_CLAMP,
								   0.0, 1.0);

	vec3 speColor = vec3(fWaterMask * RdotV);

	
	
	vec3 difColor = vColorFrag.rgb * lightColor.rgb;
	vec3 litColor = clamp(3.0 * vCityLightsFrag.rgb, 0.0, 1.0) * (1.0 - lightColor.rgb);
	
	
	
	gl_FragColor = vec4(AMBIENT_COLOR + difColor + speColor + litColor,
						fEdgeAlpha);
}
