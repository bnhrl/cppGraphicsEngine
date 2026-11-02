#pragma once
#include "RendererComponent.h"

namespace nu
{
	class LightComponent : public RendererComponent
	{
	public:
		CLASS_PROTOTYPE(LightComponent)

		void Draw(const Renderer& renderer) override;

		//virtual void Read(const json::value_t& value) override;
	private:
	};
}