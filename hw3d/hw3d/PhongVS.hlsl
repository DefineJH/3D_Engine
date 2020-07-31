cbuffer CBuf
{
	matrix model;
	matrix modelViewProj;
};

struct VSOut
{
	//shader 계산에 사용될 포지션
	float3 worldPos : Position;
	float3 normal : Normal;
	//실제 적용될 픽셀의화면상 위치
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal)
{
	VSOut vso;
	vso.worldPos = (float3)mul(float4(pos, 1.0f), model);
	vso.normal = mul(n, (float3x3)model);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	return vso;
}