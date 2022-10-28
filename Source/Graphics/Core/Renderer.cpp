#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp> //translate, rotate, scale, perspective 
#include "LLGL/Utility.h"
#include "ResourceManager.h"
#include "Tile.h"

#include "Renderer.h"

static constexpr int SCREEN_WIDTH = 800;
static constexpr int SCREEN_HEIGHT = 600;

Renderer* Renderer::mInstance{nullptr};

Renderer::Renderer()
{
	// Initialize default projection matrix
	_Init();
}

Renderer::~Renderer()
{
	delete(mSpriteShader);
	delete(mVolumeShader);
	delete(mDebugShader);

	delete(mInstance);
}

Renderer* Renderer::GetInstance()
{
	if (mInstance == nullptr)
	{
		mInstance = new Renderer();
	}
	return mInstance;
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
			contextDesc.videoMode.resolution = {SCREEN_WIDTH, SCREEN_HEIGHT};
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
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	ResourceManager::GetInstance()->LoadAllTexturesFromFolder(*mRenderer);
	_InitSpritePipeline();
	//_InitVolumePipeline(); // TODO: Add volume pipeline properly
	_InitDebugDrawPipeline();
	// NOTE: Projection update must occur after debug shader is initialized
	UpdateProjection();
}

void Renderer::_InitSpritePipeline()
{
	mSpriteConstantBuffer = mRenderer->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(SpriteSettings)),
	                                                &spriteSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float});

	mSpriteShader = new Shader(*mRenderer, vertexFormat, "sprite.vert", "sprite.frag");

	// All layout bindings that will be used by graphics and compute pipelines
	LLGL::PipelineLayoutDescriptor layoutDesc;
	{
		layoutDesc.bindings =
		{
			LLGL::BindingDescriptor{
				LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer, LLGL::StageFlags::AllStages, 0
			},
			LLGL::BindingDescriptor{
				LLGL::ResourceType::Texture, LLGL::BindFlags::Sampled, LLGL::StageFlags::FragmentStage, 0u
			},
			LLGL::BindingDescriptor{LLGL::ResourceType::Sampler, 0, LLGL::StageFlags::FragmentStage, 0},
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
		pipelineDesc.rasterizer.cullMode = LLGL::CullMode::Disabled;
	}
	mSpritePipeline = mRenderer->CreatePipelineState(pipelineDesc);

	ResourceManager::GetInstance()->CreateResourceHeap(*mRenderer, *pipelineLayout, *mSpriteConstantBuffer);

	mNumVertices = static_cast<uint32_t>(mSpriteVertices.size());

	mSpriteVertexBuffer = mRenderer->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(mSpriteVertices.size() * sizeof(Vertex)), vertexFormat),
		mSpriteVertices.data()
	);
}

void Renderer::_InitVolumePipeline()
{
	mVolumeConstantBuffer = mRenderer->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(VolumeSettings)),
	                                                &volumeSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"normal", LLGL::Format::RGB32Float});
	vertexFormat.SetStride(sizeof(TexturedVertex));

	mVolumeShader = new Shader(*mRenderer, vertexFormat, "volume.vert", "volume.frag");

	// TODO: Hardcoded volume test
	std::vector<TexturedVertex> vertices;
	ResourceManager::GetInstance()->LoadObjModel(vertices, "../Data/Models/Pyramid.obj");
	model0.numVertices = static_cast<std::uint32_t>(vertices.size());

	mVolumeVertexBuffer = mRenderer->CreateBuffer(
		VertexBufferDesc(static_cast<std::uint32_t>(vertices.size() * sizeof(Vertex)), vertexFormat),
		vertices.data()
	);

	// All layout bindings that will be used by graphics and compute pipelines
	LLGL::PipelineLayoutDescriptor layoutDesc;
	{
		layoutDesc.bindings =
		{
			LLGL::BindingDescriptor{
				LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer, LLGL::StageFlags::AllStages, 0
			}
		};
	}
	// Create pipeline layout
	const auto pipelineLayout = mRenderer->CreatePipelineLayout(layoutDesc);

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.shaderProgram = &mVolumeShader->GetShaderProgram();
		pipelineDesc.pipelineLayout = pipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleStrip;
	}
	mVolumePipeline = mRenderer->CreatePipelineState(pipelineDesc);

	// Create resource heap for constant buffer
	LLGL::ResourceHeapDescriptor heapDesc;
	{
		heapDesc.pipelineLayout = pipelineLayout;
		heapDesc.resourceViews = {mVolumeConstantBuffer};
	}
	testVolumeResourceHeap = mRenderer->CreateResourceHeap(heapDesc);
}

void Renderer::_InitDebugDrawPipeline()
{
	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});

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

void Renderer::_DrawSprites() const
{
	// set graphics pipeline
	mCommands->SetPipelineState(*mSpritePipeline);
	mCommands->SetVertexBuffer(*mSpriteVertexBuffer);

	// TODO: Should there be separate pipelines for future object types?
	// Should Debug draw be a render object and we can handle switching pipelines?
	const auto renderObjects = ResourceManager::GetInstance()->GetDrawList();
	for (const auto obj : renderObjects)
	{
		obj->Draw();
	}
}

