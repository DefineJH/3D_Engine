cbuffer CBuf
{
	//Camera ���� ���ؽ��� ��ǥ
	matrix modelView;
	//�翵���� �Ϸ��� ��, NDC������ ��ġ�� ��ǥ
	matrix modelViewProj;
};

struct VSOut
{
	//shader ��꿡 ���� ������
	float3 viewPos : Position;
	float3 normal : Normal;
	float3 tan : Tangent;
	float3 bitan : BiTangent; 
	float2 tc : Texcoord;
	//���� ����� �ȼ���ȭ��� ��ġ
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