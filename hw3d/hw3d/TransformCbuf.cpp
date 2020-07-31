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
	// bind�� �������� �ҷ�����.
	// vcbuf��, ������Ʈ�� ���ؽ� ���۸� ����ִٰ� drawable���� ������ǥ����� ���� 
	// ���ؽ� ���̴� ������ ���ؽ������� ������ ( ������ǥ ) �� ������ǥ�� �ٲٰ� ������ǥ�� ī�޶� ��ǥ�� �����ϰ� �翵����� ���� ������
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