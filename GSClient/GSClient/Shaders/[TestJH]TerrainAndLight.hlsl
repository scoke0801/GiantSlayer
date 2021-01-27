// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#define MaxLights 16

struct Light
{ 
    float3 Strength;
    float FalloffStart; // point/spot light only
    float3 Direction;   // directional/spot light only
    float FalloffEnd;   // point/spot light only
    float3 Position;    // point light only
    float SpotPower;    // spot light only
};

struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess;
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // Linear falloff.
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
    float cosIncidentAngle = saturate(dot(normal, lightVec));

    float f0 = 1.0f - cosIncidentAngle;
    float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

    return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat)
{
    const float m = mat.Shininess * 256.0f;
    float3 halfVec = normalize(toEye + lightVec);

    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, normal), 0.0f), m) / 8.0f;
    float3 fresnelFactor = SchlickFresnel(mat.FresnelR0, halfVec, lightVec);

    float3 specAlbedo = fresnelFactor * roughnessFactor;

    // Our spec formula goes outside [0,1] range, but we are 
    // doing LDR rendering.  So scale it down a bit.
    specAlbedo = specAlbedo / (specAlbedo + 1.0f);

    return (mat.DiffuseAlbedo.rgb + specAlbedo) * lightStrength;
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for directional lights.
//---------------------------------------------------------------------------------------
float3 ComputeDirectionalLight(Light L, Material mat, float3 normal, float3 toEye)
{
    // The light vector aims opposite the direction the light rays travel.
    float3 lightVec = -L.Direction;
	return lightVec;

    // Scale light down by Lambert's cosine law.
	float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for point lights.
//---------------------------------------------------------------------------------------
float3 ComputePointLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // The vector from the surface to the light.
    float3 lightVec = L.Position - pos;

    // The distance from surface to light.
    float d = length(lightVec);

    // Range test.
    if (d > L.FalloffEnd)
        return 0.0f; 

    // Normalize the light vector.
    lightVec /= d;

    // Scale light down by Lambert's cosine law.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // Attenuate light by distance.
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

//---------------------------------------------------------------------------------------
// Evaluates the lighting equation for spot lights.
//---------------------------------------------------------------------------------------
float3 ComputeSpotLight(Light L, Material mat, float3 pos, float3 normal, float3 toEye)
{
    // The vector from the surface to the light.
    float3 lightVec = L.Position - pos;

    // The distance from surface to light.
    float d = length(lightVec);

    // Range test.
    if (d > L.FalloffEnd)
        return 0.0f;

    // Normalize the light vector.
    lightVec /= d;

    // Scale light down by Lambert's cosine law.
    float ndotl = max(dot(lightVec, normal), 0.0f);
    float3 lightStrength = L.Strength * ndotl;

    // Attenuate light by distance.
    float att = CalcAttenuation(d, L.FalloffStart, L.FalloffEnd);
    lightStrength *= att;

    // Scale by spotlight
    float spotFactor = pow(max(dot(-lightVec, L.Direction), 0.0f), L.SpotPower);
    lightStrength *= spotFactor;

    return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
}

float4 ComputeLighting(Light gLights[MaxLights], Material mat,
    float3 pos, float3 normal, float3 toEye,
    float3 shadowFactor)
{
    float3 result = 0.0f;

    int i = 0;

#if (NUM_DIR_LIGHTS > 0)
    for (i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor[i] * ComputeDirectionalLight(gLights[i], mat, normal, toEye);
    }
#endif

#if (NUM_POINT_LIGHTS > 0)
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(gLights[i], mat, pos, normal, toEye);
    }
#endif

#if (NUM_SPOT_LIGHTS > 0)
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(gLights[i], mat, pos, normal, toEye);
    }
#endif 

    return float4(result, 0.0f);
}
 
//게임 객체의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbGameObjectInfo : register(b0)
{
	matrix	gmtxWorld : packoffset(c0);
	uint	gnTexturesMask : packoffset(c4);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다. 
cbuffer cbCameraInfo : register(b1)
{
	matrix	gmtxView : packoffset(c0);
	matrix	gmtxProjection : packoffset(c4);
	float3	gvCameraPosition : packoffset(c8);
};

cbuffer cbMaterial : register(b2)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float  gRoughness;
	matrix gMatTransform;
};
cbuffer cbLight : register(b3)
{
    float4 gAmbientLight;

    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    Light gLights[MaxLights];
}

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
 
struct VS_TEXTURE_IN
{
	float3 position : POSITION;
	float2 uv		: TEXCORD;
    float3 Normal   : NORMAL;
};
struct VS_TEXTURE_OUT
{
	float4 position : SV_POSITION;
	float2 uv	    : TEXCORD;
};
struct VS_LIGHT_OUT
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float2 uv	   : TEXCORD;
    float3 Normal  : NORMAL;
};

VS_TEXTURE_OUT VSTextured(VS_TEXTURE_IN input)
{
	VS_TEXTURE_OUT outRes;
	outRes.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
	outRes.uv = input.uv;
	return outRes;
}
VS_LIGHT_OUT VSLighted(VS_TEXTURE_IN input)
{
 /*   VS_LIGHT_OUT outRes;
    outRes.PosH = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
    outRes.uv = input.uv;
    return outRes;*/

    VS_LIGHT_OUT vout = (VS_LIGHT_OUT)0.0f;

    // Transform to world space.
    float4 posW = mul(float4(input.position, 1.0f), gmtxWorld);
    vout.PosW = posW.xyz;

    // Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix.
    vout.Normal = mul(input.Normal, (float3x3)gmtxWorld);

    // Transform to homogeneous clip space.
    vout.PosH = mul(mul(posW, gmtxView), gmtxProjection);

    vout.uv = input.uv;
    return vout;
}

float4 PSTextured(VS_TEXTURE_OUT input) : SV_TARGET
{
	float4 cColor;

	return gtxtBox.Sample(gssWrap, input.uv);
}
 
float4 PSTexIndex(VS_TEXTURE_OUT input) : SV_TARGET
{
	float4 cColor;

	if (gnTexturesMask & 0x01)
	{
		cColor = gtxtTerrain.Sample(gssWrap, input.uv);
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

	return cColor;
	//return gtxtBox.Sample(gssWrap, input.uv);
}

float4 PSLighted(VS_LIGHT_OUT pin) : SV_Target
{ 
    float4 diffuseAlbedo = gtxtBox.Sample(gssWrap, pin.uv) * gDiffuseAlbedo;

    // Interpolating normal can unnormalize it, so renormalize it.
    pin.Normal = normalize(pin.Normal);

    float3 EyePosW = float3(0.0f, 300.0f, -150.0f);
    float4 EyePos = mul(mul(mul(float4(EyePosW, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
    EyePosW = EyePos.xyz;
    // Vector from point being lit to eye. 
    float3 toEyeW = normalize(EyePosW - pin.PosW);

    //float4 diffuseAlbedo = gtxtBox.Sample(gssWrap, pin.uv);
    // Indirect lighting.
    float4 ambient = gAmbientLight * diffuseAlbedo;
    //
    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, pin.PosW,
        pin.Normal, toEyeW, shadowFactor);
    
    float4 litColor = ambient * directLight;// *diffuseAlbedo;
    //
    //// Common convention to take alpha from diffuse material.
    litColor.a = diffuseAlbedo.a;
    //
    return litColor;
   // return float4(1.0f, 1.0f, 1.0f, 0.0f);
}