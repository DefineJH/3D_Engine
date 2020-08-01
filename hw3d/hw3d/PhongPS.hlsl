cbuffer LightCBuf
{
	float3 LightPos; // 12 + 4(ÆÐµù)
    float3 AmbientLight; // 16
    float3 DiffuseColor; // 16
    float DiffuseInten; // 4
    float AttenuationCon; // 4
    float AttenuationLin; // 4
    float AttenuationQuad; //4
};

cbuffer ObjectMaterial
{
    float3 MaterialColor;
    float specularInten;
    float specularPower;
};



float4 main(float3 worldPos : Position , float3 n : Normal ) : SV_Target
{
	float3 posToLight = LightPos - worldPos;
    float distToL = length(posToLight);
	float3 posToLightNorm = normalize(posToLight);
    float lightAngle = max(0.0f, dot(n, posToLightNorm));
    float atten = 1.0f / (AttenuationCon + AttenuationLin * distToL + AttenuationQuad * pow(distToL, 2));
    const float3 diffuse = DiffuseColor * DiffuseInten * atten * max(0.0f, dot(posToLightNorm, n));
    const float3 w = n * dot(posToLight, n);
    const float3 r = w * 2.0f - posToLight;
    const float3 specular =  atten * (DiffuseColor * DiffuseInten) * specularInten * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);
    return float4(saturate((diffuse + AmbientLight + specular) * MaterialColor), 1.0f);
}
