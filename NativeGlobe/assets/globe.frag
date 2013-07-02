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
varying lowp  vec3 eye_dir_var;
varying lowp  vec3 lit_dir_var;

uniform sampler2D ColorTex;
uniform sampler2D NormalMapTex;
uniform sampler2D CityLightsTex;

//As an optimization, we are not normalizing L and V.
//With enough tesselation, this should be unnoticeable.
#define L (lit_dir_var)
#define V (eye_dir_var)

void main()
{
	float fEdgeAlpha = clamp(eye_dir_var.z * EDGE_ANTIALIASING_CLAMP, 0.0, 1.0);

	//RGB contain the normal (B does not need to be scaled back onto range),
	//A contains the height
	vec4 vNormalMapFrag = texture2D(NormalMapTex, tex_var);
	
#if NORMAL_MAPPING
	vec3 N = vec3((vNormalMapFrag.rg - vec2(0.5)) * 2.0, vNormalMapFrag.b);
#else
	vec3 N = NORMAL_IN_TANGENT_SPACE;
#endif

	float fWaterMask = 1.0 - clamp(vNormalMapFrag.a * WATER_MASK_CLAMP,
								   0.0, 1.0);

	float NdotL = dot(L, N);
	float RdotV = max(dot(2.0 * NdotL * N - L, V), 0.0);
	RdotV = RdotV * RdotV;
	RdotV = RdotV * RdotV;
	RdotV = RdotV * RdotV;
	NdotL = max(NdotL, 0.0);

	vec3 speColor = vec3(fWaterMask * RdotV);
	
#if PARALLAX_MAPPING
	vec2 vParallaxTex = tex_var - V.xy *
						(vNormalMapFrag.a * PARALLAX_SCALE + PARALLAX_BIAS);
	vec4 vColorFrag = texture2D(ColorTex, vParallaxTex);
	vec4 vCityLightsFrag = texture2D(CityLightsTex, vParallaxTex);
#else
	vec4 vColorFrag = texture2D(ColorTex, tex_var);
	vec4 vCityLightsFrag = texture2D(CityLightsTex, tex_var);
#endif
	
	vec3 difColor = vColorFrag.rgb * NdotL;
	vec3 litColor = clamp(3.0 * vCityLightsFrag.rgb, 0.0, 1.0) * (1.0 - NdotL);
	
	gl_FragColor = vec4(AMBIENT_COLOR + difColor + speColor + litColor,
						fEdgeAlpha);
}
