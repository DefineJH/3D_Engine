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
	float3 CamBasepos : Position;
	float3 normal : Normal;
    float2 tc : Texcoord;
	//���� ����� �ȼ���ȭ��� ��ġ
	float4 pos : SV_Position;
};

VSOut main(float3 pos : Position, float3 n : Normal , float2 tc : Texcoord)
{
	VSOut vso;
    vso.CamBasepos = (float3) mul(float4(pos, 1.0f), modelView);
    vso.normal = mul(n, (float3x3) modelView);
	vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.tc = tc;
	return vso;
}