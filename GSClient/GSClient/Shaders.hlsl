
//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
	uint	gnTexturesMask : packoffset(c4);
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


Texture2D gtxtBox : register(t0);
Texture2D gSkyBox_Front : register(t1);
Texture2D gSkyBox_Back : register(t2);
Texture2D gSkyBox_Right : register(t3);
Texture2D gSkyBox_Left : register(t4);
Texture2D gSkyBox_Top : register(t5);
Texture2D gSkyBox_Bottom : register(t6);

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

	if (gnTexturesMask & 0x00)
	{
	cColor = gtxtBox.Sample(gssWrap, input.uv);
	}

	if (gnTexturesMask & 0x01)
	{
		cColor = gSkyBox_Front.Sample(gssWrap, input.uv);
	}
	if (gnTexturesMask & 0x02)
	{
		cColor = gSkyBox_Back.Sample(gssWrap, input.uv);
	}

	if (gnTexturesMask & 0x03)
	{
		cColor = gSkyBox_Right.Sample(gssWrap, input.uv);
	}

	if (gnTexturesMask & 0x04)
	{
		cColor = gSkyBox_Left.Sample(gssWrap, input.uv);
	}

	if (gnTexturesMask & 0x05)
	{
		cColor = gSkyBox_Top.Sample(gssWrap, input.uv);
	}

	if (gnTexturesMask & 0x06)
	{
		cColor = gSkyBox_Bottom.Sample(gssWrap, input.uv);
	}

	return cColor;
	//return gtxtBox.Sample(gssWrap, input.uv);
}

//Texture2D gtxtSkyBox : register(t8);
//
//float4 PSSkyBox(VS_TEXTURED_OUTPUT input) : SV_TARGET
//{
//	float4 cColor = gtxtSkyBox.Sample(gClampSamplerState, input.uv);
//
//	return(cColor);
//}