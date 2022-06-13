#pragma once
#include <LLGL/LLGL.h>

class CustomSurface;
class Sprite;

class Renderer {
public:
	Renderer();
	~Renderer() = default;

    void OnDrawFrame(const std::function<void()>& drawCallback) const;

    LLGL::RenderContext& GetContext() const
    {
        return *mContext;
    }

    const std::unique_ptr<LLGL::RenderSystem>& GetRendererSystem() const
    {
        return mRenderer;
    }
	
    void CreatePipelines();
    void OnDrawInit();

private:
    void _InitLLGL();

	
    std::unique_ptr<LLGL::RenderSystem> mRenderer; // Render system

    LLGL::RenderContext* mContext = nullptr; // Main render context

    LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer

    LLGL::CommandQueue* mCommandQueue = nullptr;  // Command queue

    LLGL::PipelineLayout* mPipelineLayout = nullptr;

    LLGL::PipelineState* mPipeline = nullptr;
};