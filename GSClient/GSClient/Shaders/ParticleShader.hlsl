
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
Texture2D gtxtMinimap		: register(t20);
Texture2D gtxtWeapons		: register(t21);
Texture2D gtxtHpSpPer      : register(t22);


Texture2D gtxtPlayerClothes				: register(t23);
Texture2D gtxtPlayerFace				: register(t24);
Texture2D gtxtPlayerHair				: register(t25);
Texture2D gtxtPlayerSword				: register(t26);
Texture2D gtxtPlayerBow					: register(t27);
Texture2D gtxtPlayerStaff				: register(t28);

Texture2D gtxtTree						: register(t29);
Texture2D gtxtNoLeafTrees				: register(t30);
Texture2D gtxtLeaves					: register(t31);
Texture2D gtxtMoss_Rock					: register(t32);

Texture2D gtxtPuzzleBoard				: register(t33);
Texture2D gtxtHelpText					: register(t34);
Texture2D gtxtDry_Tree					: register(t35);
Texture2D gtxtStump						: register(t36);
Texture2D gtxtDead_Tree					: register(t37);
Texture2D gtxtDesert_Rock				: register(t38);
Texture2D gtxtWater						: register(t39);
Texture2D gtxtRain						: register(t40);
Texture2D gtxtFogParticle				: register(t41);

Texture2D gtxtBossD						: register(t42);
Texture2D gtxtBossC						: register(t43);
Texture2D gtxtBossE						: register(t44);
Texture2D gtxtBossN						: register(t45);
Texture2D gtxtMeleeSkeleton_01_D		: register(t46);
Texture2D gtxtMeleeSkeleton_02			: register(t47);
Texture2D gtxtMeleeSkeleton_02_Equip	: register(t48);
Texture2D gtxtMeleeSkeleton_02_EquipAll	: register(t49);
Texture2D gtxtMummy						: register(t50);
Texture2D gtxtMummy_M					: register(t51);
Texture2D gtxtGreenTree					: register(t52);
Texture2D gtxtEffect_1					: register(t53);
Texture2D gtxtEffect_2					: register(t54);
Texture2D gtxtEffect_3					: register(t55);
Texture2D gtxtEffect_4					: register(t56);
Texture2D gtxtFireBall					: register(t57);
Texture2D KingDiffuse					: register(t58);
Texture2D KnightDiffuse					: register(t59);
Texture2D PawnDiffuse					: register(t60);
Texture2D RookDiffuse					: register(t61);
Texture2D ChessTile						: register(t62);
Texture2D Laser							: register(t63);
Texture2D gtxtMap						: register(t64);
Texture2D gtxtMirror					: register(t65);
Texture2D gtxtShadowMap					: register(t66);
Texture2D gtxtFont						: register(t67);

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
    float3 positionW : POSITION;
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
	newTime = fmod(newTime, lifeTime);
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
	//outRes.color.a = intensity;
	outRes.time = input.time;
	return outRes;
}

VS_TEX_PARTICLE_OUT VSFogParticle(VS_TEX_PARTICLE_IN input)
{
    VS_TEX_PARTICLE_OUT outRes;

    float emitTime = input.time.x;
    float lifeTime = input.time.y;

    float newTime = (gfTime - emitTime);
    newTime = fmod(newTime, lifeTime);

    float3 position;
    if (newTime > 0.0f)
    {
        float t = newTime;
        float tt = newTime * newTime;

        matrix copyMat = gmtxWorld;
        float3 newAcc = float3(0, -0.0f, 0.0f);
        float toDegree = degrees(2 * 3.14 * input.randomValues.x);
		  
        //float3 directionVec = float3(copyMat._21, copyMat._22, copyMat._23);
        float speedLength = length(input.speed);
        float3 speed = speedLength;

        float3 objPos = float3(copyMat._41, copyMat._42, copyMat._43);
        position = input.position + objPos;

        position = position + t * input.speed + tt * newAcc * 0.5f;

        copyMat._22 = 1.0f;
        copyMat._21 = copyMat._23 = copyMat._24 = 0.0f;
        copyMat._41 = position.x;
        copyMat._42 = position.y;
        copyMat._43 = position.z;
		
        //outRes.positionW = position;

        outRes.position = mul(mul(mul(float4(input.position, 1.0f), copyMat), gmtxView), gmtxProjection);
    }
    else
    {
        outRes.position = 0.0f;
    }
    
	
    //outRes.color = input.color;
	//outRes.color.a = intensity;
	 
    outRes.time = (length(position - gvCameraPosition) / 7500.0f) - 0.5f;
    outRes.uv = input.uv;
    outRes.index = input.index;
     
    return outRes;
}

