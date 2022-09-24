#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "Camera.h"
#include "DebugDraw.h"
#include "Shader.h"
#include "Vertex.h"

class Renderer {
public:
	Renderer();
	~Renderer() = default;

    static Renderer* GetInstance();

    void OnDrawFrame(const std::function<void()>& drawCallback);

    LLGL::RenderContext& GetContext() const
    {
        return *mContext;
    }

    const std::unique_ptr<LLGL::RenderSystem>& GetRendererSystem() const
    {
        return mRenderer;
    }

    void SetTexture(int textureId) const;

    void UpdateModelSettings(glm::mat4 model, glm::mat4 textureClip);
    void UpdateProjection();
    void UpdateView(glm::mat4 view);

    void AddDebugDrawToVB(DebugDrawable* debug);
    void ClearDebugDraw();

private:
    void _Init();
    void _InitSpritePipeline();
    void _InitDebugDrawPipeline();

    void _DrawSprites();
    void _DrawDebug();

    void _AddDebugLineToVB(const Line* debug, std::vector<DebugVertex>& vertices);
    void _AddDebugBoxToVB(const Box* debug, std::vector<DebugVertex>& vertices);

    std::unique_ptr<LLGL::RenderSystem> mRenderer; // Render system
    LLGL::RenderContext* mContext = nullptr; // Main render context
    LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
    LLGL::CommandQueue* mCommandQueue = nullptr;  // Command queue

    // Pipelines
    LLGL::PipelineState* mSpritePipeline = nullptr;
    LLGL::PipelineState* mDebugDrawPipeline = nullptr;

    // Shaders
    Shader* mSpriteShader = nullptr;
    Shader* mDebugShader = nullptr;

	// Vertex Data
    LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mSpriteVertexBuffer = nullptr;
    template <typename T>
    struct VBData
    {
        LLGL::Buffer* mVertexBuffer;
        std::vector<T> mVertices;
    };
	std::vector<VBData<DebugVertex>> mDebugVertexBuffers;

    struct Settings
    {
        glm::mat4 model;
        glm::mat4 textureClip;
    }
    settings = {};

    const std::vector<Vertex> mSpriteVertices = {
    { { -1,  1 }, { 1, 0 } }, // top left
    { { -1, -1 }, { 1,  1 } }, // bottom left
    { {  1,  1 }, {  0, 0 } }, // top right
    { {  1, -1 }, {  0,  1 } }, // bottom right
    };

    uint32_t mNumVertices = 0;
    glm::mat4 mProjection = glm::mat4(1.0f);

    static Renderer* mInstance;
};