cbuffer CBuf
{
	//Camera 기준 버텍스의 좌표
	matrix modelView;
	//사영까지 완료한 즉, NDC공간에 위치한 좌표
	matrix modelViewProj;
};

struct VSOut
{
	//shader 계산에 사용될 포지션
	float3 viewPos : Position;
	float3 normal : Normal;
	float3 tan : Tangent;
	float3 bitan : BiTangent; 
	float2 tc : Texcoord;
	//실제 적용될 픽셀의화면상 위치
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal, float3 tan : Tangent ,float3 bitan : BiTangent,float2 tc : Texcoord)
{
	VSOut vso;
	vso.viewPos = (float3) mul(float4(pos, 1.0f), modelView);
	vso.normal = mul(n, (float3x3) modelView);
	vso.tan = mul(tan, (float3x3)modelView);
	vso.bitan = mul(bitan, (float3x3)modelView);
	vso.tc = tc;
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
	return vso;
}