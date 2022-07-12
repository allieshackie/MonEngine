#include "ResourceManager.h"
#include "Sprite.h"
#include "LLGL/Utility.h"
#include <glm/ext/matrix_clip_space.hpp>

#include "Renderer.h"


static constexpr int SCREEN_WIDTH = 800;
static constexpr int SCREEN_HEIGHT = 600;

Renderer::Renderer()
{
    // Initialize default projection matrix
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
    mConstantBuffer = mRenderer->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(Settings)), &settings);
    UpdateProjection();

	// this step creates shader program
	ResourceManager::LoadShaderProgram("sprite.vert", "sprite.frag");
	CreatePipelines();
	ResourceManager::LoadAllTexturesFromFolder();
	ResourceManager::CreateResourceHeap(*mPipelineLayout, *mConstantBuffer);

    CreateSpriteVertexBuffer();
}

void Renderer::OnDrawAll()
{
    const auto spritesList = ResourceManager::GetSpritesList();
	for (const auto& pair : spritesList)
	{
        SetTexture(pair.first);
        pair.second->UpdateDrawData();
        pair.second->UpdateTextureClip();
        UpdateSettings(pair.second->GetSpriteModelData(), pair.second->GetTextureClip());

        mCommands->Draw(mNumVertices, 0);
	}
}

void Renderer::OnDrawFrame(const std::function<void()>& drawCallback)
{ 
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

            OnDrawAll();
        	
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
	// All layout bindings that will be used by graphics and compute pipelines
    LLGL::PipelineLayoutDescriptor layoutDesc;
    {
        layoutDesc.bindings =
        {
            LLGL::BindingDescriptor{ LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer, LLGL::StageFlags::AllStages, 0 },
            LLGL::BindingDescriptor{ LLGL::ResourceType::Texture, LLGL::BindFlags::Sampled, LLGL::StageFlags::FragmentStage, 0u },
            LLGL::BindingDescriptor{ LLGL::ResourceType::Sampler, 0, LLGL::StageFlags::FragmentStage, 0 },
        };
    }
	// Create pipeline layout
    mPipelineLayout = mRenderer->CreatePipelineLayout(layoutDesc);

	// Create graphics pipeline
    LLGL::GraphicsPipelineDescriptor pipelineDesc;
    {
        pipelineDesc.shaderProgram = &ResourceManager::GetShaderProgram();
        pipelineDesc.pipelineLayout = mPipelineLayout;
        pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
    }
    mPipeline = mRenderer->CreatePipelineState(pipelineDesc);
}

void Renderer::CreateSpriteVertexBuffer()
{
    mNumVertices = static_cast<uint32_t>(mSpriteVertices.size());

    mVertexBuffer = mRenderer->CreateBuffer(
        LLGL::VertexBufferDesc(static_cast<std::uint32_t>(mSpriteVertices.size() * sizeof(Vertex)), ResourceManager::GetVertexFormat()),
        mSpriteVertices.data()
    );

    mCommands->SetVertexBuffer(*mVertexBuffer);
}

void Renderer::SetTexture(int textureId) const
{
    ResourceManager::SetTexture(*mCommands, textureId);
}

void Renderer::UpdateSettings(glm::mat4 view, glm::mat4 textureClip)
{
    // Update constant buffer (shader uniforms) with new "model view" value
    settings.model = view;
    settings.textureClip = textureClip;
    mCommands->UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

// Called on window resize
void Renderer::UpdateProjection()
{
    const auto res = mContext->GetVideoMode().resolution;
    settings.projection = glm::ortho(0.0f, static_cast<float>(res.width), static_cast<float>(res.height), 0.0f, -1.0f, 1.0f);
    mCommands->UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}
