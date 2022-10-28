#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "Camera.h"
#include "DebugDraw.h"
#include "Shader.h"
#include "Vertex.h"

class Renderer
{
public:
	Renderer();
	~Renderer();

	static Renderer* GetInstance();

	void OnDrawFrame(const std::function<void()>& drawCallback) const;

	LLGL::RenderContext& GetContext() const
	{
		return *mContext;
	}

	const std::unique_ptr<LLGL::RenderSystem>& GetRendererSystem() const
	{
		return mRenderer;
	}

	void SetTexture(int textureId) const;

	void UpdateProjectionViewModelUniform(glm::mat4 model);
	void UpdateTextureClipUniform(glm::mat4 textureClip);
	void UpdateProjection();
	void UpdateView(glm::mat4 view);

	void AddDebugDrawToVB(DebugDrawable* debug);
	void ClearDebugDraw();

	void DrawSprite() const;

private:
	void _Init();
	void _InitSpritePipeline();
	void _InitVolumePipeline();
	void _InitDebugDrawPipeline();

	void _DrawSprites() const;
	void _DrawVolumes();
	void _DrawDebug() const;

	void _AddDebugLineToVB(const Line* debug, std::vector<DebugVertex>& vertices);
	void _AddDebugBoxToVB(const Box* debug, std::vector<DebugVertex>& vertices);

	std::unique_ptr<LLGL::RenderSystem> mRenderer; // Render system
	LLGL::RenderContext* mContext = nullptr; // Main render context
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue

	// Pipelines
	LLGL::PipelineState* mSpritePipeline = nullptr;
	LLGL::PipelineState* mVolumePipeline = nullptr;

	LLGL::PipelineState* mDebugDrawPipeline = nullptr;

	// Shaders
	Shader* mSpriteShader = nullptr;
	Shader* mVolumeShader = nullptr;
	Shader* mDebugShader = nullptr;

	// Vertex Data
	LLGL::Buffer* mSpriteConstantBuffer = nullptr;
	LLGL::Buffer* mVolumeConstantBuffer = nullptr;

	LLGL::Buffer* mSpriteVertexBuffer = nullptr;
	LLGL::Buffer* mVolumeVertexBuffer = nullptr;
	LLGL::ResourceHeap* testVolumeResourceHeap = nullptr;

	template <typename T>
	struct VBData
	{
		LLGL::Buffer* mVertexBuffer;
		std::vector<T> mVertices;
	};

	std::vector<VBData<DebugVertex>> mDebugVertexBuffers;

	struct SpriteSettings
	{
		glm::mat4 pvmMat;
		glm::mat4 textureClip;
	}
	spriteSettings = {};

	struct VolumeSettings
	{
		glm::mat4 pvmMat;
		LLGL::ColorRGBAf color;
	}
	volumeSettings = {};

	const std::vector<Vertex> mSpriteVertices = {
		{{-1, 1, 1}, {1, 0}}, // top left
		{{-1, -1, 1}, {1, 1}}, // bottom left
		{{1, 1, 1}, {0, 0}}, // top right
		{{1, -1, 1}, {0, 1}}, // bottom right
	};

	uint32_t mNumVertices = 0;

	glm::mat4 mProjection = glm::identity<glm::mat4>();
	glm::mat4 mView = glm::identity<glm::mat4>();

	Model model0;

	static Renderer* mInstance;
};
