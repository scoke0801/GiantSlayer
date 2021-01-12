cbuffer cbTestData : register(b0)
{
	bool	gMouseClick : packoffset(c0); 
};
 
struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD; 
	uint index : TEXTURE;
};

Texture2D gtxtMultipleButton : register(t0);
Texture2D gtxtSimpleButton   : register(t1);
Texture2D gtxtTitle		     : register(t2);

SamplerState gssWrap : register(s0);

// 정점 셰이더를 정의한다.
float4 VSTest(uint nVertexID : SV_VertexID) :SV_POSITION
{
	float4 output;

	if (nVertexID == 0)		  { output = float4(-0.75f, -0.325f, 0.0f, 1.0f); }
	else if (nVertexID == 1)  { output = float4(-0.25f, -0.325f, 0.0f, 1.0f); }
	else if (nVertexID == 2)  { output = float4(-0.25f, -0.525f, 0.0f, 1.0f); } 
																			  
	else if (nVertexID == 3)  { output = float4(-0.75f, -0.325f, 0.0f, 1.0f); }
	else if (nVertexID == 4)  { output = float4(-0.25f, -0.525f, 0.0f, 1.0f); }
	else if (nVertexID == 5)  { output = float4(-0.75f, -0.525f, 0.0f, 1.0f); }
																			  
	else if (nVertexID == 6)  { output = float4( 0.25f, -0.325f, 0.0f, 1.0f); }
	else if (nVertexID == 7)  { output = float4( 0.75f, -0.325f, 0.0f, 1.0f); }
	else if (nVertexID == 8)  { output = float4( 0.75f, -0.525f, 0.0f, 1.0f); } 
																			  
	else if (nVertexID == 9)  { output = float4( 0.25f, -0.325f, 0.0f, 1.0f); }
	else if (nVertexID == 10) { output = float4( 0.75f, -0.525f, 0.0f, 1.0f); }
	else if (nVertexID == 11) { output = float4( 0.25f, -0.525f, 0.0f, 1.0f); }

	else if (nVertexID == 12) { output = float4(-1.0f, -0.325f, 0.0f, 1.0f); }
	else if (nVertexID == 13) { output = float4( 1.0f, -0.325f, 0.0f, 1.0f); }
	else if (nVertexID == 14) { output = float4( 1.0f, -0.525f, 0.0f, 1.0f); }
							  					 
	else if (nVertexID == 15) { output = float4(-1.0f, -0.325f, 0.0f, 1.0f); }
	else if (nVertexID == 16) { output = float4( 1.0f, -0.525f, 0.0f, 1.0f); }
	else if (nVertexID == 17) { output = float4(-1.0f, -0.525f, 0.0f, 1.0f); }

	return(output);
}

// 픽셀 셰이더를 정의한다.
float4 PSTest(float4 input : SV_POSITION) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.5f, 1.0f);
	if (gMouseClick)color = float4(0.5f, 0.0f, 0.0f, 1.0f);

	return color;
}

VS_OUT VSTextured(uint nVertexID : SV_VertexID)
{
	VS_OUT outResult;
 
	if		(nVertexID == 0)  { outResult.position = float4(-0.75f, -0.325f, 0.0f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 1; }
	else if (nVertexID == 1)  { outResult.position = float4(-0.25f, -0.325f, 0.0f, 1.0f); outResult.uv = float2(1, 0); outResult.index = 1; }
	else if (nVertexID == 2)  { outResult.position = float4(-0.25f, -0.525f, 0.0f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 1; }
							  																						   					 
	else if (nVertexID == 3)  { outResult.position = float4(-0.75f, -0.325f, 0.0f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 1; }
	else if (nVertexID == 4)  { outResult.position = float4(-0.25f, -0.525f, 0.0f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 1; }
	else if (nVertexID == 5)  { outResult.position = float4(-0.75f, -0.525f, 0.0f, 1.0f); outResult.uv = float2(0, 1); outResult.index = 1; }
							  																						   					
	else if (nVertexID == 6)  { outResult.position = float4( 0.25f, -0.325f, 0.0f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 2;  }
	else if (nVertexID == 7)  { outResult.position = float4( 0.75f, -0.325f, 0.0f, 1.0f); outResult.uv = float2(1, 0); outResult.index = 2;  }
	else if (nVertexID == 8)  { outResult.position = float4( 0.75f, -0.525f, 0.0f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 2;  }
							  																						   					 
	else if (nVertexID == 9)  { outResult.position = float4( 0.25f, -0.325f, 0.0f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 2;  }
	else if (nVertexID == 10) { outResult.position = float4( 0.75f, -0.525f, 0.0f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 2;  }
	else if (nVertexID == 11) { outResult.position = float4( 0.25f, -0.525f, 0.0f, 1.0f); outResult.uv = float2(0, 1); outResult.index = 2;  }
	
	else if (nVertexID == 12) { outResult.position = float4(-1.0f, 1.0f, 0.0f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 3; }
	else if (nVertexID == 13) { outResult.position = float4( 1.0f, 1.0f, 0.0f, 1.0f); outResult.uv = float2(1, 0); outResult.index = 3; }
	else if (nVertexID == 14) { outResult.position = float4( 1.0f, -1.0f, 0.0f, 1.0f);outResult.uv = float2(1, 1); outResult.index = 3; }

	else if (nVertexID == 15) { outResult.position = float4(-1.0f, 1.0f, 0.0f, 1.0f); outResult.uv = float2(0, 0); outResult.index = 3; }
	else if (nVertexID == 16) { outResult.position = float4( 1.0f, -1.0, 0.0f, 1.0f); outResult.uv = float2(1, 1); outResult.index = 3; }
	else if (nVertexID == 17) { outResult.position = float4(-1.0f, -1.0, 0.0f, 1.0f); outResult.uv = float2(0, 1); outResult.index = 3; }

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
