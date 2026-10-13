#include "Engine.h"
#include "Core/File.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Shader.h"
#include "Resources/ResourceManager.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

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
    Vertex{{  0.0f,  1.0f, 0.0f}, Color4(0, 1, 0, 1)}, // Top-Middle
};

int main()
{
    SetWorkingDirectory("Assets");

    // INITIALIZATION
    Engine::Instance().Initialize();

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Instance().GetRenderer().GetGPUDevice());

    auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Instance().GetRenderer(), 1);
    auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Instance().GetRenderer(), 1);

    struct vertexUniform
    {
        glm::mat4 model;
    };

    vertexUniform uniform;

    auto pipeline = std::make_shared<Pipeline>();
    pipeline->AddVertexBuffer(sizeof(Vertex));
    // position
    pipeline->AddVertexAttribute(
        0,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
        offsetof(Vertex, position.x));
    // color
    pipeline->AddVertexAttribute(
        1,
        SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
        offsetof(Vertex, color));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Instance().GetRenderer().GetGPUDevice(),
        Engine::Instance().GetRenderer().GetWindow()
    );


    // MAIN LOOP
    bool quit = false;
    while (!quit) 
    {
        // UPDATE
        SDL_Event event;
        while (SDL_PollEvent(&event)) 
        {
            if (event.type == SDL_EVENT_QUIT) 
            {
                quit = true;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE)
            {
                quit = true;
            }
        }

        // ENGINE
        Engine::Instance().Update();

        // SHADER??
        float dt = Engine::Instance().GetTime().GetDeltaTime();
        float time = Engine::Instance().GetTime().GetTime();

        Color4 color{ 0, sin(time)+1.0f, 0,1 };
        auto mxt = glm::translate(glm::mat4{ 1.0f }, glm::vec3(0.5f, 0.0f, 0.0f));
        auto mxr = glm::rotate(glm::mat4{ 1.0f }, time, glm::vec3(0.0f, sin(time), 1.0f));
        auto mxs = glm::scale(glm::mat4{1.0f}, glm::vec3(0.5f, 0.5f, 0.5f));

        uniform.model = mxt * mxr * mxs;

        // RENDER
        Engine::Instance().GetRenderer().BeginFrame();

        Engine::Instance().GetRenderer().SetPipeline(*pipeline);
        Engine::Instance().GetRenderer().SetVertexBuffer(*vb);
        Engine::Instance().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        Engine::Instance().GetRenderer().SetFragmentUniform(0, &color, sizeof(color));


        Engine::Instance().GetRenderer().Draw(vb->GetVertexCount());

        Engine::Instance().GetRenderer().EndFrame();
    }

    // SHUTDOWN
    Engine::Instance().Shutdown();

    return 0;
}

