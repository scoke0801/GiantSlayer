
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

struct VS_PARTICLE_IN
{ 
	float3 position : POSITION;
	float4 color    : COLOR;
	float3 speed    : SPEED;
	float2 time     : time;
};
struct VS_PARTICLE_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR; 
	float2 time     : time;
};

//////////////////////////////////////////////////////////////////////
//
VS_PARTICLE_OUT VSParticle(VS_PARTICLE_IN input)
{
	float3 position = float3(gmtxWorld._41, gmtxWorld._42, gmtxWorld._43);
	float parameter = gmtxWorld._31;

	VS_PARTICLE_OUT outRes;
	outRes.position.x = position.x + cos(2 * 3.14 * parameter);
	outRes.position = float4(input.position, 1.0f);
	outRes.color = input.color;
	outRes.time = input.time;
	return outRes;
}

float4 PSParticle(VS_PARTICLE_OUT input) : SV_TARGET
{
	float4 cColor = 0.0f;
	return cColor;
}