#include "pch.h"
#include "CameraComponent.h"
#include "Core/Factory.h"
#include "Framework/Actor.h"
#include "Engine.h"

namespace nu
{
	FACTORY_REGISTER(CameraComponent)

	void CameraComponent::Update(float delta)
	{
		m_view = glm::lookAtRH(m_owner->GetTransform().position, m_owner->GetTransform().position + glm::vec3(0, -1, 0), glm::vec3{0, 1, 0});
		float aspect = Engine::Instance().GetRenderer().GetWidth() / (float)Engine::Instance().GetRenderer().GetHeight();
		m_projection = glm::perspectiveRH(glm::radians(fov), aspect, nearPlane, farPlane);
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
}