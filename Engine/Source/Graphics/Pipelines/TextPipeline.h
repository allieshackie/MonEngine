#pragma once
#include "PipelineBase.h"
#include <glm/mat4x4.hpp>
#include "Graphics/Core/Texture.h"

struct GlyphInfo
{
	glm::vec2 mPositions[4];
	glm::vec2 mUVs[4];

	float mOffsetX = 0;
	float mOffsetY = 0;
};

class TextPipeline : public PipelineBase
{
public:
	TextPipeline(const LLGL::RenderSystemPtr& renderSystem);

	void Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat);
	void Release();

	void LoadFont(const char* fontFile);

	uint32_t AddText(const std::string& text, glm::vec2 pos, glm::vec2 scale, glm::vec4 color);
	void UpdateText(uint32_t index, const std::string& text);
	void SetVisible(uint32_t index, bool visible);
	void SetPosition(uint32_t index, glm::vec2 pos);
	void RemoveText(uint32_t index);

private:
	struct TextEntry {
		uint32_t id = 0;
		std::string text;
		glm::vec2 position = { 0,0 };
		glm::vec2 scale = { 0,0 };
		glm::vec4 color = { 0,0,0,0 };
		bool visible = true;
		bool dirty = true;
		uint32_t startIndex = 0;  // Start index in batch
		uint32_t indexCount = 0;  // Number of indices for this text
	};

	void _UpdateUniforms(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, glm::vec3 pos,
	                     glm::vec3 size) const;

	void _AllocateBuffers();
	void _RebuildBatch();
	GlyphInfo _GenerateGlyphInfo(uint32_t character, float offsetX, float offsetY);

	std::unique_ptr<Texture> mTextureAtlas;
	std::unordered_map<uint32_t, TextEntry> mTexts;
	uint32_t mNextId = 0;

	// Single shared buffers
	LLGL::Buffer* mVertexBuffer;
	LLGL::Buffer* mIndexBuffer;
	std::vector<TextVertex> mBatchVertices;
	std::vector<uint32_t> mBatchIndices;
	size_t mMaxCharacters = 10000; // Preallocate

	bool mBatchDirty = true;

	struct GUISettings
	{
		glm::mat4 pvmMat;
	}
	guiSettings = {};

	const LLGL::RenderSystemPtr& mRenderSystem;
};
