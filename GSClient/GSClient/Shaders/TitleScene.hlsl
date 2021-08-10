
//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameOBJInfo : register(b0)
{
	matrix	gmtxWorld : packoffset(c0);
	uint	gnTexturesMask : packoffset(c4.x);
	uint	gnMaterialID : packoffset(c4.y);
};

//게임 씬의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbSceneFrameData : register(b1)
{
	uint	gnHP : packoffset(c0.r);
	uint	gnSP: packoffset(c0.g);
	uint	gnWeapon: packoffset(c0.b);
	float   gfTime : packoffset(c0.a);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCameraInfo : register(b2)
{
	matrix	gmtxView : packoffset(c0);
	matrix	gmtxProjection : packoffset(c4);
	float3	gvCameraPosition : packoffset(c8);
};

cbuffer cbLightCameraInfo : register(b3)
{
	matrix gmtxViewProjection : packoffset(c0);
	matrix gmtxShadowTransform : packoffset(c4);
};
cbuffer cbBoneOffsets : register(b6)
{
	float4x4 gpmtxBoneOffsets[128];
};

cbuffer cbBoneTransforms : register(b7)
{
	float4x4 gpmtxBoneTransforms[128];
};

cbuffer cbFontTransform : register(b8)
{
	float4x4 wvpMat;
};


#define MATERIAL_ALBEDO_MAP			0x01
#define MATERIAL_SPECULAR_MAP		0x02
#define MATERIAL_NORMAL_MAP			0x04
#define MATERIAL_METALLIC_MAP		0x08
#define MATERIAL_EMISSION_MAP		0x10
#define MATERIAL_DETAIL_ALBEDO_MAP	0x20
#define MATERIAL_DETAIL_NORMAL_MAP	0x40

SamplerState gssWrap : register(s0);
SamplerState gssClamp : register(s1);
SamplerComparisonState gscsShadow : register(s2); // 그림자
 
Texture2D gtxtMultipleButton : register(t0);
Texture2D gtxtSimpleButton   : register(t1);
Texture2D gtxtTitle		     : register(t2);
Texture2D gtxtRoomBoard		 : register(t3);

Texture2D gtxtRocky_Terrain: register(t4);
Texture2D gtxtBossWall	   : register(t5); // 수정요망  

struct VS_TEXTURE_IN
{
	float3 position : POSITION;
	float2 uv		: TEXCOORD;
};
struct VS_TEXTURE_OUT
{
	float4 position : SV_POSITION;
	float2 uv	 : TEXCOORD;
};

VS_TEXTURE_OUT VS_UI_Textured(VS_TEXTURE_IN input)
{
	VS_TEXTURE_OUT outRes;
	outRes.position = mul(float4(input.position, 1.0f), gmtxWorld);
	outRes.uv = input.uv;
	return outRes;
}

float4 PS_UI_Textured(VS_TEXTURE_OUT input) : SV_TARGET
{
	float4 cColor;

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtTitle.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x02)
	{
		cColor = gtxtTitle.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x04)
	{
		cColor = gtxtTitle.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x08)
	{

		cColor = gtxtTitle.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x10)
	{
		cColor = gtxtTitle.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x20)
	{
		cColor = gtxtTitle.Sample(gssWrap, input.uv);
	}
	return cColor;
}

float4 PS_UI_HelpText(VS_TEXTURE_OUT input) : SV_TARGET
{
	float4 cColor;

	if (gnTexturesMask & 0x01)
	{
		input.uv.y += 0.1;
	}
	if (gnTexturesMask & 0x02)
	{
		input.uv.y += 0.2;
	}
	if (gnTexturesMask & 0x04)
	{
		input.uv.y += 0.3;
	}
	if (gnTexturesMask & 0x08)
	{
		input.uv.y += 0.4;
	}
	if (gnTexturesMask & 0x10)
	{
		input.uv.y += 0.5;
	}
	if (gnTexturesMask & 0x20)
	{
		input.uv.y += 0.6;
	} 
	return cColor;
}