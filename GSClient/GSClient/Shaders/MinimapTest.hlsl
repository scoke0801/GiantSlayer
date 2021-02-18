cbuffer cbTestData : register(b0)
{
	bool	gMouseClick : packoffset(c0);
};

struct VS_OUT
{
	float4	position : SV_POSITION;
	float2	uv		 : TEXCOORD;
	uint	index	 : TEXTURE;
};

Texture2D gtxtMultipleButton : register(t0);
Texture2D gtxtSimpleButton   : register(t1);
Texture2D gtxtTitle		     : register(t2);
Texture2D gtxtMiniMap		 : register(t3);

SamplerState gssWrap : register(s0);

VS_OUT VSTextured(uint nVertexID : SV_VertexID)
{
	VS_OUT outResult;

	if		(nVertexID == 0)  { outResult.position = float4(-0.75f, -0.325f, 0.2f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 1; }
	else if (nVertexID == 1)  { outResult.position = float4(-0.25f, -0.325f, 0.2f, 1.0f); outResult.uv = float2(1, 0); outResult.index = 1; }
	else if (nVertexID == 2)  { outResult.position = float4(-0.25f, -0.525f, 0.2f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 1; }
							  												   
	else if (nVertexID == 3)  { outResult.position = float4(-0.75f, -0.325f, 0.2f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 1; }
	else if (nVertexID == 4)  { outResult.position = float4(-0.25f, -0.525f, 0.2f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 1; }
	else if (nVertexID == 5)  { outResult.position = float4(-0.75f, -0.525f, 0.2f, 1.0f); outResult.uv = float2(0, 1); outResult.index = 1; }
							  												   
	else if (nVertexID == 6)  { outResult.position = float4( 0.25f, -0.325f, 0.2f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 2; }
	else if (nVertexID == 7)  { outResult.position = float4( 0.75f, -0.325f, 0.2f, 1.0f); outResult.uv = float2(1, 0); outResult.index = 2; }
	else if (nVertexID == 8)  { outResult.position = float4( 0.75f, -0.525f, 0.2f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 2; }
							  								 				   
	else if (nVertexID == 9)  { outResult.position = float4( 0.25f, -0.325f, 0.2f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 2; }
	else if (nVertexID == 10) { outResult.position = float4( 0.75f, -0.525f, 0.2f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 2; }
	else if (nVertexID == 11) { outResult.position = float4( 0.25f, -0.525f, 0.2f, 1.0f); outResult.uv = float2(0, 1); outResult.index = 2; }

	else if (nVertexID == 12) { outResult.position = float4(-1.0f,   1.0f,   0.2f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 3; }
	else if (nVertexID == 13) { outResult.position = float4( 1.0f,   1.0f,   0.2f, 1.0f); outResult.uv = float2(1, 0); outResult.index = 3; }
	else if (nVertexID == 14) { outResult.position = float4( 1.0f,  -1.0f,   0.2f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 3; }
																    	       
	else if (nVertexID == 15) { outResult.position = float4(-1.0f,   1.0f,   0.2f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 3; }
	else if (nVertexID == 16) { outResult.position = float4( 1.0f,  -1.0f,   0.2f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 3; }
	else if (nVertexID == 17) { outResult.position = float4(-1.0f,  -1.0f,   0.2f, 1.0f); outResult.uv = float2(0, 1); outResult.index = 3; }

	return(outResult);
}

float4 PSTextured(VS_OUT input) : SV_TARGET
{
	float4 cColor;
	if (input.index == 1)
	{
		cColor = gtxtMultipleButton.Sample(gssWrap, input.uv);
	}
	else if (input.index == 2)
	{
		cColor = gtxtSimpleButton.Sample(gssWrap, input.uv);
	}
	else
	{
		cColor = gtxtTitle.Sample(gssWrap, input.uv);
	}
	return(cColor);
}

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
	outRes.position = float4(input.position, 1.0f);  
	outRes.uv = input.uv;
	return outRes;
}

float4 PSMinimap(VS_MOUT input) : SV_TARGET
{	
	float4 cColor;
	//return cColor = gtxtTitle.Sample(gssWrap, input.uv);
	return cColor = gtxtMiniMap.Sample(gssWrap, input.uv);
	
	//return float4(1.0f, 1.0f, 0.0f, 0.0f);
}
