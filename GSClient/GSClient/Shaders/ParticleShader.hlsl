
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
	float3 randomValues : RVALUE;
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
	  
	VS_PARTICLE_OUT outRes;

	float emitTime = input.time.x;
	float lifeTime = input.time.y;

	float newTime = (gfTime - emitTime); 
	//newTime = fmod(newTime, lifeTime); 
	if (newTime > lifeTime)
		newTime = -1.0f;
	if (newTime > 0.0f) 
	{ 
		float t = newTime;
		float tt = newTime * newTime; 

		matrix copyMat = gmtxWorld;
		float3 newAcc = float3(0, -0.0f, 0.0f);
		float toDegree = degrees(2 * 3.14 * input.randomValues.x); 
		float circleSize = input.randomValues.y;
		  
		float3 directionVec = float3(copyMat._21, copyMat._22, copyMat._23);
		float speedLength = length(input.speed);
		float3 speed = directionVec * speedLength;

		float3 objPos = float3(copyMat._41, copyMat._42, copyMat._43);
		float3 position = input.position + objPos; 
		position.x = position.x + cos(toDegree) * circleSize;
		position.y = position.y + sin(toDegree) * circleSize; 
		position = position + t * speed + tt * newAcc * 0.5f;

		copyMat._22 = 1.0f;
		copyMat._21 = copyMat._23 = copyMat._24 = 0.0f;
		copyMat._41 = position.x;
		copyMat._42 = position.y;
		copyMat._43 = position.z;

		outRes.position = mul(mul(mul(float4(input.position, 1.0f), copyMat), gmtxView), gmtxProjection);   
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
	//cColor = 1.0f;
	return cColor;
}