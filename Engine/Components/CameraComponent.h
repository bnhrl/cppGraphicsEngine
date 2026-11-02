#pragma once
#include "RendererComponent.h"

namespace nu
{
	class CameraComponent : public RendererComponent
	{
	public:
		CLASS_PROTOTYPE(CameraComponent)

		void Update(float delta) override;
		void Draw(const Renderer& renderer) override;

		void Read(const json::value_t& value) override;

		float fov = 70.0f;
		float nearPlane{ 0.001f };
		float farPlane{ 333.0f };

	private:
		glm::mat4x4 m_view;
		glm::mat4x4 m_projection;

	};
}