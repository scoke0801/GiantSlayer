
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
	uint	gnHP : packoffset(c0.x);
	uint	gnSP : packoffset(c0.y);
	uint	gnWeapon : packoffset(c0.z);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCameraInfo : register(b2)
{
	matrix	gmtxView : packoffset(c0);
	matrix	gmtxProjection : packoffset(c4);
	float3	gvCameraPosition : packoffset(c8);
};

SamplerState gssWrap : register(s0);
SamplerState gssClamp : register(s1);

Texture2D gtxtForest : register(t0);
Texture2D gtxtDryForest : register(t1);
Texture2D gtxtDesert : register(t2);
Texture2D gtxtDryDesert : register(t3);
Texture2D gtxtRocky_Terrain : register(t4);

Texture2D gSkyBox_Front    : register(t5);
Texture2D gSkyBox_Back     : register(t6);
Texture2D gSkyBox_Right    : register(t7);
Texture2D gSkyBox_Left     : register(t8);
Texture2D gSkyBox_Top      : register(t9);
Texture2D gSkyBox_Bottom   : register(t10);

Texture2D gtxtBox          : register(t11);
Texture2D gtxtWood         : register(t12);
Texture2D gtxtWoodSignBoard: register(t13);
Texture2D gtxtGrassWall    : register(t14);
Texture2D gtxtSandWall     : register(t15); 
Texture2D gtxtRockyWall    : register(t16);
Texture2D gtxtDoor         : register(t17);

Texture2D gtxtHpSpGauge    : register(t18);
Texture2D gtxtHpSpPer      : register(t19);
Texture2D gtxtMinimap      : register(t20);
Texture2D gtxtWeapons      : register(t21);

Texture2D gtxtFlower_Red   : register(t22);
Texture2D gtxtFlower_White : register(t23);
Texture2D gtxtGrass_Width  : register(t24);
Texture2D gtxtGrass_Depth  : register(t25);
Texture2D gtxtTree         : register(t26);
Texture2D gtxtNoLeafTrees  : register(t27);
Texture2D gtxtLeaves       : register(t28);
Texture2D gtxtMoss_Rock    : register(t29);

Texture2D gtxtPuzzleBoard  : register(t30);
Texture2D gtxtHelpText     : register(t31);
Texture2D gtxtDry_Tree	   : register(t32);
Texture2D gtxtStump		   : register(t33);
Texture2D gtxtDead_Tree	   : register(t34);

Texture2D gtxtMap          : register(t35);
Texture2D gtxtMirror       : register(t36);
 
//정점 셰이더의 입력을 위한 구조체를 선언한다. 
struct VS_COLOR_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다.
struct VS_COLOR_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

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

//////////////////////////////////////////////////////////////////////
//
VS_COLOR_OUTPUT VSColor(VS_COLOR_INPUT input)
{
	input.position.x += gmtxWorld._41;
	input.position.y += gmtxWorld._42;
	input.position.z += gmtxWorld._43;

	VS_COLOR_OUTPUT outRes;
	outRes.position = float4(input.position, 1.0f);
	outRes.color = input.color;
	return outRes;
}

float4 PSColor(VS_COLOR_OUTPUT input) : SV_TARGET
{
	float4 cColor = input.color;
	return cColor;
}
VS_COLOR_OUTPUT VSBasic(VS_COLOR_INPUT input)
{
	VS_COLOR_OUTPUT outRes;
	outRes.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	return outRes;
}
float4 PSBasic(VS_COLOR_OUTPUT input) : SV_TARGET
{
	float4 cColor = float4(1.0f, 0.0f, 0.0f, 0.0f);
	return cColor;
}
//////////////////////////////////////////////////////////////////////
//
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
		cColor = gtxtForest.Sample(gssClamp, input.uv);
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
	if (gnTexturesMask & 0x100)
	{
		cColor = gtxtWood.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x200)
	{
		cColor = gtxtGrassWall.Sample(gssWrap, input.uv);
	}
	return cColor;
}

