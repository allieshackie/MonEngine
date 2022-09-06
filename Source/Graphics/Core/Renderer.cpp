#include <glm/ext/matrix_clip_space.hpp>
#include "LLGL/Utility.h"

#include "ResourceManager.h"
#include "Sprite.h"

#include "Renderer.h"

static constexpr int SCREEN_WIDTH = 800;
static constexpr int SCREEN_HEIGHT = 600;

Renderer::Renderer()
{
    // Initialize default projection matrix
    _Init();
}

void Renderer::_Init()
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

    _InitSpritePipeline();
    _InitDebugDrawPipeline();
    UpdateProjection();
}

void Renderer::_InitSpritePipeline()
{
    mConstantBuffer = mRenderer->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(Settings)), &settings);

    LLGL::VertexFormat vertexFormat;
    vertexFormat.AppendAttribute({ "position", LLGL::Format::RG32Float });
    vertexFormat.AppendAttribute({ "texCoord", LLGL::Format::RG32Float });

    mSpriteShader = new Shader(*mRenderer, vertexFormat, "sprite.vert", "sprite.frag");

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
    const auto pipelineLayout = mRenderer->CreatePipelineLayout(layoutDesc);

    // Create graphics pipeline
    LLGL::GraphicsPipelineDescriptor pipelineDesc;
    {
        pipelineDesc.shaderProgram = &mSpriteShader->GetShaderProgram();
        pipelineDesc.pipelineLayout = pipelineLayout;
        pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
    }
    mSpritePipeline = mRenderer->CreatePipelineState(pipelineDesc);

    ResourceManager::LoadAllTexturesFromFolder(*mRenderer);
	ResourceManager::CreateResourceHeap(*mRenderer, *pipelineLayout, *mConstantBuffer);

    mNumVertices = static_cast<uint32_t>(mSpriteVertices.size());

    mSpriteVertexBuffer = mRenderer->CreateBuffer(
        LLGL::VertexBufferDesc(static_cast<std::uint32_t>(mSpriteVertices.size() * sizeof(Vertex)), vertexFormat),
        mSpriteVertices.data()
    );
}

void Renderer::_InitDebugDrawPipeline()
{
    LLGL::VertexFormat vertexFormat;
    vertexFormat.AppendAttribute({ "position", LLGL::Format::RG32Float });
    vertexFormat.AppendAttribute({ "color", LLGL::Format::RGB32Float });

    mDebugShader = new Shader(*mRenderer, vertexFormat, "debug.vert", "debug.frag");

    // Create graphics pipeline
    LLGL::GraphicsPipelineDescriptor pipelineDesc;
    {
        pipelineDesc.shaderProgram = &mDebugShader->GetShaderProgram();
        pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::LineList;
    }
    mDebugDrawPipeline = mRenderer->CreatePipelineState(pipelineDesc);

    mCommands->SetPipelineState(*mDebugDrawPipeline);
}

void Renderer::_DrawSprites()
{
    // set graphics pipeline
    mCommands->SetPipelineState(*mSpritePipeline);

    mCommands->SetVertexBuffer(*mSpriteVertexBuffer);

    const auto spritesList = ResourceManager::GetSpritesList();
    for (const auto& pair : spritesList)
    {
        SetTexture(pair.first);
        pair.second->UpdateDrawData();
        pair.second->UpdateTextureClip();
        UpdateModelSettings(pair.second->GetSpriteModelData(), pair.second->GetTextureClip());

		mCommands->Draw(mNumVertices, 0);
    }
}

void Renderer::_DrawDebug()
{
    if (!mDebugVertexBuffers.empty())
    {
    	mCommands->SetPipelineState(*mDebugDrawPipeline);
	    
	    for (const auto& vb: mDebugVertexBuffers)
	    {
		    // set graphics pipeline
		    mCommands->SetVertexBuffer(*vb.mVertexBuffer);
			mCommands->Draw(vb.mVertices.size(), 0);
	    }
    }
}

void Renderer::OnDrawFrame(const std::function<void()>& drawCallback)
{ 
     // Render Commands to Queue
    mCommands->Begin();
    {
        // set viewport and scissor rectangle
        mCommands->SetViewport(mContext->GetResolution());

        // set the render context as the initial render target
        mCommands->BeginRenderPass(*mContext);
        {
		    // clear color buffer
		    mCommands->Clear(LLGL::ClearFlags::Color);
            _DrawSprites();
            _DrawDebug();
        	
            drawCallback();
            
        }
        mCommands->EndRenderPass();
    }
    mCommands->End();
    mCommandQueue->Submit(*mCommands);

    // Present results on screen
    mContext->Present();
}


