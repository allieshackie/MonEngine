#include <SDL_image.h>
#include <glad/glad.h>

#include <LLGL/LLGL.h>
#include <LLGL/Utility.h>

#include "Window.h"

// Enable multi-sampling render context
#define ENABLE_MULTISAMPLING

#define ENABLE_CACHED_PSO

Window::Window() {
    initSDL();
}

Window::~Window() {
    closeSDL();
}

bool Window::initSDL() {
    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    //Create window
    mSDLWindow = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (mSDLWindow == nullptr)
    {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    //Create context
    mContext = SDL_GL_CreateContext(mSDLWindow);
    if (mContext == nullptr)
    {
        printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(mSDLWindow, mContext);

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
    //Use Vsync
    if (SDL_GL_SetSwapInterval(1) < 0)
    {
        printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
    }

    return true;
}

void Window::LLGLExample()
{
	try
	{
		// Load render system module (hard code to OpenGL for now)
        std::unique_ptr<LLGL::RenderSystem> renderer = LLGL::RenderSystem::Load("OpenGL");

		// Render context (window attributes)
        LLGL::RenderContextDescriptor contextDesc;
        {
            contextDesc.videoMode.resolution = { SCREEN_WIDTH, SCREEN_HEIGHT };
            contextDesc.vsync.enabled = true;
			#ifdef ENABLE_MULTISAMPLING
            contextDesc.samples = 8;
			#endif
        }
        LLGL::RenderContext* context = renderer->CreateRenderContext(contextDesc);

		// TODO: Can remove ==============
        const auto& info = renderer->GetRendererInfo();

        std::cout << "Renderer:         " << info.rendererName << std::endl;
        std::cout << "Device:           " << info.deviceName << std::endl;
        std::cout << "Vendor:           " << info.vendorName << std::endl;
        std::cout << "Shading Language: " << info.shadingLanguageName << std::endl;
		// TODO: ==========================

		// get window from context surface
        auto& window = LLGL::CastTo<LLGL::Window>(context->GetSurface());

		// Needs L outside of quotes?
        window.SetTitle(L"Magooooo");
        window.Show();

        Vertex vertices[] = {
        	{{0, s}, {255, 0 ,0 ,255}},
        	{{s, -s}, {0,255, 0,255}},
        	{{-s, -s}, {0,0,255,255}},
        };

		// required to specify vertex attributes inside a vertex buffer
        LLGL::VertexFormat vertexFormat;
        vertexFormat.AppendAttribute({ "position", LLGL::Format::RG32Float });
        vertexFormat.AppendAttribute({ "color", LLGL::Format::RGBA8UNorm });
        vertexFormat.SetStride(sizeof(Vertex));

		// Vertex buffer
        LLGL::BufferDescriptor vertexBufferDesc;
        {
            vertexBufferDesc.size = sizeof(vertices); // size in bytes of vertices list
            vertexBufferDesc.bindFlags = LLGL::BindFlags::VertexBuffer;
            vertexBufferDesc.vertexAttribs = vertexFormat.attributes;  // vertex format layout
        }

        LLGL::Buffer* vertexBuffer = renderer->CreateBuffer(vertexBufferDesc, vertices);

		// Shaders
        LLGL::Shader* vertShader = nullptr;
        LLGL::Shader* fragShader = nullptr;

        LLGL::ShaderDescriptor vertShaderDesc, fragShaderDesc;
        vertShaderDesc = { LLGL::ShaderType::Vertex,   "C:/dev/MonDev/Data/Shaders/example.vert" };
        fragShaderDesc = { LLGL::ShaderType::Fragment, "C:/dev/MonDev/Data/Shaders/example.frag" };

		// Specify vertex attributes for vertex shader
        vertShaderDesc.vertex.inputAttribs = vertexFormat.attributes;

        vertShader = renderer->CreateShader(vertShaderDesc);
        fragShader = renderer->CreateShader(fragShaderDesc);

		// TODO: Can remove, printing shader log
        for (auto shader : { vertShader, fragShader })
        {
            if (shader != nullptr)
            {
                std::string log = shader->GetReport();
                if (!log.empty())
                    std::cerr << log << std::endl;
            }
        }

		// Create shader program
        LLGL::ShaderProgramDescriptor shaderProgramDesc;
        {
            shaderProgramDesc.vertexShader = vertShader;
            shaderProgramDesc.fragmentShader = fragShader;
        }
        LLGL::ShaderProgram* shaderProgram = renderer->CreateShaderProgram(shaderProgramDesc);

		// Link shader program and log errors
		if (shaderProgram->HasErrors())
		{
            throw std::runtime_error(shaderProgram->GetReport());
		}

		// Graphics Pipeline
        LLGL::PipelineState* pipeline = nullptr;
        std::unique_ptr<LLGL::Blob> pipelineCache;


        LLGL::GraphicsPipelineDescriptor pipelineDesc;
        {
            pipelineDesc.shaderProgram = shaderProgram;
            pipelineDesc.renderPass = context->GetRenderPass();
#ifdef ENABLE_MULTISAMPLING
            pipelineDesc.rasterizer.multiSampleEnabled = (contextDesc.samples > 1);
#endif
        }

        // Create graphics PSO
        pipeline = renderer->CreatePipelineState(pipelineDesc);


		// Get command queue to record and submit command buffers
        LLGL::CommandQueue* queue = renderer->GetCommandQueue();

		// Create command buffer to submit graphics commands
        LLGL::CommandBuffer* commands = renderer->CreateCommandBuffer();


		// Main Loop
		while(window.ProcessEvents())
		{
            commands->Begin();
            {
	            // set viewport and scissor rectangle
                commands->SetViewport(context->GetResolution());

            	// set graphics pipeline
                commands->SetPipelineState(*pipeline);

            	// set vertex buffer
                commands->SetVertexBuffer(*vertexBuffer);

            	// set the render context as the initial render target
                commands->BeginRenderPass(*context);
                {
	                // clear color buffer
                    commands->Clear(LLGL::ClearFlags::Color);

                	// Draw triangle
                    commands->Draw(3, 0);
                }
                commands->EndRenderPass();
            }
            commands->End();
            queue->Submit(*commands);

			// Present results on screen
            context->Present();
		}
	}
	catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
#ifdef _WIN32
        system("pause");
#endif
	}
}

void Window::closeSDL() {
    //Destroy window    
    SDL_DestroyWindow(mSDLWindow);
    mSDLWindow = nullptr;
    
    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}

SDL_Window& Window::getSDLWindow()
{
    return *mSDLWindow;
}

SDL_GLContext& Window::getSDLContext()
{
    return mContext;
}
