#pragma once
#include "Graphics.h"

// ���������ο� ���� ��ü���� ��ӹ��� �������̽�
class Bindable
{
public:
	virtual void Bind(Graphics& gfx) noexcept = 0;
	virtual ~Bindable() = default;
protected:
	//Graphics�� friend����� �Ǿ������Ƿ� �� �Լ����� ���Ͽ� �ڽ� Ŭ�������� �ʿ��� ��ü�� �������� �� ����
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(Graphics& gfx);
};