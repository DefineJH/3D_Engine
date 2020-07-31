cbuffer LightCBuf
{
	float3 LightPos;
};

static const float3 MaterialColor = { 0.7f, 0.7f, 0.9f };
static const float3 AmbientLight = { 0.15f, 0.15f, 0.15f };
static const float3 DiffuseColor = { 1.0f, 1.0f, 1.0f };
static const float DiffuseInten = 1.0f;
static const float AttenuationCon = 1.0f;
static const float AttenuationLin = 1.0f;
static const float AttenuationQuad = 1.0f;

float4 main(float3 worldPos : Position , float3 n : Normal ) : SV_Target
{
	return float4(1.0f,0.7f,1.0f,1.0f);
}
