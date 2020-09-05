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
	UpdateBindImpl(gfx, GetTransforms(gfx));
}

void TransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tr) noexcept
{
	vcbuf->Update(gfx, tr);
	vcbuf->Bind(gfx);
}

TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& gfx)
{
	// vcbuf즉, 오브젝트의 버텍스 상수 버퍼를 들고있다가 부모(소유자)drawable에서 월드좌표행렬을 얻어와 
	// 버텍스 쉐이더 내에서 버텍스 상수 버퍼의 데이터 ( 뷰 스페이스 ) 를 월드좌표로 바꾸고 사영행렬을 얻어와 렌더링
	const auto model = parent.GetTransformXM() * gfx.GetCamera();
	const Transforms tr =
	{
		DirectX::XMMatrixTranspose(model),
		DirectX::XMMatrixTranspose(
			model *
			gfx.GetProjection()
		)
	};
	return tr;
}

std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::vcbuf;

