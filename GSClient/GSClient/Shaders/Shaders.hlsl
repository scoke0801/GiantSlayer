#pragma
#pragma enable_d3d12_debug_symbols

//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameObjectInfo : register(b0)
{
	matrix	gmtxWorld : packoffset(c0);
	uint	gnTexturesMask : packoffset(c4);	
	uint	gnMaterialID : packoffset(c4.y);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCameraInfo : register(b1)
{
	matrix	gmtxView : packoffset(c0);
	matrix	gmtxProjection : packoffset(c4);
	float3	gvCameraPosition : packoffset(c8);
};

#include "Light.hlsl"

SamplerState gssWrap : register(s0);
SamplerState gssClamp : register(s1);

Texture2D gtxtTerrain : register(t0);
Texture2D gSkyBox_Front : register(t1);
Texture2D gSkyBox_Back : register(t2);
Texture2D gSkyBox_Right : register(t3);
Texture2D gSkyBox_Left : register(t4);
Texture2D gSkyBox_Top : register(t5);
Texture2D gSkyBox_Bottom : register(t6);
Texture2D gtxtBox : register(t7);

//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다.
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//정점 셰이더를 정의한다. 
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	//정점을 변환(월드 변환, 카메라 변환, 투영 변환)한다. 
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	output.color = input.color;
	return(output);
}

//픽셀 셰이더를 정의한다. 
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}



struct VS_TEXTURE_IN
{
	float3 position : POSITION;
	float2 uv		: TEXCORD;
};
struct VS_TEXTURE_OUT
{
	float4 position : SV_POSITION;
	float2 uv	 : TEXCOORD;
};

VS_TEXTURE_OUT VSTextured(VS_TEXTURE_IN input)
{
	VS_TEXTURE_OUT outRes;
	outRes.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	outRes.uv = input.uv;
	return outRes;
}

float4 PSTextured(VS_TEXTURE_OUT input) : SV_TARGET
{
	float4 cColor;

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtTerrain.Sample(gssClamp, input.uv);
	}

	if (gnTexturesMask & 0x02)
	{
		cColor = gSkyBox_Front.Sample(gssClamp, input.uv);
	}
	if (gnTexturesMask & 0x04)
	{
		cColor = gSkyBox_Back.Sample(gssClamp, input.uv);
	}

	if (gnTexturesMask & 0x08)
	{
		cColor = gSkyBox_Right.Sample(gssClamp, input.uv);
	}

	if (gnTexturesMask & 0x10)
	{
		cColor = gSkyBox_Left.Sample(gssClamp, input.uv);
	}

	if (gnTexturesMask & 0x20)
	{
		cColor = gSkyBox_Top.Sample(gssClamp, input.uv);
	}

	if (gnTexturesMask & 0x40)
	{
		cColor = gSkyBox_Bottom.Sample(gssClamp, input.uv);
	}
	if (gnTexturesMask & 0x80) 
	{
		cColor = gtxtBox.Sample(gssWrap, input.uv);
	}
	return cColor; 
}

///////////////////////////////////////////
// VS
struct VS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float3 positionW : POSITION1;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
	float2 uv1 : TEXCOORD1;
};

VS_TERRAIN_TESSELLATION_OUTPUT VSTerrainTessellation(VS_INPUT input)
{
	VS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input.position;
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.color = input.color;

	return(output);
}



struct HS_TERRAIN_TESSELLATION_CONSTANT
{
	float fTessEdges[4] : SV_TessFactor;
	float fTessInsides[2] : SV_InsideTessFactor;
};

struct HS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

struct DS_TERRAIN_TESSELLATION_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float4 tessellation : TEXCOORD2;
};

void BernsteinCoeffcient5x5(float t, out float fBernstein[5])
{
	float tInv = 1.0f - t;
	fBernstein[0] = tInv * tInv * tInv * tInv;
	fBernstein[1] = 4.0f * t * tInv * tInv * tInv;
	fBernstein[2] = 6.0f * t * t * tInv * tInv;
	fBernstein[3] = 4.0f * t * t * t * tInv;
	fBernstein[4] = t * t * t * t;
}

