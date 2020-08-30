#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacro.h"
#include "Vertex.h"

 class VertexBuffer :
	 public Bindable
 {
 public:
	 //���ؽ��� �츮�� ���� �����ϴ� Ŭ���� Ȥ�� ����ü�̱� ������ ���ø�ȭ�Ͽ� ���ؽ� ������ �ٲ�������� �����۵��ϰ� �Ѵ�
	 VertexBuffer(Graphics& gfx, const DynamicVertex::VertexBuffer& vbuf);
	 VertexBuffer(Graphics& gfx, const std::string& tag, const DynamicVertex::VertexBuffer vbuf);
	 void Bind(Graphics& gfx) noexcept override;
	 static std::shared_ptr<Bindable> Resolve(Graphics& gfx, const std::string& tag, const DynamicVertex::VertexBuffer& vbuf);
	 template<typename...Ignore>
	 static std::string GenerateUID(const std::string& tag, Ignore&&...ignore)
	 {
		 return GenerateUID_(tag);
	 }
	 std::string GetUID() const noexcept override;
 private:
	 static std::string GenerateUID_(const std::string& tag);
 protected:
	 DynamicVertex::VertexBuffer vbuf;
	 // tag�� �ο������� ���ؽ� ���۸� ����
	 std::string tag;
	 UINT stride;
	 Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};


