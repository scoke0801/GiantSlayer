cbuffer cbTestData : register(b0)
{
	bool	gMouseClick : packoffset(c0); 
};
 
struct VS_OUT
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
};

// ���� ���̴��� �����Ѵ�.
float4 VSTest(uint nVertexID : SV_VertexID) :SV_POSITION
{
	float4 output;

// ������Ƽ��(�ﰢ��)�� �����ϴ� 
// ������ �ε���(SV_VertexID)�� ���� ������ ��ȯ�Ѵ�.
// ������ ��ġ ��ǥ�� ��ȯ�� �� ��ǥ(SV_POSITION)�̴�.
// ��, ������ǥ���� ��ǥ�̴�.
	if (nVertexID == 0)		  output = float4(-0.75f, -0.325f, 0.0f, 1.0f);
	else if (nVertexID == 1)  output = float4(-0.25f, -0.325f, 0.0f, 1.0f);
	else if (nVertexID == 2)  output = float4(-0.25f, -0.525f, 0.0f, 1.0f);
							  						   
	else if (nVertexID == 3)  output = float4(-0.75f, -0.325f, 0.0f, 1.0f);
	else if (nVertexID == 4)  output = float4(-0.25f, -0.525f, 0.0f, 1.0f);
	else if (nVertexID == 5)  output = float4(-0.75f, -0.525f, 0.0f, 1.0f);

	else if (nVertexID == 6)  output = float4( 0.25f, -0.325f, 0.0f, 1.0f);
	else if (nVertexID == 7)  output = float4( 0.75f, -0.325f,0.0f, 1.0f);
	else if (nVertexID == 8)  output = float4( 0.75f, -0.525f,0.0f, 1.0f);

	else if (nVertexID == 9)  output = float4(0.25f, -0.325f, 0.0f, 1.0f);
	else if (nVertexID == 10) output = float4(0.75f, -0.525f, 0.0f, 1.0f);
	else if (nVertexID == 11) output = float4(0.25f, -0.525f, 0.0f, 1.0f);

	return(output);
}

// �ȼ� ���̴��� �����Ѵ�.
float4 PSTest(float4 input : SV_POSITION) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.5f, 1.0f);
	if (gMouseClick)color = float4(0.5f, 0.0f, 0.0f, 1.0f);

	return color;
}

float4 PSTextured(VS_OUT input) : SV_TARGET
{
	float4 cColor;// = gtxtTexture.Sample(gSamplerState, input.uv);

	return(cColor);
}
