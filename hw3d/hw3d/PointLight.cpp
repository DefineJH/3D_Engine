#include "PointLight.h"
#include "ImGUI/imgui.h"
PointLight::PointLight(Graphics& gfx, float radius /*= 0.5f*/)
	:
	mesh(gfx,radius),
	cbuf(gfx,0u)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Point Light Controller"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &cbData.pos.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &cbData.pos.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &cbData.pos.z, -60.0f, 60.0f, "%.1f");
		ImGui::Text("Lighting//Inten");
		ImGui::SliderFloat("Intensity", &cbData.DiffuseInten, 0.0f, 1.5f, "%.1f");
		ImGui::ColorEdit3("DiffuseColor", &cbData.DiffuseColor.x);
		ImGui::ColorEdit3("AmbientColor", &cbData.AmbientLight.x);
		ImGui::SliderFloat("AttenConst", &cbData.AttenuationCon, 0.0f, 1.5f, "%.1f");
		ImGui::SliderFloat("AttenLinear", &cbData.AttenuationLin, 0.0f, 1.0f, "%.3f");
		ImGui::SliderFloat("AttenQuadertic", &cbData.AttenuationQuad, 0.0f, 1.0f, "%.4f");

		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	cbData =
	{
		{0.0f,0.0f,0.0f}, 
		{0.05f,0.05f,0.05f},
		{1.0f,1.0f,1.0f},
		1.0f,
		1.0f,
		0.045f,
		0.0075f,
	};
}

void PointLight::Draw(Graphics& gfx) const noexcept(!IS_DEBUG)
{
	mesh.SetPos(cbData.pos);
	mesh.Draw(gfx);
}

void PointLight::Bind(Graphics& gfx, DirectX::FXMMATRIX view) const noexcept
{
	auto dataCopy = cbData;
	const auto pos = DirectX::XMLoadFloat3(&cbData.pos);
	DirectX::XMStoreFloat3(&dataCopy.pos, DirectX::XMVector3Transform(pos, view));
	cbuf.Update(gfx, dataCopy);
	cbuf.Bind(gfx);
}

