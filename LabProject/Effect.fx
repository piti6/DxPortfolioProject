//--------------------------------------------------------------------------------------
// File: Effect.fx
//--------------------------------------------------------------------------------------

#define MAX_LIGHTS			1
#define POINT_LIGHT			1.0f
#define SPOT_LIGHT			2.0f
#define DIRECTIONAL_LIGHT	3.0f
#define MAX_BONE			64
#define NULL_IDX			100000

#define _WITH_LOCAL_VIEWER_HIGHLIGHTING
#define _WITH_THETA_PHI_CONES

//--------------------------------------------------------------------------------------
// Struct For Material & Lighting
//--------------------------------------------------------------------------------------


//物質をための構造体を登録します。
struct MATERIAL
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;
	float4 m_cEmissive;
};

//光をための構造体を登録します。
struct LIGHT
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;
	float3 m_vPosition;
	float m_fRange;
	float3 m_vDirection;
	float m_nType;
	float3 m_vAttenuation;
	float m_fFalloff;
	float m_fTheta;
	float m_fPhi;
	float m_bEnable;
	float padding;
};

struct LIGHTEDCOLOR
{
	float4 m_cAmbient;
	float4 m_cDiffuse;
	float4 m_cSpecular;
};

//--------------------------------------------------------------------------------------
// Struct For Vertex Shader In & Out
//--------------------------------------------------------------------------------------

struct VS_TEXTURED_COLOR_INPUT
{
	float3 position : POSITION;
	float2 tex2dCoord : TEXCOORD0;
};

struct VS_TEXTURED_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float2 tex2dCoord : TEXCOORD0;
};
struct VS_TEXTURED_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_INSTANCED_TEXTURED_LIGHTING_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
	float4x4 mtxTransform : INSTANCEPOS;
};

struct VS_INSTANCED_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};

struct VS_INSTANCED_TEXTURED_LIGHTING_ANIMATION_INPUT
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
	uint4 boneindex : BLENDINDICES0;
	float4 boneweight : BLENDWEIGHT0;

	float4x4 mtxTransform : INSTANCEPOS;
	uint4 animationdata : ANIMATIONOFFSET;
};

struct VS_INSTANCED_TEXTURED_LIGHTING_ANIMATION_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 tex2dcoord : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------

//ビュー行列
cbuffer cbViewMatrix : register(b0)
{
	matrix		gmtxView : packoffset(c0);
	matrix		gmtxProjection : packoffset(c4);
};

//ワールド行列
cbuffer cbWorldMatrix : register(b1)
{
	matrix		gmtxWorld : packoffset(c0);
};

//普通のアニメーションに使うボーンバッファ（未使用）
cbuffer cbBone : register(b2)
{
	float4x4 BoneMtx[MAX_BONE] : packoffset(c0);
}

//インスタンス化したアニメーションに使うアニメーションテクスチャの長さ
cbuffer cbAnimationTextureWidth : register(b3)
{
	uint4 gInstanceMatricesWidth : packoffset(c0);
}

float4x4    gIdentity = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

//光バッファ
cbuffer cbLight : register(b0)
{
	LIGHT gLights[MAX_LIGHTS];
	float4 gcLightGlobalAmbient;
	float4 gvCameraPosition;
};

//マテリアルバッファ
cbuffer cbMaterial : register(b1)
{
	MATERIAL gMaterial;
};

//--------------------------------------------------------------------------------------
// Texture & Sampler
//--------------------------------------------------------------------------------------

Texture2D gtxtTexture : register(ps, t0);
Texture2D<float4> gtxtAnimation : register(vs, t0);//VSで使うアニメーションテクスチャ

SamplerState gSamplerState : register(s0);

//--------------------------------------------------------------------------------------
// Lighting Function
//--------------------------------------------------------------------------------------

//光の計算
LIGHTEDCOLOR DirectionalLight(int i, float3 vNormal, float3 vToCamera)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = -gLights[i].m_vDirection;
	float fDiffuseFactor = dot(vToLight, vNormal);
	
	if (fDiffuseFactor > 0.0f)
	{
		if (gMaterial.m_cSpecular.a != 0.0f)
		{
		#ifdef _WITH_REFLECT
			float3 vReflect = reflect(-vToLight, vNormal);
			float fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterial.m_cSpecular.a);
		#else
		#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
			float3 vHalf = normalize(vToCamera + vToLight);
		#else
			float3 vHalf = float3(0.0f, 1.0f, 0.0f);
		#endif
			float fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterial.m_cSpecular.a);
		#endif
			output.m_cSpecular = gMaterial.m_cSpecular * (gLights[i].m_cSpecular * fSpecularFactor);
		}
		output.m_cDiffuse = gMaterial.m_cDiffuse * (gLights[i].m_cDiffuse * fDiffuseFactor);
	}
	output.m_cAmbient = gMaterial.m_cAmbient * gLights[i].m_cAmbient;
	return(output);
}
LIGHTEDCOLOR PointLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;

	float3 vToLight = gLights[i].m_vPosition - vPosition;
		float fDistance = length(vToLight);
	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);
		if (fDiffuseFactor > 0.0f)
		{
			if (gMaterial.m_cSpecular.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 vReflect = reflect(-vToLight, vNormal);
					float fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterial.m_cSpecular.a);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 vHalf = normalize(vToCamera + vToLight);
#else
				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
#endif
					float fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterial.m_cSpecular.a);
