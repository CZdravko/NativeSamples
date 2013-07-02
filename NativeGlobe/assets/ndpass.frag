//----------------------------------------------------------------------------------
// File:        native_globe/assets/globe.frag
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

#define NORMAL_IN_TANGENT_SPACE (vec3(0, 0, 1))

precision mediump float;

varying highp vec2 tex_var;


uniform sampler2D NormalMapTex;

varying mat3 tbn;
varying vec3 pos;

vec2 pack16F( float input)
{
  vec2 result;
  result.y = floor(input/256);
  result.x = input - 256*result.y;
  return result;
}

float unpack16F( vec2 packed )
{
  float result = packed.x + packed.y*256;
  return result;
}

void main()
{
	//RGB contain the normal (B does not need to be scaled back onto range),
	//A contains the height
	vec4 vNormalMapFrag = texture2D(NormalMapTex, tex_var);
	
#if NORMAL_MAPPING
	vec3 N = vec3((vNormalMapFrag.rg - vec2(0.5)) * 2.0, vNormalMapFrag.b);
#else
	vec3 N = NORMAL_IN_TANGENT_SPACE;
#endif

	//Only store x,y of normalized vector
	vec2 Normal = (normalize(tbn * N).xy;

	//Shift from [-1,1] to [0,1]
	vec2 GBufferN = Normal/2 + vec2(0.5,0.5);

	gl_FragColor = vec4( GBufferN , pack16F(length(pos.z)));
}
