#pragma once
#include "Graphics.h"
#include <memory>

// 파이프라인에 묶을 객체들이 상속받을 인터페이스
class Bindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual std::string GetUID() const noexcept
	{
		assert(false);
		return "";
	}
	virtual ~Bindable() = default;
protected:
	//Graphics에 friend등록이 되어있으므로 이 함수들을 통하여 자식 클래스들은 필요한 객체를 지원받을 수 있음
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(Graphics& gfx);
};
