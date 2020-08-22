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
	//기존의 전방 벡터
	const dx::XMVECTOR forwardVec = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//현재 카메라가 가리키는 전방 벡터
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
	//z축 중심회전은 빙글빙글 도는형태
	yaw = wrap_angle(yaw + dx * rotSpeed);
	//위를 바라보는 회전은 클램프를 걸어 박쥐처럼 위에 매달려 거꾸로 보지 못하게끔 한다
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
