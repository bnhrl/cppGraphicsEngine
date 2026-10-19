#pragma once
#include "Component.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

namespace nu
{
	class PhysicsComponent : public Component
	{
	public:
		virtual void ApplyForce(const glm::vec3& force) = 0;
		virtual void SetVelocity(const glm::vec3& velocity) = 0;
		virtual glm::vec3 GetVelocity() const = 0;

		virtual void ApplyTorque(const glm::vec3& torque) = 0;
		virtual void SetAngularVelocity(const glm::vec3& angularVelocity) = 0;
		virtual glm::vec3 GetAngularVelocity() const = 0;

		virtual void SetPosition(const glm::vec3& position) = 0;
		virtual glm::vec3 GetPosition() const = 0;

		virtual void SetRotation(const glm::vec3& rotation) = 0;
		virtual glm::vec3 GetRotation() const = 0;

		virtual void Read(const json::value_t& value);

	protected:
		float m_mass{ 1.0f };
		float m_damping{ 0.0f };
		float m_angularDamping{ 0.0f };
		float m_gravityScale{ 1.0f };
	};
}