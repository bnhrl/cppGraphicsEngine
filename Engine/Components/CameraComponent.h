#pragma once
#include "RendererComponent.h"

namespace nu
{
	class CameraComponent : public RendererComponent
	{
	public:
		CLASS_PROTOTYPE(CameraComponent)

		void Start() override;
		void Update(float delta) override;
		void Draw(const Renderer& renderer) override;

		void Read(const json::value_t& value) override;

		const glm::mat4x4& GetView() const		 { return m_view; }
		const glm::mat4x4& GetProjection() const { return m_projection; }

		float fov = 70.0f;
		float nearPlane{ 0.001f };
		float farPlane{ 333.0f };


	private:
		glm::mat4x4 m_view{1.0f};
		glm::mat4x4 m_projection{1.0f};
		float m_aspectRatio{ 16.f / 9.f };
	};
}