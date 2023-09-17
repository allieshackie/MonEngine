#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"

class Map;

class MapPipeline
{
public:
	MapPipeline() = default;

	~MapPipeline()
	{
		std::cout << "Delete MapPipeline" << std::endl;
	}

	void Init(std::shared_ptr<LLGL::RenderSystem>& renderSystem);
	void Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, const std::shared_ptr<Map>& map) const;

	void RenderMapTexture(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat,
	                      const std::shared_ptr<Map>& map) const;
	void QueueWriteMapTexture(const std::shared_ptr<Map>& map);
	void WriteQueuedMapTextures(const std::shared_ptr<LLGL::RenderSystem>& renderSystem,
	                            LLGL::CommandBuffer& commandBuffer);

private:
	void _CreateResourceHeap(const std::shared_ptr<LLGL::RenderSystem>& renderSystem);

	void _Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, const std::shared_ptr<Map>& map) const;
	void _UpdateUniforms(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, glm::vec3 pos, glm::vec3 size,
	                     glm::vec3 rot) const;
	void _UpdateUniformsModel(LLGL::CommandBuffer& commandBuffer, glm::vec3 pos, glm::vec3 size, glm::vec3 rot,
	                          glm::vec4 texClip) const;

	void _InitMapTexturePipeline(LLGL::CommandBuffer& commandBuffer,
	                             const std::shared_ptr<LLGL::RenderSystem>& renderSystem,
	                             std::shared_ptr<Map>& map);
	void _WriteMapTexture(LLGL::CommandBuffer& commandBuffer, const std::shared_ptr<Map>& map,
	                      LLGL::PipelineState* writePipeline, LLGL::RenderTarget* writeTarget,
	                      LLGL::Texture* writeableTexture) const;

	LLGL::PipelineLayout* mPipelineLayout = nullptr;
	LLGL::PipelineState* mPipeline = nullptr;
	LLGL::ResourceHeap* mResourceHeap = nullptr;

	std::unique_ptr<Shader> mShader = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;
	LLGL::Buffer* mVertexBuffer = nullptr;

	struct SpriteSettings
	{
		glm::mat4 pvmMat;
		glm::mat4 textureTransform;
	}
	spriteSettings = {};

	std::vector<Vertex> mVertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 1}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 0}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 1}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 0}}, // bottom right
	};


	// Write Map Textures
	std::vector<std::shared_ptr<Map>> mQueuedMaps = {};

	std::unordered_map<int, LLGL::Texture*> mMapTextures;
	const LLGL::Extent2D mTextureSize = {512, 512};
};
