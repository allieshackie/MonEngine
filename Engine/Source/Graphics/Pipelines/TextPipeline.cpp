#ifndef STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#include "Graphics/Util/stb_truetype.h"
#endif

#include <stb_image_write.h>

#include <imgui.h>
#include <iomanip>
#include <LLGL/LLGL.h>
#include "LLGL/Utils/Parse.h"
#include "LLGL/Utils/Utility.h"

#include "Core/Camera.h"
#include "Core/SceneManager.h"
#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Texture.h"
#include "Util/FileSystem.h"

#include "TextPipeline.h"

struct FontData
{
	const uint32_t mSize = 40;
	const uint32_t mAtlasWidth = 1024;
	const uint32_t mAtlasHeight = 1024;
	const uint32_t mFontOverSampleX = 2;
	const uint32_t mFontOverSampleY = 2;
	const uint32_t mFirstChar = ' ';
	const uint32_t mCharCount = '~' - ' ';
	std::unique_ptr<stbtt_packedchar[]> mCharInfo;
} _font;

void TextPipeline::Render(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat)
{
	for (const auto& mesh : mTextMeshes)
	{
		// set graphics pipeline
		commandBuffer.SetPipelineState(*mPipeline);
		commandBuffer.SetVertexBuffer(*mesh->mVertexBuffer);
		commandBuffer.SetIndexBuffer(*mesh->mIndexBuffer);

		assert(mResourceHeap != nullptr && "No font has been loaded.  Call EngineContext::LoadFont in Game init");
		commandBuffer.SetResourceHeap(*mResourceHeap, 0);

		_UpdateUniforms(commandBuffer, pvMat, mesh->mPosition, mesh->mSize);

		commandBuffer.DrawIndexed(mesh->mIndexCount, 0);
	}
}

void TextPipeline::Release()
{
	for (const auto& mesh : mTextMeshes)
	{
		mRenderSystem->Release(*mesh->mVertexBuffer);
		mRenderSystem->Release(*mesh->mIndexBuffer);
	}
	mTextMeshes.clear();
}

void TextPipeline::_CreateResourceHeap()
{
	std::vector<LLGL::ResourceViewDescriptor> resourceViews;
	resourceViews.reserve(3);

	resourceViews.emplace_back(mConstantBuffer);
	resourceViews.emplace_back(&mTextureAtlas->GetTextureData());
	resourceViews.emplace_back(&mTextureAtlas->GetSamplerData());

	mResourceHeap = mRenderSystem->CreateResourceHeap(mPipelineLayout, resourceViews);
}

