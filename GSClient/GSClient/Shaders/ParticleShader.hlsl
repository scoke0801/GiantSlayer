
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
	//copyMat._41 = position.x + cos(2 * 3.14 * parameter) * 10;
	//copyMat._42 = position.y + sin(2 * 3.14 * parameter) * 10;
	//copyMat._43 = position.z;
	//copyMat._31 = 0;

	
	//outRes.position._43 = input.position.z;

	//
	//

	VS_PARTICLE_OUT outRes;

	matrix copyMat = gmtxWorld;
	float3 position = input.position;
	float parameter = gmtxWorld._31;
	float emitTime = input.time.x;
	float lifeTime = input.time.y;

	float newTime = (gfTime - emitTime);
	newTime = fmod(newTime, lifeTime); 
	
	if (newTime > 0.0f) {
		float t = newTime;
		float tt = newTime * newTime; 
		//float3 temp = t * (float3(input.speed,1.0f));
		float newAcc = float3(0, -9.8f, 0.0f);
		position = position + t * input.speed + tt * newAcc * 0.5f;
		//position.x = position.x + cos(2 * 3.14 * parameter) * 1000;
		//position.y = position.y + sin(2 * 3.14 * parameter) * 1000;
		//position.z = position.z + newTime;
		outRes.position = mul(mul(mul(float4(position, 1.0f), copyMat), gmtxView), gmtxProjection);  
	}
	else {
		outRes.position = 0.0f;
	}
	outRes.color = input.color;
	outRes.time = input.time;
	return outRes;
}

float4 PSParticle(VS_PARTICLE_OUT input) : SV_TARGET
{
	float4 cColor = input.color; 
	return cColor;
}