#include "Engine.h"
#include "Core/File.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Shader.h"
#include "Renderer/Model.h"
#include "Renderer/Texture.h"
#include "Resources/ResourceManager.h"
#include "Math/Transform.h"
#include "Framework/Scene.h"
#include "Renderer/Light.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <Components/CameraComponent.h>

using namespace nu;


struct Color4
{
    float r, g, b, a;
};

struct Vertex
{
    Vector3 position;
    Color4 color;
};

std::vector<Vertex> vertices =
{
    Vertex{{ -1.0f, -1.0f, 0.0f}, Color4(1, 0, 0, 1)}, // Bottom-Left
    Vertex{{  1.0f, -1.0f, 0.0f}, Color4(0, 0, 1, 1)}, // Bottom-Right
    Vertex{{  0.0f,  1.0f, 0.5f}, Color4(0, 1, 0, 1)}, // Top-Right
    Vertex{{  -1.0f,  1.0f, 1.0f}, Color4(0, 1, 0, 1)}, // Top-Left
};

std::vector<uint32_t> indices = {0, 1, 2, 3, 0};

int main()
{
    SetWorkingDirectory("Assets");

    // INITIALIZATION
    Engine::Instance().Initialize();

    Scene scene;
    scene.Load("scenes/scene.json");

    // ENGINE PARTS
    auto& engineTime = Engine::Instance().GetTime();
    auto& renderer = Engine::Instance().GetRenderer();
    auto& editor = Engine::Instance().GetEditor();

    // MAIN LOOP
    bool quit = false;
    while (!quit) 
    {
        // UPDATE
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_QUIT) 
                quit = true;
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
                quit = true;
        }

        // ENGINE
        Engine::Instance().Update();
        float dt = engineTime.GetDeltaTime();
        float time = engineTime.GetTime();

        // SCENE
        scene.Update(dt);

        // EDITOR
        editor.BeginFrame();
        editor.Update();

        // RENDER
        renderer.BeginFrame();

        scene.Draw(renderer);

        renderer.EndRenderPass();
        editor.Draw(renderer);
        renderer.EndFrame();
    }

    // SHUTDOWN
    Engine::Instance().Shutdown();

    return 0;
}