/////////////////////////////////////////////////////////////////
/////

struct VS_BILLBOARD_INPUT
{
	float3 center : POSITION;
	float2 size : TEXCOORD;
	uint index : TEXTURE;
};

VS_BILLBOARD_INPUT VSBillboard(VS_BILLBOARD_INPUT input)
{
	input.center.x = gmtxWorld._41;
	input.center.y = gmtxWorld._42;
	input.center.z = gmtxWorld._43;
	return(input);
}
 
struct GS_BILLBOARD_GEOMETRY_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
	uint index : TEXTURE;
};

static float2 pf2UVs[4] = { float2(0.0f,1.0f), float2(0.0f,0.0f), float2(1.0f,1.0f), float2(1.0f,0.0f) };

[maxvertexcount(4)]
void GSBillboard(point VS_BILLBOARD_INPUT input[1], inout TriangleStream<GS_BILLBOARD_GEOMETRY_OUTPUT> outStream)
{
	float3 f3Up = float3(0.0f, 1.0f, 0.0f);
	float3 f3Look = normalize(gvCameraPosition - input[0].center.xyz);
	//float3 f3Look = normalize(input[0].center.xyz);
	float3 f3Right = cross(f3Up, f3Look);
	float fHalfWidth = input[0].size.x * 0.5f;
	float fHalfHeight = input[0].size.y * 0.5f;

	float4 pf4Vertices[4];
	pf4Vertices[0] = float4(input[0].center.xyz + (fHalfWidth * f3Right)*gmtxWorld._11 - (fHalfHeight * f3Up)*gmtxWorld._22, 1.0f);
	pf4Vertices[1] = float4(input[0].center.xyz + (fHalfWidth * f3Right)*gmtxWorld._11 + (fHalfHeight * f3Up)*gmtxWorld._22, 1.0f);
	pf4Vertices[2] = float4(input[0].center.xyz - (fHalfWidth * f3Right)*gmtxWorld._11 - (fHalfHeight * f3Up)*gmtxWorld._22, 1.0f);
	pf4Vertices[3] = float4(input[0].center.xyz - (fHalfWidth * f3Right)*gmtxWorld._11 + (fHalfHeight * f3Up)*gmtxWorld._22, 1.0f);

	GS_BILLBOARD_GEOMETRY_OUTPUT output;
	for (int i = 0; i < 4; i++)
	{
		output.positionW = pf4Vertices[i].xyz;
		
        //outRes.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
        output.position = mul(mul(pf4Vertices[i], gmtxView), gmtxProjection);
        //output.position = mul(mul(mul(pf4Vertices[i], gmtxWorld), gmtxView), gmtxProjection);
		output.normal = f3Look;
		output.uv = pf2UVs[i];
		output.index = input[0].index;

		outStream.Append(output);
	}
}

float4 PSBillboard(GS_BILLBOARD_GEOMETRY_OUTPUT input) : SV_TARGET
{
	// gtxtBillboardTextures[input.index].Sample(gssClamp, input.uv); 
	// 위 형식은 빌보드 이미지를 배열형식으로 불러오는 경우에 사용가능
	float4 cColor;
	
	if (gnTexturesMask & 0x01)
	{
        cColor = gtxtFlower_Red.Sample(gssClamp, input.uv);
    }
    if (gnTexturesMask & 0x02)
    {
        cColor = gtxtFlower_White.Sample(gssClamp, input.uv);
    }
    if (gnTexturesMask & 0x04)
    {
        cColor = gtxtGrass_Width.Sample(gssClamp, input.uv);
    }
    if (gnTexturesMask & 0x08)
    {
        cColor = gtxtGrass_Depth.Sample(gssClamp, input.uv);
    }
    if (gnTexturesMask & 0x10)
    {
        cColor = gtxtTree.Sample(gssClamp, input.uv);
    }
    if (gnTexturesMask & 0x20)
    {
        cColor = gtxtNoLeafTrees.Sample(gssClamp, input.uv);
    }
	
	if (cColor.a <= 0.3f) discard; //clip(cColor.a - 0.3f);

	return(cColor);
}

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
		cColor = gtxtHpSpGauge.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x02)
	{
		input.uv.y += 0.5;
		cColor = gtxtHpSpGauge.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x04)
	{
		cColor = gtxtHpSpPer.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x08)
	{

	}
	if (gnTexturesMask & 0x10)
	{
		if (gnWeapon & 0x02)
		{
			input.uv.y += 0.5;
		}
		cColor = gtxtWeapons.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x20)
	{
		cColor = gtxtMinimap.Sample(gssWrap, input.uv);
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
	cColor = gtxtHelpText.Sample(gssWrap, input.uv);
	return cColor;
}
/////////////////////////////////////////////////////
// Minimap