void Renderer::_DrawVolumes()
{
	// Set resources
	mCommands->SetPipelineState(*mVolumePipeline);
	mCommands->SetVertexBuffer(*mVolumeVertexBuffer);
	mCommands->SetResourceHeap(*testVolumeResourceHeap);

	static bool doOnce = true;
	if (doOnce)
	{
		volumeSettings.pvmMat = glm::identity<glm::mat4>();
		volumeSettings.color = {1, 1, 1, 1};
		mCommands->UpdateBuffer(*mVolumeConstantBuffer, 0, &volumeSettings, sizeof(VolumeSettings));

		doOnce = false;
	}

	mCommands->Draw(model0.numVertices, model0.firstVertex);
}

void Renderer::_DrawDebug() const
{
	if (!mDebugVertexBuffers.empty())
	{
		mCommands->SetPipelineState(*mDebugDrawPipeline);

		for (const auto& vb : mDebugVertexBuffers)
		{
			// set graphics pipeline
			mCommands->SetVertexBuffer(*vb.mVertexBuffer);
			mCommands->Draw(vb.mVertices.size(), 0);
		}
	}
}

void Renderer::OnDrawFrame(const std::function<void()>& drawCallback) const
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
			//_DrawVolumes();
			// gui draw calls, this can include images so we want it to
			// piggyback off the pipeline change in the sprite draw call
			drawCallback();

			_DrawDebug();
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
	ResourceManager::GetInstance()->SetTexture(*mCommands, textureId);
}

void Renderer::UpdateProjectionViewModelUniform(glm::mat4 model)
{
	spriteSettings.pvmMat = mProjection * mView * model;
	mCommands->UpdateBuffer(*mSpriteConstantBuffer, 0, &spriteSettings, sizeof(SpriteSettings));
}

void Renderer::UpdateTextureClipUniform(glm::mat4 textureClip)
{
	spriteSettings.textureClip = textureClip;
	mCommands->UpdateBuffer(*mSpriteConstantBuffer, 0, &spriteSettings, sizeof(SpriteSettings));
}

// Called on window resize
void Renderer::UpdateProjection()
{
	const auto res = mContext->GetVideoMode().resolution;
	mProjection = glm::perspective(glm::radians(45.0f),
	                               static_cast<float>(res.width) / static_cast<float>(res.height),
	                               0.1f, 1000.0f);

	// TODO: special case for debug shader, move MVP calculation out of shader similar to sprite
	const LLGL::UniformLocation projectionUniform = mDebugShader->GetShaderProgram().FindUniformLocation("projection");
	mCommands->SetUniform(projectionUniform, &mProjection, sizeof(mProjection));
}

void Renderer::UpdateView(glm::mat4 view)
{
	mView = view;

	// TODO: special case for debug shader, move MVP calculation out of shader similar to sprite
	const LLGL::UniformLocation viewUniform = mDebugShader->GetShaderProgram().FindUniformLocation("view");
	mCommands->SetUniform(viewUniform, &mView, sizeof(mView));
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
		vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float});
		vertexFormat.AppendAttribute({"color", LLGL::Format::RGB32Float});
		LLGL::BufferDescriptor vertexBufferDesc;
		{
			// NOTE: Even though this VB isn't initialized with starting data, we need to
			// specify a larger size for when we update the buffer with data
			vertexBufferDesc.size = sizeof(DebugVertex) * vertices.size(); // Size (in bytes) of the vertex buffer
			vertexBufferDesc.bindFlags = LLGL::BindFlags::VertexBuffer;
			// Enables the buffer to be bound to a vertex buffer slot
			vertexBufferDesc.vertexAttribs = vertexFormat.attributes; // Vertex format layout
		}
		const auto vertexBuffer = mRenderer->CreateBuffer(vertexBufferDesc, vertices.data());

		mDebugVertexBuffers.push_back({vertexBuffer, vertices});
	}
}

void Renderer::ClearDebugDraw()
{
	mDebugVertexBuffers.clear();
}

void Renderer::DrawSprite() const
{
	mCommands->Draw(mNumVertices, 0);
}

void Renderer::_AddDebugLineToVB(const Line* debug, std::vector<DebugVertex>& vertices)
{
	vertices.push_back({{debug->pointA}, {debug->color}});
	vertices.push_back({{debug->pointB}, {debug->color}});
}

void Renderer::_AddDebugBoxToVB(const Box* debug, std::vector<DebugVertex>& vertices)
{
	vertices.push_back({{debug->pointA}, {debug->color}});
	vertices.push_back({{debug->pointB}, {debug->color}});

	vertices.push_back({{debug->pointB}, {debug->color}});
	vertices.push_back({{debug->pointD}, {debug->color}});

	vertices.push_back({{debug->pointD}, {debug->color}});
	vertices.push_back({{debug->pointC}, {debug->color}});

	vertices.push_back({{debug->pointC}, {debug->color}});
	vertices.push_back({{debug->pointA}, {debug->color}});
}
