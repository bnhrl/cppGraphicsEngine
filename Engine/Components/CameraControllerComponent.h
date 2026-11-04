#pragma once
#include "Component.h"
#include "Input/Input.h"

namespace nu
{
	class CameraControllerComponent : public Component
	{
	public:
		CLASS_PROTOTYPE(CameraControllerComponent)

		void Start() override;
		void Update(float dt) override;
		void Read(const json::value_t& value) override;

	private:
		Input m_input;
		float m_speed = 4.f;
		float m_yaw;
		float m_pitch;
		float m_sensitivity;
	};
}