#endif
				output.m_cSpecular = gMaterial.m_cSpecular * (gLights[i].m_cSpecular * fSpecularFactor);
			}
			output.m_cDiffuse = gMaterial.m_cDiffuse * (gLights[i].m_cDiffuse * fDiffuseFactor);
		}
		float fAttenuationFactor = 1.0f / dot(gLights[i].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
		output.m_cAmbient = gMaterial.m_cAmbient * (gLights[i].m_cAmbient * fAttenuationFactor);
		output.m_cDiffuse *= fAttenuationFactor;
		output.m_cSpecular *= fAttenuationFactor;
	}
	return(output);
}
LIGHTEDCOLOR SpotLight(int i, float3 vPosition, float3 vNormal, float3 vToCamera)
{
	LIGHTEDCOLOR output = (LIGHTEDCOLOR)0;
	float3 vToLight = gLights[i].m_vPosition - vPosition;
		float fDistance = length(vToLight);
	if (fDistance <= gLights[i].m_fRange)
	{
		vToLight /= fDistance;
		float fDiffuseFactor = dot(vToLight, vNormal);
		if (fDiffuseFactor > 0.0f)
		{
			if (gMaterial.m_cSpecular.a != 0.0f)
			{
#ifdef _WITH_REFLECT
				float3 vReflect = reflect(-vToLight, vNormal);
					float fSpecularFactor = pow(max(dot(vReflect, vToCamera), 0.0f), gMaterial.m_cSpecular.a);
#else
#ifdef _WITH_LOCAL_VIEWER_HIGHLIGHTING
				float3 vHalf = normalize(vToCamera + vToLight);
#else
				float3 vHalf = float3(0.0f, 1.0f, 0.0f);
#endif
					float fSpecularFactor = pow(max(dot(vHalf, vNormal), 0.0f), gMaterial.m_cSpecular.a);
#endif
				output.m_cSpecular = gMaterial.m_cSpecular * (gLights[i].m_cSpecular * fSpecularFactor);
			}
			output.m_cDiffuse = gMaterial.m_cDiffuse * (gLights[i].m_cDiffuse * fDiffuseFactor);
		}
#ifdef _WITH_THETA_PHI_CONES
		float fAlpha = max(dot(-vToLight, gLights[i].m_vDirection), 0.0f);
		float fSpotFactor = pow(max(((fAlpha - gLights[i].m_fPhi) / (gLights[i].m_fTheta - gLights[i].m_fPhi)), 0.0f), gLights[i].m_fFalloff);
#else
		float fSpotFactor = pow(max(dot(-vToLight, gLights[i].m_vDirection), 0.0f), gLights[i].m_fFalloff);
#endif
		float fAttenuationFactor = 1.0f / dot(gLights[i].m_vAttenuation, float3(1.0f, fDistance, fDistance*fDistance));
		output.m_cAmbient = gMaterial.m_cAmbient * (gLights[i].m_cAmbient * fAttenuationFactor * fSpotFactor);
		output.m_cDiffuse *= fAttenuationFactor * fSpotFactor;
		output.m_cSpecular *= fAttenuationFactor * fSpotFactor;
	}
	return(output);
}
float4 Lighting(float3 vPosition, float3 vNormal)
{
	int i;
	float3 vCameraPosition = float3(gvCameraPosition.x, gvCameraPosition.y, gvCameraPosition.z);
	float3 vToCamera = normalize(vCameraPosition - vPosition);
	LIGHTEDCOLOR LightedColor = (LIGHTEDCOLOR)0;
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (i = 0; i < MAX_LIGHTS; i++)
	{
		if (gLights[i].m_bEnable == 1.0f)
		{
			if (gLights[i].m_nType == DIRECTIONAL_LIGHT)
			{
				LightedColor = DirectionalLight(i, vNormal, vToCamera);
				cColor += (LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular);
			}
			if (gLights[i].m_nType == POINT_LIGHT)
			{
				LightedColor = PointLight(i, vPosition, vNormal, vToCamera);
				cColor += (LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular);
			}
			if (gLights[i].m_nType == SPOT_LIGHT)
			{
				LightedColor = SpotLight(i, vPosition, vNormal, vToCamera);
				cColor += (LightedColor.m_cAmbient + LightedColor.m_cDiffuse + LightedColor.m_cSpecular);
			}
		}
	}

	cColor += (gcLightGlobalAmbient * gMaterial.m_cAmbient);
	cColor.a = gMaterial.m_cDiffuse.a;
	return(cColor);
}

