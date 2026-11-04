#include "pch.h"

#include <SDL3/SDL_scancode.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "CameraControllerComponent.h"
#include "Framework/Actor.h"
#include "Engine.h"
#include "Core/Factory.h"

namespace nu
{
	FACTORY_REGISTER(CameraControllerComponent)

	void CameraControllerComponent::Start()
	{
		m_input = Engine::Instance().GetInput();
	}
	void CameraControllerComponent::Update(float dt)
	{
		// get camera rotation
		if (m_input.GetButtonDown(Input::MouseButton::Right))
		{
			glm::vec2 axis = m_input.GetMouseDelta() * m_sensitivity;

			m_yaw -= axis.x;
			m_pitch -= axis.y;
			m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);
		}

		//m_owner->transform.SetRotation(glm::vec3(pitch, yaw, 0.0f));

		glm::quat q = glm::quat_cast(glm::eulerAngleYXZ(glm::radians(m_yaw), glm::radians(m_pitch), 0.0f));
		//m_owner->transform.SetRotationQuat(q);

		glm::vec3 direction{ 0 };
		if (m_input.GetKeyDown(SDL_SCANCODE_A)) direction -= m_owner->GetTransform().Right();
		if (m_input.GetKeyDown(SDL_SCANCODE_D)) direction += m_owner->GetTransform().Right();
		if (m_input.GetKeyDown(SDL_SCANCODE_E)) direction += m_owner->GetTransform().Up();
		if (m_input.GetKeyDown(SDL_SCANCODE_Q)) direction -= m_owner->GetTransform().Up();
		if (m_input.GetKeyDown(SDL_SCANCODE_W)) direction += m_owner->GetTransform().Forward();
		if (m_input.GetKeyDown(SDL_SCANCODE_S)) direction -= m_owner->GetTransform().Forward();;

		if (glm::length2(direction) > 0.0f) {
			direction = glm::normalize(direction);
		}
		m_owner->SetPosition(m_owner->GetTransform().position + direction * (m_speed * dt));

	}
	void CameraControllerComponent::Read(const json::value_t& value)
	{
		Component::Read(value);

		JSON_READ_NAME(value, "speed", m_speed);
	}
}