#include "Engine.h"
#include "Core/File.h"
#include "Renderer/VertexBuffer.h"
#include "Renderer/IndexBuffer.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Shader.h"
#include "Resources/ResourceManager.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

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

    auto vb = std::make_shared<VertexBuffer>();
    vb->Create<Vertex>(vertices, Engine::Instance().GetRenderer());

    auto ib = std::make_shared<IndexBuffer>();
    ib->Create(indices, Engine::Instance().GetRenderer());


    auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Instance().GetRenderer(), 1);
    auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Instance().GetRenderer(), 1);

    struct vertexUniform
    {
        glm::mat4 mvp;
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

    glm::vec3 cameraPosition{ 0, 0, 5 };

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
        float dt = Engine::Instance().GetTime().GetDeltaTime();
        float time = Engine::Instance().GetTime().GetTime();

        //Color4 color{ 0, sin(time)+1.0f, 0,1 };

        glm::vec3 direction{ 0 };
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_W)) direction.z = 1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_A)) direction.x = -1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_S)) direction.z = -1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_D)) direction.x = 1;

        cameraPosition += direction * 4.0f * dt;

        // SHADER??

        auto mxt = glm::translate(glm::vec3(0.75f, 0.0f, 0.0f));
        auto mxr = glm::eulerAngleXYZ(0.0f, 0.0f, time * 3.0f);
        auto mxs = glm::scale(glm::vec3(0.5f, 0.5f, 0.5f));
        auto model = mxt * mxr * mxs;

        // VIEW MATRIX
        auto view = glm::lookAtRH(cameraPosition, glm::vec3{0}, glm::vec3{0, 1, 0});


        // PROJECTION MATRIX
        float aspect = Engine::Instance().GetRenderer().GetWidth() / (float)Engine::Instance().GetRenderer().GetHeight();
        auto projection = glm::perspectiveRH(glm::radians(70.0f), aspect, 0.01f, 20.0f);

        uniform.mvp = projection * view * model;

        // RENDER
        Engine::Instance().GetRenderer().BeginFrame();

        Engine::Instance().GetRenderer().SetPipeline(*pipeline);
        Engine::Instance().GetRenderer().SetVertexBuffer(*vb);
        Engine::Instance().GetRenderer().SetIndexBuffer(*ib);
        Engine::Instance().GetRenderer().SetVertexUniform(0, &uniform, sizeof(uniform));
        //Engine::Instance().GetRenderer().SetFragmentUniform(0, &color, sizeof(color));


        //Engine::Instance().GetRenderer().Draw(vb->GetVertexCount());
        Engine::Instance().GetRenderer().DrawIndex(*ib);

        Engine::Instance().GetRenderer().EndFrame();
    }

    // SHUTDOWN
    Engine::Instance().Shutdown();

    return 0;
}

