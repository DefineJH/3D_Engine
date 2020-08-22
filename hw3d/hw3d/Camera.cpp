#include "Camera.h"
#include "JHMath.h"
#include "ImGUI/imgui.h"

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	namespace dx = DirectX;
	//������ ���� ����
	const dx::XMVECTOR forwardVec = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//���� ī�޶� ����Ű�� ���� ����
	const auto lookVector = dx::XMVector3Transform(forwardVec,
		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f)
	);

	const auto camPos = XMLoadFloat3(&pos);
	const auto camTar = dx::XMVectorAdd(camPos, lookVector);
	return dx::XMMatrixLookAtLH(camPos, camTar, dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera Controller"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f, "%.1f");
		ImGui::Text("Orientation");
		ImGui::SliderFloat("Pitch", &pitch, 0.995f * -90.0f, 0.995f * 90.0f);
		ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset()
{
	pos = { 0.0f, 7.5f,-18.0f };
	pitch = 0.0f;
	yaw = 0.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	//z�� �߽�ȸ���� ���ۺ��� ��������
	yaw = wrap_angle(yaw + dx * rotSpeed);
	//���� �ٶ󺸴� ȸ���� Ŭ������ �ɾ� ����ó�� ���� �Ŵ޷� �Ųٷ� ���� ���ϰԲ� �Ѵ�
	pitch = std::clamp(pitch + dy * rotSpeed, 0.995f * -PI / 2.0f , 0.995f * PI / 2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
	DirectX::XMStoreFloat3(&translation, 
		DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&translation),
									DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f) * DirectX::XMMatrixScaling(travelSpeed, travelSpeed, travelSpeed)
									)
	);
	pos = {
		pos.x + translation.x,
		pos.y + translation.y,
		pos.z + translation.z,
	};
}
