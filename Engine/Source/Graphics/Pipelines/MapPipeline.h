#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "PipelineBase.h"
#include "Graphics/Core/Vertex.h"

class Camera;
class MeshPipeline;

struct EntityId;
struct MapComponent;
struct TransformComponent;

class EntityRegistry;

class MapPipeline : public PipelineBase
{
public:
	MapPipeline(const LLGL::RenderSystemPtr& renderSystem, LLGL::Buffer* meshConstantBuffer);

	void Render(LLGL::CommandBuffer& commandBuffer, const Camera& camera,
	            const glm::mat4 projection, EntityRegistry& entityRegistry,
	            MeshPipeline& meshPipeline) const;

	void GenerateMapTexture(const LLGL::RenderSystemPtr& renderSystem, LLGL::CommandBuffer& commandBuffer,
	                        EntityRegistry& entityRegistry, EntityId mapId);

private:
	void _CreateMapResourceHeap(const LLGL::RenderSystemPtr& renderSystem);

	void _UpdateUniformsModel(LLGL::CommandBuffer& commandBuffer, glm::vec3 pos, glm::vec3 size, glm::vec3 rot,
	                          glm::vec4 texClip);

	void _WriteMapTexture(LLGL::CommandBuffer& commandBuffer, LLGL::PipelineState* writePipeline,
	                      LLGL::RenderTarget* writeTarget, LLGL::Texture* writtenTexture,
	                      EntityRegistry& entityRegistry, EntityId mapId);

	void _CalculateTileDrawData(const MapComponent& mapComponent, int tileIndex, glm::vec3& pos, glm::vec3& size,
	                            glm::vec4& clip) const;
	glm::vec4 _GetClipForTile(const MapComponent& mapComponent, int index) const;


	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::Sampler* mSampler = nullptr;

	std::vector<Vertex> mVertices = {
		{{-0.5, -0.5, 1}, {1, 1, 1}, {0, 1}}, // top left
		{{-0.5, 0.5, 1}, {1, 1, 1}, {0, 0}}, // bottom left
		{{0.5, -0.5, 1}, {1, 1, 1}, {1, 1}}, // top right
		{{0.5, 0.5, 1}, {1, 1, 1}, {1, 0}}, // bottom right
	};

	// texture id, texture ptr
	std::vector<LLGL::Texture*> mGeneratedTextures;

	const LLGL::Extent2D mTextureSize = {512, 512};

	LLGL::ResourceHeap* mMapMeshResourceHeap = nullptr;
	LLGL::Buffer* mMeshConstantBuffer = nullptr;
};
