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
	// vcbuf��, ������Ʈ�� ���ؽ� ��� ���۸� ����ִٰ� �θ�(������)drawable���� ������ǥ����� ���� 
	// ���ؽ� ���̴� ������ ���ؽ� ��� ������ ������ ( �� �����̽� ) �� ������ǥ�� �ٲٰ� �翵����� ���� ������
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

