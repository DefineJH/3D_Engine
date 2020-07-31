#include "TransformCbuf.h"
TransformCbuf::TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot)
	:
	parent(parent)
{
	if (!vcbuf)
	{
		vcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx,slot);
	}
}

void TransformCbuf::Bind(Graphics& gfx) noexcept
{
	// bind는 매프레임 불려진다.
	// vcbuf즉, 오브젝트의 버텍스 버퍼를 들고있다가 drawable에서 월드좌표행렬을 얻어와 
	// 버텍스 쉐이더 내에서 버텍스버퍼의 데이터 ( 로컬좌표 ) 를 월드좌표로 바꾸고 월드좌표서 카메라 좌표로 변경하고 사영행렬을 얻어와 렌더링
	const auto model = parent.GetTransformXM();
	const Transforms tf =
	{
		DirectX::XMMatrixTranspose(model),
		DirectX::XMMatrixTranspose(
			model *
			gfx.GetCamera() *
			gfx.GetProjection()
		)
	};
	vcbuf->Update(gfx, tf);
	vcbuf->Bind(gfx);
}

std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::vcbuf;