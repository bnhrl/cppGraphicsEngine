#pragma once
#include "RendererComponent.h"
#include "Renderer/Model.h"

namespace nu
{
	class ModelComponent : public RendererComponent
	{
	public:
		CLASS_PROTOTYPE(ModelComponent)

		virtual void Draw(const Renderer& renderer) override;
		virtual void Read(const json::value_t& value) override;

	private:
		res_t<Model> m_model;
		std::vector<res_t<class MaterialInstance>> m_materials;
	};
}