
//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameObjectInfo : register(b0)
{
	matrix	gmtxWorld : packoffset(c0);
	uint	gnTexturesMask : packoffset(c4.x);
	uint	gnMaterialID : packoffset(c4.y);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCameraInfo : register(b1)
{
	matrix	gmtxView : packoffset(c0);
	matrix	gmtxProjection : packoffset(c4);
	float3	gvCameraPosition : packoffset(c8);
};

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
Texture2D gtxtWood : register(t8);
Texture2D gtxtWall : register(t9);

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
	float2 uv		: TEXCORD;
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
	if (gnTexturesMask & 0x100)
	{
		cColor = gtxtWood.Sample(gssWrap, input.uv);
	}	
	if (gnTexturesMask & 0x200)
	{
		cColor = gtxtWall.Sample(gssWrap, input.uv);
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
	return(input);
}

VS_BILLBOARD_INPUT VSExBillboard(VS_BILLBOARD_INPUT input)
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
	pf4Vertices[0] = float4(input[0].center.xyz + (fHalfWidth * f3Right) - (fHalfHeight * f3Up), 1.0f);
	pf4Vertices[1] = float4(input[0].center.xyz + (fHalfWidth * f3Right) + (fHalfHeight * f3Up), 1.0f);
	pf4Vertices[2] = float4(input[0].center.xyz - (fHalfWidth * f3Right) - (fHalfHeight * f3Up), 1.0f);
	pf4Vertices[3] = float4(input[0].center.xyz - (fHalfWidth * f3Right) + (fHalfHeight * f3Up), 1.0f);

	GS_BILLBOARD_GEOMETRY_OUTPUT output;
	for (int i = 0; i < 4; i++)
	{
		output.positionW = pf4Vertices[i].xyz;
		output.position = mul(mul(pf4Vertices[i], gmtxView), gmtxProjection);
		output.normal = f3Look;
		output.uv = pf2UVs[i];
		output.index = input[0].index;

		outStream.Append(output);
	}
}
Texture2D gtxtPlayerInfoTest : register(t111);
Texture2D gtxtTextBGTest : register(t112);
Texture2D gtxtMinimapBGTest : register(t113);

float4 PSBillboard(GS_BILLBOARD_GEOMETRY_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtPlayerInfoTest.Sample(gssClamp, input.uv);
	if (cColor.a <= 0.3f) discard; //clip(cColor.a - 0.3f);

	return(cColor);
}
//
//float4 PSExBillboard(GS_BILLBOARD_GEOMETRY_OUTPUT input) : SV_TARGET
//{
//	float4 cColor = gtxtExBillboardTextures[input.index].Sample(gssClamp, input.uv);
//	if (cColor.a <= 0.3f) discard; //clip(cColor.a - 0.3f);
//
//	return(cColor);
//}

VS_TEXTURE_OUT VS_UI_Textured(VS_TEXTURE_IN input)
{
	input.position.x += gmtxWorld._41;
	input.position.y += gmtxWorld._42;
	input.position.z += gmtxWorld._43;
	VS_TEXTURE_OUT outRes;
	outRes.position = float4(input.position, 1.0f);
	outRes.uv = input.uv;
	return outRes;
}

float4 PS_UI_Textured(VS_TEXTURE_OUT input) : SV_TARGET
{
	float4 cColor;

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtPlayerInfoTest.Sample(gssWrap, input.uv);
		
	}
	if (gnTexturesMask & 0x02)
	{
		cColor = gtxtTextBGTest.Sample(gssWrap, input.uv); 
	}
	if (gnTexturesMask & 0x04)
	{
		cColor = gtxtMinimapBGTest.Sample(gssWrap, input.uv);
	}
	return cColor;
}