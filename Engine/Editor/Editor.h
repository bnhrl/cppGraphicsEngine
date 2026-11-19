#pragma once

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlgpu3.h>

namespace nu
{
	class Renderer;

	class IGui
	{
	public:
		virtual void UpdateGui() = 0;
	};

	class Editor
	{
	public:
		bool Initialize(Renderer& renderer);
		void Shutdown();

		void Update(class Scene& scene);
		void ProcessEvent(const SDL_Event& event);

		void BeginFrame();
		void Draw(Renderer& renderer);

		void SetActive(bool active = true) { m_active = active; }
		bool GetActive() { return m_active; }

		void SetSelected(class Object* selected) { m_selectedQueued = selected; }
		const class Object* GetSelected() const { return m_selected; }

	private:
		bool m_active = true;
		class Object* m_selected{ nullptr };
		class Object* m_selectedQueued{ nullptr };

		void UpdateHierarchy(class Scene& scene);
		void UpdateInspector();
		void UpdateAssets();
	};
}