// Orthographic Projection
void TextPipeline::_UpdateUniforms(LLGL::CommandBuffer& commandBuffer, const glm::mat4 pvMat, glm::vec3 pos,
                                   glm::vec3 size) const
{
	// Update
	auto model = glm::mat4(1.0f);
	model = translate(model, pos);
	model = scale(model, size);

	const GUISettings settings = {pvMat * model};
	commandBuffer.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void TextPipeline::LoadFont(const char* fontFile)
{
	const std::string fontPath = "Fonts/";
	std::string fullPath = ASSETS_FOLDER + fontPath;
	fullPath.append(fontFile);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF(fullPath.c_str(), 25.0f);

	auto fontData = FileSystem::ReadBytes(fullPath);
	std::vector<uint8_t> atlasData(_font.mAtlasWidth * _font.mAtlasHeight);

	_font.mCharInfo = std::make_unique<stbtt_packedchar[]>(_font.mCharCount);

	stbtt_pack_context context;
	if (!stbtt_PackBegin(&context, atlasData.data(), _font.mAtlasWidth, _font.mAtlasHeight, 0, 1, nullptr))
	{
		// Failed to initialize font
		assert(false);
		return;
	}

	stbtt_PackSetOversampling(&context, _font.mFontOverSampleX, _font.mFontOverSampleY);
	if (!stbtt_PackFontRange(&context, fontData.data(), 0, _font.mSize, _font.mFirstChar, _font.mCharCount,
	                         _font.mCharInfo.get()))
	{
		// Failed to pack font
		assert(false);
		return;
	}

	stbtt_PackEnd(&context);

	mTextureAtlas = std::make_unique<Texture>(mRenderSystem, atlasData.data(), _font.mAtlasWidth,
	                                          _font.mAtlasHeight, true);

	_CreateResourceHeap();
}

std::shared_ptr<TextMesh> TextPipeline::CreateTextMesh(const std::string& text, glm::vec2 pos, glm::vec2 size, glm::vec4 color)
{
	auto textMesh = std::make_shared<TextMesh>();
	textMesh->mPosition = {pos, 1.0f};
	textMesh->mSize = {size, 1.0f};

	std::vector<TextVertex> vertices;
	std::vector<uint32_t> indices;

	if (_font.mCharInfo)
	{
		uint32_t lastIndex = 0;
		float offsetX = 0, offsetY = static_cast<float>(_font.mSize) / 2.0f;
		for (const char c : text)
		{
			const auto glyphInfo = _GenerateGlyphInfo(c, offsetX, offsetY);
			offsetX = glyphInfo.mOffsetX;
			offsetY = glyphInfo.mOffsetY;
			TextVertex vertex1 = {glyphInfo.mPositions[0], glyphInfo.mUVs[0], color};
			TextVertex vertex2 = {glyphInfo.mPositions[1], glyphInfo.mUVs[1], color};
			TextVertex vertex3 = {glyphInfo.mPositions[2], glyphInfo.mUVs[2], color};
			TextVertex vertex4 = {glyphInfo.mPositions[3], glyphInfo.mUVs[3], color};
			vertices.emplace_back(vertex1);
			vertices.emplace_back(vertex2);
			vertices.emplace_back(vertex3);
			vertices.emplace_back(vertex4);

			indices.push_back(lastIndex);
			indices.push_back(lastIndex + 1);
			indices.push_back(lastIndex + 2);

			indices.push_back(lastIndex + 1);
			indices.push_back(lastIndex + 2);
			indices.push_back(lastIndex + 3);

			lastIndex += 4;
		}
	}

	textMesh->mVertexBuffer = mRenderSystem->CreateBuffer(
		LLGL::VertexBufferDesc(static_cast<std::uint32_t>(vertices.size() * sizeof(TextVertex)),
		                       mShader->GetVertexFormat()),
		vertices.data()
	);

	textMesh->mIndexCount = static_cast<std::uint32_t>(indices.size());
	textMesh->mIndexBuffer = mRenderSystem->CreateBuffer(
		LLGL::IndexBufferDesc(static_cast<std::uint32_t>(indices.size() * sizeof(uint32_t)), LLGL::Format::R32UInt),
		indices.data()
	);

	mTextMeshes.push_back(std::move(textMesh));

	return textMesh;
}

GlyphInfo TextPipeline::_GenerateGlyphInfo(uint32_t character, float offsetX, float offsetY)
{
	stbtt_aligned_quad quad;

	stbtt_GetPackedQuad(_font.mCharInfo.get(), _font.mAtlasWidth, _font.mAtlasHeight,
	                    character - _font.mFirstChar, &offsetX, &offsetY, &quad, 1);
	const auto xMin = quad.x0;
	const auto xMax = quad.x1;
	const auto yMin = quad.y1;
	const auto yMax = quad.y0;

	GlyphInfo info{};
	info.mOffsetX = offsetX;
	info.mOffsetY = offsetY;

	info.mPositions[0] = {xMin, yMin};
	info.mPositions[1] = {xMin, yMax};
	info.mPositions[2] = {xMax, yMin};
	info.mPositions[3] = {xMax, yMax};

	info.mUVs[0] = {quad.s0, quad.t1};
	info.mUVs[1] = {quad.s0, quad.t0};
	info.mUVs[2] = {quad.s1, quad.t1};
	info.mUVs[3] = {quad.s1, quad.t0};

	return info;
}

TextPipeline::TextPipeline(const LLGL::RenderSystemPtr& renderSystem) : mRenderSystem(renderSystem)
{
	mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(GUISettings)),
	                                             &guiSettings);

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"guiPosition", LLGL::Format::RG32Float});
	vertexFormat.AppendAttribute({"guiTexCoord", LLGL::Format::RG32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGBA32Float});

	std::string vertPath = SHADERS_FOLDER;
	vertPath.append("gui.vert");

	std::string fragPath = SHADERS_FOLDER;
	fragPath.append("gui.frag");

	mShader = std::make_unique<Shader>(*renderSystem, vertexFormat, vertPath.c_str(),
	                                   fragPath.c_str());

	// All layout bindings that will be used by graphics and compute pipelines
	// Create pipeline layout
	mPipelineLayout = renderSystem->CreatePipelineLayout(
		LLGL::Parse("heap{cbuffer(0):vert:frag, texture(0):frag, sampler(0):frag}"));

	// Create graphics pipeline
	LLGL::GraphicsPipelineDescriptor pipelineDesc;
	{
		pipelineDesc.vertexShader = &mShader->GetVertexShader();
		pipelineDesc.fragmentShader = &mShader->GetFragmentShader();
		pipelineDesc.pipelineLayout = mPipelineLayout;
		pipelineDesc.primitiveTopology = LLGL::PrimitiveTopology::TriangleList;
		pipelineDesc.blend.targets[0].blendEnabled = true;
	}
	mPipeline = renderSystem->CreatePipelineState(pipelineDesc);
}
