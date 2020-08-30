#pragma once
#include "Bindable.h"
#include "GraphicsThrowMacro.h"
#include "Vertex.h"

 class VertexBuffer :
	 public Bindable
 {
 public:
	 //버텍스는 우리가 직접 정의하는 클래스 혹은 구조체이기 때문에 템플릿화하여 버텍스 정보가 바뀌었을때도 정상작동하게 한다
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
	 // tag를 부여함으로 버텍스 버퍼를 구분
	 std::string tag;
	 UINT stride;
	 Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};


