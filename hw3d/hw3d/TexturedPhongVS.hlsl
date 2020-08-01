cbuffer CBuf
{
    matrix modelView;
    matrix modelViewProj;
};

struct VSOut
{
    float3 worldPos : Position;
    float3 normal : Normal;
    float2 TexCoord : Texcoord;
    float4 pos : SV_Position;
};

VSOut main(float3 pos : Position , float3 n : Normal , float2 tc : Texcoord)
{
    VSOut vso;
    vso.worldPos = (float3)mul(float4(pos, 1.0f), modelView);
    vso.normal = (float3) mul(float4(n, 1.0f), modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.TexCoord = tc;
    return vso;
}