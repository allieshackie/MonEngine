#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"

class LevelManager;
class Renderer;
class Shader;
class Texture;

struct TextVertex
{
	TextVertex() = default;

	TextVertex(glm::vec2 pos, glm::vec2 uv) : position(pos), texCoord(uv)
	{
	}

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
	glm::vec3 mPosition;
	glm::vec3 mSize;

	LLGL::Buffer* mVertexBuffer = nullptr;
	LLGL::Buffer* mIndexBuffer = nullptr;

	uint32_t mIndexCount = 0;
};


class TextPipeline
{
public:
	TextPipeline(Renderer& renderer, LevelManager& levelManager, std::string shadersFolderPath,
	             std::string fontFolderPath);
	~TextPipeline() = default;

	void Render() const;

	void LoadFont(const char* fontPath);

	void CreateTextMesh(const std::string& text, glm::vec2 pos, glm::vec2 size);

private:
	void _InitPipeline();
	void _CreateResourceHeap();

	void _UpdateUniforms(glm::vec3 pos, glm::vec3 size) const;
	GlyphInfo _GenerateGlyphInfo(uint32_t character, float offsetX, float offsetY);

	std::vector<uint8_t> _ReadFontFromFileTTF(const char* fontPath) const;

	std::shared_ptr<Texture> mTextureAtlas;

	LLGL::PipelineLayout* mPipelineLayout = nullptr;
	LLGL::PipelineState* mPipeline = nullptr;
	LLGL::ResourceHeap* mResourceHeap = nullptr;
	std::unique_ptr<Shader> mShader = nullptr;

	struct GUISettings
	{
		glm::mat4 pvmMat;
	}
	guiSettings = {};

	LLGL::Buffer* mConstantBuffer = nullptr;
	std::vector<std::unique_ptr<TextMesh>> mTextMeshes;

	Renderer& mRenderer;
	LevelManager& mLevelManager;
	std::string mShadersFolderPath;
	std::string mFontFolderPath;
};