float3 CubicBezierSum5x5(OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch, float uB[5], float vB[5])
{
	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
	f3Sum = vB[0] * (uB[0] * patch[0].position + uB[1] * patch[1].position + uB[2] * patch[2].position + uB[3] * patch[3].position + uB[4] * patch[4].position);
	f3Sum += vB[1] * (uB[0] * patch[5].position + uB[1] * patch[6].position + uB[2] * patch[7].position + uB[3] * patch[8].position + uB[4] * patch[9].position);
	f3Sum += vB[2] * (uB[0] * patch[10].position + uB[1] * patch[11].position + uB[2] * patch[12].position + uB[3] * patch[13].position + uB[4] * patch[14].position);
	f3Sum += vB[3] * (uB[0] * patch[15].position + uB[1] * patch[16].position + uB[2] * patch[17].position + uB[3] * patch[18].position + uB[4] * patch[19].position);
	f3Sum += vB[4] * (uB[0] * patch[20].position + uB[1] * patch[21].position + uB[2] * patch[22].position + uB[3] * patch[23].position + uB[4] * patch[24].position);

	return(f3Sum);
}

float CalculateTessFactor(float3 f3Position)
{
	float fDistToCamera = distance(f3Position, gvCameraPosition);
	float s = saturate((fDistToCamera - 10.0f) / (500.0f - 10.0f));

	return(lerp(64.0f, 5.0f, s));
}

// HS
[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(25)]
[patchconstantfunc("HSTerrainTessellationConstant")]
[maxtessfactor(64.0f)]
HS_TERRAIN_TESSELLATION_OUTPUT HSTerrainTessellation(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input, uint i : SV_OutputControlPointID)
{
	HS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input[i].position;
	output.color = input[i].color;

	return(output);
}

HS_TERRAIN_TESSELLATION_CONSTANT HSTerrainTessellationConstant(InputPatch<VS_TERRAIN_TESSELLATION_OUTPUT, 25> input)
{
	HS_TERRAIN_TESSELLATION_CONSTANT output;

	float3 e0 = 0.5f * (input[0].positionW + input[4].positionW);
	float3 e1 = 0.5f * (input[0].positionW + input[20].positionW);
	float3 e2 = 0.5f * (input[4].positionW + input[24].positionW);
	float3 e3 = 0.5f * (input[20].positionW + input[24].positionW);

	output.fTessEdges[0] = CalculateTessFactor(e0);
	output.fTessEdges[1] = CalculateTessFactor(e1);
	output.fTessEdges[2] = CalculateTessFactor(e2);
	output.fTessEdges[3] = CalculateTessFactor(e3);

	float3 f3Sum = float3(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < 25; i++)
		f3Sum += input[i].positionW;
	float3 f3Center = f3Sum / 25.0f;
	output.fTessInsides[0] = output.fTessInsides[1] = CalculateTessFactor(f3Center);

	return (output);
}

// DS
[domain("quad")]
DS_TERRAIN_TESSELLATION_OUTPUT DSTerrainTessellation(HS_TERRAIN_TESSELLATION_CONSTANT patchConstant, float2 uv : SV_DomainLocation, OutputPatch<HS_TERRAIN_TESSELLATION_OUTPUT, 25> patch)
{
	DS_TERRAIN_TESSELLATION_OUTPUT output = (DS_TERRAIN_TESSELLATION_OUTPUT)0;

	float uB[5], vB[5];
	BernsteinCoeffcient5x5(uv.x, uB);
	BernsteinCoeffcient5x5(uv.y, vB);

	output.color = lerp(lerp(patch[0].color, patch[4].color, uv.x), lerp(patch[20].color, patch[24].color, uv.x), uv.y);

	float3 position = CubicBezierSum5x5(patch, uB, vB);
	matrix mtxWorldViewProjection = mul(mul(gmtxWorld, gmtxView), gmtxProjection);
	output.position = mul(float4(position, 1.0f), mtxWorldViewProjection);

	output.tessellation = float4(patchConstant.fTessEdges[0], patchConstant.fTessEdges[1], patchConstant.fTessEdges[2], patchConstant.fTessEdges[3]);

	return(output);
}

// PS
float4 PSTerrainTessellation(DS_TERRAIN_TESSELLATION_OUTPUT input) : SV_TARGET
{
	float4 cColor = float4(0.0f, 0.0f, 0.0f, 1.0f);

	if (input.tessellation.w <= 5.0f) cColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	else if (input.tessellation.w <= 10.0f) cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	else if (input.tessellation.w <= 20.0f) cColor = float4(0.0f, 0.0f, 1.0f, 1.0f);
	else if (input.tessellation.w <= 30.0f) cColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
	else if (input.tessellation.w <= 40.0f) cColor = float4(1.0f, 1.0f, 0.0f, 1.0f);
	else if (input.tessellation.w <= 50.0f) cColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	else if (input.tessellation.w <= 55.0f) cColor = float4(0.2f, 0.2f, 0.72f, 1.0f);
	else if (input.tessellation.w <= 60.0f) cColor = float4(0.5f, 0.75f, 0.75f, 1.0f);
	else cColor = float4(0.87f, 0.17f, 1.0f, 1.0f);

	return (input.color);
}