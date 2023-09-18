#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class Shader;
class Texture;

struct TextVertex
{
	glm::vec2 position = {0, 0};
	glm::vec2 texCoord = {0, 0};
};

struct GlyphInfo
{
	glm::vec2 mPositions[4];
	glm::vec2 mUVs[4];

	float mOffsetX = 0;
	float mOffsetY = 0;
};

struct TextMesh
{
	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mSize = {0, 0, 0};

	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::Buffer* mIndexBuffer = nullptr;

	uint32_t mIndexCount = 0;
};


class TextPipeline
{
public:
	TextPipeline() = default;
	~TextPipeline() = default;

	void Init(std::shared_ptr<LLGL::RenderSystem>& renderSystem);
	void Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat) const;

	void LoadFont(const std::shared_ptr<LLGL::RenderSystem>& renderSystem, const char* fontFile);
	void CreateTextMesh(std::shared_ptr<LLGL::RenderSystem>& renderSystem, const std::string& text, glm::vec2 pos,
	                    glm::vec2 size);

private:
	void _CreateResourceHeap(const std::shared_ptr<LLGL::RenderSystem>& renderSystem);
	void _UpdateUniforms(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, glm::vec3 pos,
	                     glm::vec3 size) const;

	GlyphInfo _GenerateGlyphInfo(uint32_t character, float offsetX, float offsetY);

	std::shared_ptr<Texture> mTextureAtlas;

	LLGL::ResourceHeap* mResourceHeap = nullptr;
	LLGL::PipelineLayout* mPipelineLayout = nullptr;
	LLGL::PipelineState* mPipeline = nullptr;
	std::unique_ptr<Shader> mShader = nullptr;

	struct GUISettings
	{
		glm::mat4 pvmMat;
	}
	guiSettings = {};

	LLGL::Buffer* mConstantBuffer = nullptr;
	std::vector<std::unique_ptr<TextMesh>> mTextMeshes;
};
