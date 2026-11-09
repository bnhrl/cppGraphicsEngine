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

#define CHECK_UNIFORM_BOUNDARY(T) static_assert((sizeof(T) % 16) == 0);

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

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Instance().GetRenderer());

    auto ib = std::make_shared<IndexBuffer>();
    ib->Create(indices, Engine::Instance().GetRenderer());


    auto texture = Resources().Get<Texture>("textures/ogre_diffuse.bmp", Engine::Instance().GetRenderer());

    auto vshader = Resources().Get<nu::Shader>("shaders/phong.vert", Engine::Instance().GetRenderer(), 3);
    auto fshader = Resources().Get<nu::Shader>("shaders/phong.frag", Engine::Instance().GetRenderer(), 3, 1);
    //auto vshader = Resources().Get<nu::Shader>("shaders/gouraud.vert", Engine::Instance().GetRenderer(), 3);
    //auto fshader = Resources().Get<nu::Shader>("shaders/gouraud.frag", Engine::Instance().GetRenderer(), 1, 1);

    auto pipeline = std::make_shared<Pipeline>();

    pipeline->AddVertexBuffer(sizeof(Model::Vertex));
    pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
    pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
    pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
    pipeline->AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Instance().GetRenderer().GetGPUDevice(),
        Engine::Instance().GetRenderer().GetWindow());



    // ENGINE PARTS
    auto& engineTime = Engine::Instance().GetTime();
    auto& renderer = Engine::Instance().GetRenderer();

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

        // RENDER
        renderer.BeginFrame();

        renderer.SetPipeline(*pipeline);
        renderer.SetFragmentTexture(0, *texture);

        scene.Draw(renderer);

        renderer.EndFrame();
    }

    // SHUTDOWN
    Engine::Instance().Shutdown();

    return 0;
}

