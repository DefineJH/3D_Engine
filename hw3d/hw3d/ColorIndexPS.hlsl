cbuffer CBuf
{
	float4 face_colors[8];
};
//rasterizer 단계에서 이미 픽셀의 포지션은 정해져있다.
//pixel shader는 이미 정해진 픽셀의 색을 리턴하기 때문에 인풋값이 필요없는 함수이다.(지금은)
float4 main(uint tid : SV_PrimitiveID) : SV_Target
{
	return face_colors[(tid / 2) % 8];
}