struct VS_MIN
{
	float3 position : POSITION;
	float2 uv		: TEXCOORD;
};
struct VS_MOUT
{
	float4 position : SV_POSITION;
	float2 uv	 : TEXCOORD;
};

VS_MOUT VSMinimap(VS_MIN input)
{
	VS_MOUT outRes;
	outRes.position = mul(float4(input.position, 1.0f), gmtxWorld);
	outRes.uv = input.uv;
	return outRes;
}

float4 PSMinimap(VS_MOUT input) : SV_TARGET
{
	float4 cColor;
	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtMinimap.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x02)
	{
		cColor = gtxtMap.Sample(gssWrap, input.uv);
	}
	return cColor;
}

//////////////////////////////////////////////////////
//

struct VS_TERRAIN_INPUT
{
	float3 position : POSITION;
	float2 uv0 : TEXCOORD0;
	float4 color : COLOR;
};

struct VS_TERRAIN_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv0 : TEXCOORD0;
	float4 color : COLOR;
};

///////////////////////////////////////////
// VS
struct VS_TERRAIN_TESSELLATION_OUTPUT
{
	float3 position : POSITION;
	float3 positionW : POSITION1;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
};

VS_TERRAIN_TESSELLATION_OUTPUT VSTerrainTessellation(VS_TERRAIN_INPUT input)
{
	VS_TERRAIN_TESSELLATION_OUTPUT output;

	output.position = input.position;
	output.positionW = mul(float4(input.position, 1.0f), gmtxWorld).xyz;
	output.color = input.color;
	output.uv0 = input.uv0;

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
	float2 uv0 : TEXCOORD0;
};

struct DS_TERRAIN_TESSELLATION_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 uv0 : TEXCOORD0;
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
	float s = saturate((fDistToCamera - 10.0f) / (10000.0f - 10.0f));

	return(lerp(64.0f, 1.0f, s));
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
	output.uv0 = input[i].uv0;

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
	output.uv0 = lerp(lerp(patch[0].uv0, patch[4].uv0, uv.x), lerp(patch[20].uv0, patch[24].uv0, uv.x), uv.y);

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

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtForest.Sample(gssWrap, input.uv0);
	}
    if (gnTexturesMask & 0x02)
    {
        cColor = gtxtDryForest.Sample(gssWrap, input.uv0);
    }
    if (gnTexturesMask & 0x04)
    {
        cColor = gtxtDesert.Sample(gssWrap, input.uv0);
    }
    if (gnTexturesMask & 0x08)
    {
        cColor = gtxtDryDesert.Sample(gssWrap, input.uv0);
    }
    if (gnTexturesMask & 0x10)
    {
        cColor = gtxtRocky_Terrain.Sample(gssWrap, input.uv0);
    }
	
	//else
	//{
	//	cColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
	//}

	return (cColor);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
#include "Light.hlsl"

struct VS_LIGHT_OUT
{
	float4 PosH    : SV_POSITION;
	float3 PosW    : POSITION;
	float2 uv	   : TEXCOORD;
	float3 Normal  : NORMAL;
};
struct VS_TEXTURED_LIGHTING_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
	float4 position : SV_POSITION;
	float3 positionW : POSITION;
	float3 normalW : NORMAL;
	float2 uv : TEXCOORD;
};

VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
	VS_TEXTURED_LIGHTING_OUTPUT output;

	output.normalW = mul(input.normal, (float3x3)gmtxWorld);
	output.positionW = (float3)mul(float4(input.position, 1.0f), gmtxWorld);
	output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor;// = gtxtBox.Sample(gssWrap, uvw);

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtForest.Sample(gssClamp, input.uv);
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
	if (gnTexturesMask & 0x100)
	{
		cColor = gtxtWood.Sample(gssWrap, input.uv);
	}
   
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW, gnMaterialID);

	return(cColor * cIllumination);
}

float4 PSDoorWall(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor;// = gtxtBox.Sample(gssWrap, uvw);

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtGrassWall.Sample(gssWrap, input.uv);
	}

	if (gnTexturesMask & 0x02)
	{
		cColor = gtxtSandWall.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x04)
	{
		cColor = gtxtDryDesert.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x08)
	{
		cColor = gtxtRockyWall.Sample(gssWrap, input.uv);
	}

	if (gnTexturesMask & 0x10)
	{
		cColor = gtxtDoor.Sample(gssWrap, input.uv);
	}
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW, gnMaterialID);

	return(cColor * cIllumination);
}

float4 PSBridgeLight(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor;// = gtxtBox.Sample(gssWrap, uvw);

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtWood.Sample(gssClamp, uvw);
	}

	if (gnTexturesMask & 0x02)
	{
		cColor = gtxtBox.Sample(gssClamp, uvw);
	}
	if (gnTexturesMask & 0x04)
	{
		cColor = gtxtBox.Sample(gssClamp, uvw);
	}

	if (gnTexturesMask & 0x08)
	{
		cColor = gtxtBox.Sample(gssClamp, uvw);
	}

	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW, gnMaterialID);

	return(cColor * cIllumination);
}


float4 PSPuzzle(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor = gtxtWood.Sample(gssWrap, uvw);

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtWood.Sample(gssClamp, uvw);
	}
	if (gnTexturesMask & 0x02)
	{
		cColor = gtxtBox.Sample(gssClamp, uvw);
	}
	if (gnTexturesMask & 0x04)
	{
	}
	if (gnTexturesMask & 0x08)
	{
		cColor = gtxtPuzzleBoard.Sample(gssClamp, uvw);
	}
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW, gnMaterialID);

	return(cColor * cIllumination);
}
float4 PSSign(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor = gtxtWood.Sample(gssWrap, uvw);

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtBox.Sample(gssClamp, uvw);
	}
	if (gnTexturesMask & 0x02)
	{
		cColor = gtxtWoodSignBoard.Sample(gssClamp, uvw);
	}
	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW, gnMaterialID);

	return(cColor * cIllumination);
} 

float4 PSMirror(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor;// = gtxtBox.Sample(gssWrap, uvw);

	if (gnTexturesMask & 0x01)
	{
		cColor = cColor = gtxtMirror.Sample(gssWrap, input.uv);
	}

	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW, gnMaterialID);

	return(cColor * cIllumination);
}
float4 PSFBXFeatureShader(VS_TEXTURED_LIGHTING_OUTPUT input, uint nPrimitiveID : SV_PrimitiveID) : SV_TARGET
{
	float3 uvw = float3(input.uv, nPrimitiveID / 2);
	float4 cColor;// = gtxtBox.Sample(gssWrap, uvw);

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtLeaves.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x02)
	{
		cColor = gtxtMoss_Rock.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x04)
	{
		cColor = gtxtDry_Tree.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x08)
	{
		cColor = gtxtStump.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x10)
	{
		cColor = gtxtDead_Tree.Sample(gssWrap, input.uv);
	}

	input.normalW = normalize(input.normalW);
	float4 cIllumination = Lighting(input.positionW, input.normalW, gnMaterialID);

	return(cColor * cIllumination);
}