//アニメーションテクスチャから現在インデックスのボーンを計算して持って来ます。
float4x4 loadBoneMatrix(uint animationData, uint bone)
{
	
	float4x4 rval = gIdentity;

	uint baseIndex = animationData;
	baseIndex += (4 * bone);

	uint baseU = baseIndex%gInstanceMatricesWidth.x;
	uint baseV = baseIndex / gInstanceMatricesWidth.x;
	
	float4 mat1 = gtxtAnimation.Load(uint3(baseU, baseV, 0));
	float4 mat2 = gtxtAnimation.Load(uint3(baseU + 1, baseV, 0));
	float4 mat3 = gtxtAnimation.Load(uint3(baseU + 2, baseV, 0));
	float4 mat4 = gtxtAnimation.Load(uint3(baseU + 3, baseV, 0));

	return float4x4(float4(mat1), float4(mat2),	float4(mat3), float4(mat4));
}

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------

VS_TEXTURED_COLOR_OUTPUT VSTexturedColor(VS_TEXTURED_COLOR_INPUT input)
{
	VS_TEXTURED_COLOR_OUTPUT output = (VS_TEXTURED_COLOR_OUTPUT)0;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.tex2dCoord = input.tex2dCoord;

	return(output);
}

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output = (VS_TEXTURED_LIGHTING_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)gmtxWorld);

	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.tex2dcoord = input.tex2dcoord;

	return(output);
}


VS_INSTANCED_TEXTURED_LIGHTING_OUTPUT VSInstancedTexturedLighting(VS_INSTANCED_TEXTURED_LIGHTING_INPUT input)
{
	VS_INSTANCED_TEXTURED_LIGHTING_OUTPUT output = (VS_INSTANCED_TEXTURED_LIGHTING_OUTPUT)0;
	output.normalW = mul(input.normal, (float3x3)input.mtxTransform);
	//output.normalW = input.normal, (float3x3)input.mtxTransform);
	output.positionW = mul(float4(input.position, 1.0f), input.mtxTransform).xyz;
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.tex2dcoord = input.tex2dcoord;

	return(output);
}

VS_INSTANCED_TEXTURED_LIGHTING_ANIMATION_OUTPUT VSInstancedTexturedLightingAnimation(VS_INSTANCED_TEXTURED_LIGHTING_ANIMATION_INPUT input)
{
	VS_INSTANCED_TEXTURED_LIGHTING_ANIMATION_OUTPUT output = (VS_INSTANCED_TEXTURED_LIGHTING_ANIMATION_OUTPUT)0;


	float3 Src = input.position;

	if (input.boneindex.x < NULL_IDX)//ボーンインデックスがある場合
	{
		float4x4 mtx = (float4x4)0;
		float weight[4];
		weight[0] = input.boneweight.x;
		weight[1] = input.boneweight.y;
		weight[2] = input.boneweight.z;
		weight[3] = input.boneweight.w;
		float TotalWeight = 0.0f;
		uint bone[4];
		bone[0] = input.boneindex.x;
		bone[1] = input.boneindex.y;
		bone[2] = input.boneindex.z;
		bone[3] = input.boneindex.w;
		
		for (int i = 0; i<4; ++i)
		{
			if (weight[i]>0.0f)
				mtx += loadBoneMatrix(input.animationdata.x, bone[i]) * weight[i];
		}
		Src = mul(float4(input.position, 1.0f), mtx);
	}
	output.normalW = mul(input.normal, (float3x3)input.mtxTransform);
	output.positionW = mul(float4(input.position, 1.0f), input.mtxTransform).xyz;
	matrix mtxWorldViewProjection = mul(input.mtxTransform, gmtxView);
	mtxWorldViewProjection = mul(mtxWorldViewProjection, gmtxProjection);
	output.position = mul(float4(Src, 1.0f), mtxWorldViewProjection);
	output.tex2dcoord = input.tex2dcoord;

	return(output);
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------

float4 PSTexturedColor(VS_TEXTURED_COLOR_OUTPUT input) : SV_Target
{
	float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dCoord);

	return(cColor);
}

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
		float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord) * cIllumination;

		return(cColor);
}

float4 PSInstancedTexturedLighting(VS_INSTANCED_TEXTURED_LIGHTING_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
		float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord) * cIllumination;

		return(cColor);
}

float4 PSInstancedTexturedLightingAnimation(VS_INSTANCED_TEXTURED_LIGHTING_ANIMATION_OUTPUT input) : SV_Target
{
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW);
		float4 cColor = gtxtTexture.Sample(gSamplerState, input.tex2dcoord) * cIllumination;

		return(cColor);
}

