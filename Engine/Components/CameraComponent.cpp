#include "pch.h"
#include "CameraComponent.h"
#include "Core/Factory.h"
#include "Framework/Actor.h"
#include "Engine.h"

namespace nu
{
	FACTORY_REGISTER(CameraComponent)

	void CameraComponent::Start()
	{
		m_aspectRatio = Engine::Instance().GetRenderer().GetWidth() / (float)Engine::Instance().GetRenderer().GetHeight();
	}

	void CameraComponent::Update(float delta)
	{
		glm::vec3 forward = m_owner->GetTransform().Forward();

		m_view = glm::lookAtRH(m_owner->GetTransform().position, m_owner->GetTransform().position + forward, glm::vec3{0, 1, 0});
		m_projection = glm::perspectiveRH(glm::radians(fov), m_aspectRatio, nearPlane, farPlane);
	}

	void CameraComponent::Draw(const Renderer& renderer)
	{

	}

	void CameraComponent::Read(const json::value_t& value)
	{
		RendererComponent::Read(value);

		JSON_READ(value, fov);
		JSON_READ_NAME(value, "near_plane", nearPlane);
		JSON_READ_NAME(value, "far_plane", farPlane);
	}

	void CameraComponent::UpdateGui()
	{
		RendererComponent::UpdateGui();

		ImGui::SliderFloat("FOV", &fov, 0.1f, 180.0f);
		ImGui::DragFloat("Near Plane", &nearPlane, 0.1f);
		ImGui::DragFloat("Far Plane", &farPlane, 0.1f);
	}
}