void Renderer::SetTexture(int textureId) const
{
    ResourceManager::SetTexture(*mCommands, textureId);
}

void Renderer::UpdateModelSettings(glm::mat4 model, glm::mat4 textureClip)
{
    // Update constant buffer (shader uniforms) with new "model view" value
    settings.model = model;
    settings.textureClip = textureClip;
    mCommands->UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

// Called on window resize
void Renderer::UpdateProjection()
{
    const auto res = mContext->GetVideoMode().resolution;
    const auto projection = glm::ortho(0.0f, static_cast<float>(res.width), static_cast<float>(res.height), 0.0f, -1.0f, 1.0f);

    mCommands->SetPipelineState(*mSpritePipeline);
	LLGL::UniformLocation projectionUniform = mSpriteShader->GetShaderProgram().FindUniformLocation("projection");
    mCommands->SetUniform(projectionUniform, &projection, sizeof(projection));

    mCommands->SetPipelineState(*mDebugDrawPipeline);
    projectionUniform = mDebugShader->GetShaderProgram().FindUniformLocation("projection");
    mCommands->SetUniform(projectionUniform, &projection, sizeof(projection));
}

void Renderer::UpdateView(glm::mat4 view)
{
    mCommands->SetPipelineState(*mSpritePipeline);
    LLGL::UniformLocation viewUniform = mSpriteShader->GetShaderProgram().FindUniformLocation("view");
    mCommands->SetUniform(viewUniform, &view, sizeof(view));

    mCommands->SetPipelineState(*mDebugDrawPipeline);
    viewUniform = mDebugShader->GetShaderProgram().FindUniformLocation("view");
    mCommands->SetUniform(viewUniform, &view, sizeof(view));
}

void Renderer::AddDebugDrawToVB(DebugDrawable* debug)
{
    std::vector<DebugVertex> vertices;
	// Line case
    if (const auto line = dynamic_cast<Line*>(debug))
    {
        _AddDebugLineToVB(line, vertices);
    }

    // Box case
    if (const auto box = dynamic_cast<Box*>(debug))
    {
        _AddDebugBoxToVB(box, vertices);
    }

    // Grid case
    if (const auto grid = dynamic_cast<Grid*>(debug))
    {
        _AddDebugBoxToVB(&grid->mOutline, vertices);
        for (const auto line : grid->mLines)
        {
            _AddDebugLineToVB(&line, vertices);
        }
    }

    if (!vertices.empty())
    {
	    LLGL::VertexFormat vertexFormat;
	    vertexFormat.AppendAttribute({ "position", LLGL::Format::RG32Float });
	    vertexFormat.AppendAttribute({ "color", LLGL::Format::RGB32Float });
	    LLGL::BufferDescriptor vertexBufferDesc;
	    {
	        // NOTE: Even though this VB isn't initialized with starting data, we need to
	        // specify a larger size for when we update the buffer with data
	        vertexBufferDesc.size = sizeof(DebugVertex) * vertices.size();  // Size (in bytes) of the vertex buffer
	        vertexBufferDesc.bindFlags = LLGL::BindFlags::VertexBuffer;    // Enables the buffer to be bound to a vertex buffer slot
	        vertexBufferDesc.vertexAttribs = vertexFormat.attributes;      // Vertex format layout
	    }
	    const auto vertexBuffer = mRenderer->CreateBuffer(vertexBufferDesc, vertices.data());

        mDebugVertexBuffers.push_back({ vertexBuffer, vertices });
    }
}

void Renderer::ClearDebugDraw()
{
    mDebugVertexBuffers.clear();
}

void Renderer::_AddDebugLineToVB(const Line* debug, std::vector<DebugVertex>& vertices)
{
    vertices.push_back({ {debug->pointA}, {debug->color} });
    vertices.push_back({ {debug->pointB}, {debug->color} });
}

void Renderer::_AddDebugBoxToVB(const Box* debug, std::vector<DebugVertex>& vertices)
{
    vertices.push_back({ {debug->pointA}, {debug->color} });
    vertices.push_back({ {debug->pointB}, {debug->color} });

    vertices.push_back({ {debug->pointB}, {debug->color} });
    vertices.push_back({ {debug->pointD}, {debug->color} });

    vertices.push_back({ {debug->pointD}, {debug->color} });
    vertices.push_back({ {debug->pointC}, {debug->color} });

    vertices.push_back({ {debug->pointC}, {debug->color} });
    vertices.push_back({ {debug->pointA}, {debug->color} });
}

