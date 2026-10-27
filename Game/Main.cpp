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

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>

using namespace nu;

enum class LightType : uint32_t
{
    Directional, Point, Spot
};

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


    //auto vshader = Resources().Get<nu::Shader>("shaders/position.vert", Engine::Instance().GetRenderer(), 1);
    //auto fshader = Resources().Get<nu::Shader>("shaders/color.frag", Engine::Instance().GetRenderer(), 1);

    glm::vec3 cameraPosition{ 0, 0, 5 };

    auto model = Resources().Get<Model>("models/spot.obj", Engine::Instance().GetRenderer());
    auto texture = Resources().Get<Texture>("textures/spot_diffuse.png", Engine::Instance().GetRenderer());

    auto vshader = Resources().Get<nu::Shader>("shaders/gouraud.vert", Engine::Instance().GetRenderer(), 3);
    auto fshader = Resources().Get<nu::Shader>("shaders/gouraud.frag", Engine::Instance().GetRenderer(), 1, 1);

    auto pipeline = std::make_shared<Pipeline>();

    pipeline->AddVertexBuffer(sizeof(Model::Vertex));
    pipeline->AddVertexAttribute(0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, position));
    pipeline->AddVertexAttribute(1, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, offsetof(Model::Vertex, texcoord));
    pipeline->AddVertexAttribute(2, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, normal));
    pipeline->AddVertexAttribute(3, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Model::Vertex, tangent));

    pipeline->Create(*vshader.get(), *fshader.get(),
        Engine::Instance().GetRenderer().GetGPUDevice(),
        Engine::Instance().GetRenderer().GetWindow());



    // UNIFORMS
    struct VertexUniform
    {
        glm::mat4 mvp;
        glm::mat4 model;
        glm::vec3 cameraPosition;
        float padding1;
    };
    
    struct LightUniform 
    {
        glm::vec3 position;
        LightType type;
        glm::vec3 direction;
        float innerCutoff{ 0.05f };
        glm::vec3 color;
        float intensity;
        glm::vec3 ambient;
        float outerCutoff{ 1.5f };
    };

    struct MaterialUniform
    {
        glm::vec4 color{ 1.f, 1.f, 1.f, 1.f };
        glm::vec2 tiling{ 1.f };
        glm::vec2 offset{ 0.f };
        float shininess{ 1.0f };
    };


    VertexUniform vertexUniform;

    LightUniform lightUniform
    {
        .position{0, 5, 0},
        .type{LightType::Spot},
        .direction{0, -1, 0},
        .color{0.5f, 1.0f, 0.5f},
        .intensity{1.0f},
        .ambient{0.25f}
    };

    MaterialUniform materialUniform
    {
        .color{1.f, 0.5f, 0.0f, 1.0f},
        .tiling{ 1.f},
        .offset{ 0.f, 0.f },
        .shininess{ 128.0f }
    };

    Transform transform;

    glm::vec4 tint{ 1,1,1,1 };



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

        glm::vec3 direction{ 0 };
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_W)) direction.z = 1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_A)) direction.x = -1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_S)) direction.z = -1;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_D)) direction.x = 1;

        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_F)) lightUniform.type = LightType::Directional;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_G)) lightUniform.type = LightType::Point;
        if (Engine::Instance().GetInput().GetKeyDown(SDL_SCANCODE_H)) lightUniform.type = LightType::Spot;

        cameraPosition += direction * 4.0f * dt;
        //materialUniform.offset.x = time * 0.333f;

        // TRANSFORM
        //transform.rotation.z = time;

        // VIEW MATRIX
        auto view = glm::lookAtRH(cameraPosition, glm::vec3{0}, glm::vec3{0, 1, 0});


        // PROJECTION MATRIX
        float aspect = Engine::Instance().GetRenderer().GetWidth() / (float)Engine::Instance().GetRenderer().GetHeight();
        auto projection = glm::perspectiveRH(glm::radians(90.0f), aspect, 0.01f, 20.0f);

        // MVP
        vertexUniform.model = transform.GetMatrix();
        vertexUniform.mvp = projection * view * vertexUniform.model;
        vertexUniform.cameraPosition = cameraPosition;

        // RENDER
        Engine::Instance().GetRenderer().BeginFrame();

        Engine::Instance().GetRenderer().SetPipeline(*pipeline);

        Engine::Instance().GetRenderer().SetVertexUniform(0, &vertexUniform, sizeof(vertexUniform));
        Engine::Instance().GetRenderer().SetVertexUniform(1, &lightUniform, sizeof(lightUniform));
        Engine::Instance().GetRenderer().SetVertexUniform(2, &materialUniform, sizeof(materialUniform));

        Engine::Instance().GetRenderer().SetFragmentUniform(0, &materialUniform, sizeof(materialUniform));
        Engine::Instance().GetRenderer().SetFragmentTexture(0, *texture);

        model->Draw(Engine::Instance().GetRenderer());

        Engine::Instance().GetRenderer().EndFrame();
    }

    // SHUTDOWN
    Engine::Instance().Shutdown();

    return 0;
}

