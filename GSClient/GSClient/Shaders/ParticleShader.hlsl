
SamplerState gssWrap : register(s0);
SamplerState gssClamp : register(s1);

Texture2D gtxtForest	   : register(t0);
Texture2D gtxtDryForest	   : register(t1);
Texture2D gtxtDesert	   : register(t2);
Texture2D gtxtDryDesert	   : register(t3);
Texture2D gtxtRocky_Terrain: register(t4);
Texture2D gtxtBossWall	   : register(t5); // 수정요망

Texture2D gSkyBox_Front    : register(t6);
Texture2D gSkyBox_Back     : register(t7);
Texture2D gSkyBox_Right    : register(t8);
Texture2D gSkyBox_Left     : register(t9);
Texture2D gSkyBox_Top      : register(t10);
Texture2D gSkyBox_Bottom   : register(t11);

Texture2D gtxtBox          : register(t12);
Texture2D gtxtWood         : register(t13);
Texture2D gtxtWoodSignBoard: register(t14);
Texture2D gtxtGrassWall    : register(t15);
Texture2D gtxtSandWall     : register(t16);
Texture2D gtxtRockyWall    : register(t17);
Texture2D gtxtDoor         : register(t18);

Texture2D gtxtHpSpGauge    : register(t19);
Texture2D gtxtHpSpPer      : register(t20);
Texture2D gtxtMinimap      : register(t21);
Texture2D gtxtWeapons      : register(t22);

Texture2D gtxtFlower_Red   : register(t23); // 여분
Texture2D gtxtFlower_White : register(t24); // 여분
Texture2D gtxtGrass_Width  : register(t25); // 여분
Texture2D gtxtGrass_Depth  : register(t26); // 여분
Texture2D gtxtTree         : register(t27);
Texture2D gtxtNoLeafTrees  : register(t28);
Texture2D gtxtLeaves       : register(t29);
Texture2D gtxtMoss_Rock    : register(t30);

Texture2D gtxtPuzzleBoard  : register(t31);
Texture2D gtxtHelpText     : register(t32);
Texture2D gtxtDry_Tree	   : register(t33);
Texture2D gtxtStump		   : register(t34);
Texture2D gtxtDead_Tree	   : register(t35);
Texture2D gtxtDesert_Rock  : register(t36);

Texture2D gtxtMap          : register(t37);
Texture2D gtxtMirror       : register(t38);

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

struct VS_TEX_PARTICLE_IN
{
	float3 position : POSITION;
	float3 speed    : SPEED;
	float2 time     : time;
	float3 randomValues : RVALUE;
	float2 uv	 : TEXCOORD;	
	uint index : TEXTURE;
};
struct VS_PARTICLE_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR; 
	float2 time     : time;
};

struct VS_TEX_PARTICLE_OUT
{
	float4 position : SV_POSITION;
	float2 time     : time; 
	float2 uv	 : TEXCOORD;
	uint index : TEXTURE;
};
//////////////////////////////////////////////////////////////////////
//
VS_PARTICLE_OUT VSArrowParticle(VS_PARTICLE_IN input)
{ 
	VS_PARTICLE_OUT outRes;

	float emitTime = input.time.x;
	float lifeTime = input.time.y;

	float newTime = (gfTime - emitTime);  
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
	float intensity = 1 - (newTime / lifeTime);
	outRes.color = input.color;
	outRes.color.a = intensity;
	outRes.time = input.time;
	return outRes;
}

float4 PSParticle(VS_PARTICLE_OUT input) : SV_TARGET
{
	float4 cColor = input.color;
	//cColor = 1.0f;
	return cColor;
}

//////////////////////////////////////////////////////////////////////
//
VS_TEX_PARTICLE_OUT VSTexParticle(VS_TEX_PARTICLE_IN input)
{
	VS_TEX_PARTICLE_OUT outRes;

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
	outRes.time = input.time;
	outRes.uv = input.uv;
	outRes.index = input.index;
	return outRes;
}

float4 PSTexParticle(VS_TEX_PARTICLE_OUT input) : SV_TARGET
{
	float4 cColor;

	if (input.index & 0x01)
	{
		cColor = gtxtBox.Sample(gssWrap, input.uv);
	}
	//cColor = 1.0f;
	return cColor;
}