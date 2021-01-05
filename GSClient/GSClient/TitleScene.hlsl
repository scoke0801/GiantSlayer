cbuffer cbTestData : register(b1)
{
	bool	gMouseClick : packoffset(c0); 
};

// 정점 셰이더를 정의한다.
float4 VSTest(uint nVertexID : SV_VertexID) :SV_POSITION
{
	float4 output;

// 프리미티브(삼각형)를 구성하는 
// 정점의 인덱스(SV_VertexID)에 따라 정점을 반환한다.
// 정점의 위치 좌표는 변환이 된 좌표(SV_POSITION)이다.
// 즉, 투영좌표계의 좌표이다.
	if (nVertexID == 0)		 output = float4(-0.5f, +0.5f, 0.0f, 1.0f);
	else if (nVertexID == 1) output = float4(+0.5f, +0.5f, 0.0f, 1.0f);
	else if (nVertexID == 2) output = float4(+0.5f, -0.5f, 0.0f, 1.0f);
	
	else if (nVertexID == 3) output = float4(-0.5f, +0.5f, 0.0f, 1.0f);
	else if (nVertexID == 4) output = float4(+0.5f, -0.5f, 0.0f, 1.0f);
	else if (nVertexID == 5) output = float4(-0.5f, -0.5f, 0.0f, 1.0f);

	return(output);
}

// 픽셀 셰이더를 정의한다.
float4 PSTest(float4 input : SV_POSITION) : SV_TARGET
{
	float4 color = float4(0.0f, 0.0f, 0.5f, 1.0f);
	if (gMouseClick)color = float4(0.5f, 0.0f, 0.0f, 1.0f);

	return color;
}