#include "ResourceManager.h"
#include "Sprite.h"

#include "Renderer.h"

static constexpr int SCREEN_WIDTH = 800;
static constexpr int SCREEN_HEIGHT = 600;

Renderer::Renderer()
{
    _InitLLGL();
}

void Renderer::_InitLLGL()
{
    try
    {
        // Load render system module (hard code to OpenGL for now)
        mRenderer = LLGL::RenderSystem::Load("OpenGL");
    	
        // Render context (window attributes)
        LLGL::RenderContextDescriptor contextDesc;
        {
            contextDesc.videoMode.resolution = { SCREEN_WIDTH, SCREEN_HEIGHT };
            contextDesc.vsync.enabled = true;
#ifdef ENABLE_MULTISAMPLING
            contextDesc.samples = 8;
#endif
        }
        mContext = mRenderer->CreateRenderContext(contextDesc);

        // Get command queue to record and submit command buffers
        mCommandQueue = mRenderer->GetCommandQueue();

        // Create command buffer to submit graphics commands
        mCommands = mRenderer->CreateCommandBuffer();
    }
    catch (const std::exception & e) {
        std::cerr << e.what() << std::endl;
    }
}
/*
 * Must be called after Renderer constructor finishes so that Singleton instance
 * can be used to create/load the shader program
 */
void Renderer::OnDrawInit()
{
	// this step creates shader program
	ResourceManager::LoadShaderProgram("texture.vert", "texture.frag");
	CreatePipelines();
	//ResourceManager::LoadAllTexturesFromFolder();
	//ResourceManager::CreateResourceHeap(*mPipelineLayout);
}

void Renderer::OnDrawFrame(const std::function<void()>& drawCallback) const
{
    const auto& sprites = ResourceManager::GetSpriteList();
    
     // Render Commands to Queue
    mCommands->Begin();
    {
        // set viewport and scissor rectangle
        mCommands->SetViewport(mContext->GetResolution());

        // set graphics pipeline
        mCommands->SetPipelineState(*mPipeline);

        // set the render context as the initial render target
        mCommands->BeginRenderPass(*mContext);
        {
            // clear color buffer
            mCommands->Clear(LLGL::ClearFlags::Color);

        	// Draw each sprite
            for (const auto& sprite : sprites)
            {
                sprite->Draw(*mCommands);
            }
        	
            drawCallback();
            
        }
        mCommands->EndRenderPass();
    }
    mCommands->End();
    mCommandQueue->Submit(*mCommands);

    // Present results on screen
    mContext->Present();
}

void Renderer::CreatePipelines()
{
	// Create pipeline layout
	// All layout bindings that will be used by graphics and compute pipelines
    LLGL::PipelineLayoutDescriptor layoutDesc;
    {
        layoutDesc.bindings = {
            LLGL::BindingDescriptor{ LLGL::ResourceType::Sampler, 0, LLGL::StageFlags::FragmentStage, 0},
            LLGL::BindingDescriptor{ LLGL::ResourceType::Texture, LLGL::BindFlags::Sampled, LLGL::StageFlags::FragmentStage, 0u}, // 0u for OpenGL
        };
    }
    mPipelineLayout = mRenderer->CreatePipelineLayout(layoutDesc);

	// Create graphics pipeline
    LLGL::GraphicsPipelineDescriptor pipelineDesc;
    {
        pipelineDesc.shaderProgram = &ResourceManager::GetShaderProgram();
        pipelineDesc.pipelineLayout = mPipelineLayout;
        pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
    	// TODO: multi sample?
    	// pipelineDesc.rasterizer.multiSampleEnabled  = (GetSampleCount() > 1);
    }
    mPipeline = mRenderer->CreatePipelineState(pipelineDesc);
}