VS_TEX_PARTICLE_OUT VSRainParticle(VS_TEX_PARTICLE_IN input)
{
    VS_TEX_PARTICLE_OUT outRes;

    float emitTime = input.time.x;
    float lifeTime = input.time.y;

    float newTime = (gfTime - emitTime);
    newTime = fmod(newTime, lifeTime);
  
    if (newTime > 0.0f)
    {
        float t = newTime;
        float tt = newTime * newTime;

        matrix copyMat = gmtxWorld;
        float3 newAcc = float3(0, -0.0f, 0.0f);
        float toDegree = degrees(2 * 3.14 * input.randomValues.x);
       
        //float3 directionVec = float3(copyMat._21, copyMat._22, copyMat._23);
        float speedLength = length(input.speed);
        float3 speed = speedLength;

        float3 objPos = float3(copyMat._41, copyMat._42, copyMat._43);
        float3 position = input.position + objPos;
         
        position = position + t * input.speed + tt * newAcc * 0.5f;

        copyMat._22 = 1.0f;
        copyMat._21 = copyMat._23 = copyMat._24 = 0.0f;
        copyMat._41 = position.x;
        copyMat._42 = position.y;
        copyMat._43 = position.z;

        outRes.position = mul(mul(mul(float4(input.position, 1.0f), copyMat), gmtxView), gmtxProjection);
    }
    else
        {
            outRes.position = 0.0f;
        }
    
    
    outRes.time = input.time;
    outRes.uv = input.uv;
    outRes.index = input.index;
	
    return outRes;
}

VS_TEX_PARTICLE_OUT VSTexParticle(VS_TEX_PARTICLE_IN input)
{
    VS_TEX_PARTICLE_OUT outRes;

    float emitTime = input.time.x;
    float lifeTime = input.time.y;

    float paraTime = gmtxWorld._31;
	
    if (paraTime > 0.0f)
    {
        float newTime = (paraTime - emitTime);
        newTime = fmod(newTime, lifeTime);
		//if (newTime > lifeTime) {
		//	newTime = -1.0f;
		//}
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
        else
        {
            outRes.position = 0.0f;
        }
    }
    outRes.time = input.time;
    outRes.uv = input.uv;
    outRes.index = input.index;
    return outRes;
}

VS_TEX_PARTICLE_OUT VSMummyLaserParticle(VS_TEX_PARTICLE_IN input)
{
    VS_TEX_PARTICLE_OUT outRes;

    float emitTime = input.time.x;
    float lifeTime = input.time.y;

    float newTime = (gfTime - emitTime);
    newTime = fmod(newTime, lifeTime);
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
    else
    {
        outRes.position = 0.0f;
    }
    
    float intensity = 1 - (newTime / lifeTime);
    
    outRes.time = input.time;
    outRes.uv = input.uv;
    outRes.index = input.index;
    return outRes;
    
 //   float intensity = 1 - (newTime / lifeTime);
 //   outRes.color = input.color;
	////outRes.color.a = intensity;
 //   outRes.time = input.time;
 //   return outRes;
}


float4 PSParticle(VS_PARTICLE_OUT input) : SV_TARGET
{
	float4 cColor = input.color;
	
    //cColor.a = (length(input.positionW - gvCameraPosition) / 7500.0f)-0.5f;
    //cColor.a = 1 - lerp(cColor.a, FogColor.a, 1 - fogAmount);
	//cColor = 1.0f;
	
	return cColor;
}

float4 PSFogParticle(VS_PARTICLE_OUT input) : SV_TARGET
{
    float4 cColor = input.color;
	
    cColor.a = (length(input.positionW - gvCameraPosition) / 7500.0f) - 0.5f;
    //cColor.a = 1 - lerp(cColor.a, FogColor.a, 1 - fogAmount);
	
	//cColor = 1.0f;
    return cColor;
}

float4 PSTexParticle(VS_TEX_PARTICLE_OUT input) : SV_TARGET
{
	float4 cColor;
   
    cColor = gtxtBox.Sample(gssWrap, input.uv);
    
    if (gnTexturesMask & 0x01)
    {
        cColor = gtxtRain.Sample(gssWrap, input.uv);
    }   
    if (gnTexturesMask & 0x02)
    {
        cColor = gtxtFogParticle.Sample(gssWrap, input.uv); 
        //cColor.g += 0.2f;
        cColor.a += input.time;
    }
     
	//cColor = gtxtBox.Sample(gssWrap, input.uv);
	//cColor = 0.1f;
	return cColor;
}

