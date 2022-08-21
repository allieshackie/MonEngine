#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include "Camera.h"
#include "Vertex.h"

class CustomSurface;
class Sprite;

class Renderer {
public:
	Renderer();
	~Renderer() = default;

    void OnDrawFrame(const std::function<void()>& drawCallback);

    LLGL::RenderContext& GetContext() const
    {
        return *mContext;
    }

    const std::unique_ptr<LLGL::RenderSystem>& GetRendererSystem() const
    {
        return mRenderer;
    }
	
    void OnDrawInit();

    void OnDrawAll();
	
    void CreatePipelines();
    void CreateSpriteVertexBuffer();

    void SetTexture(int textureId) const;

    void UpdateModelSettings(glm::mat4 model, glm::mat4 textureClip);
    void UpdateProjection();
    void UpdateView(glm::mat4 view);

private:
    void _InitLLGL();

    std::unique_ptr<LLGL::RenderSystem> mRenderer; // Render system

    LLGL::RenderContext* mContext = nullptr; // Main render context

    LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer

    LLGL::CommandQueue* mCommandQueue = nullptr;  // Command queue

    LLGL::PipelineLayout* mPipelineLayout = nullptr;

    LLGL::PipelineState* mPipeline = nullptr;

	// Vertex Data
    LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mVertexBuffer = nullptr;

    struct Settings
    {
        glm::mat4 projection;
        glm::mat4 view;
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
};