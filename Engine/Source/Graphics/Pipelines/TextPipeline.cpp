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
	if (mTexts.empty() || _font.mCharInfo == nullptr)
	{
		return;
	}

	// Rebuild batch if anything changed
	_RebuildBatch();

	if (mBatchVertices.empty())
	{
		return;
	}

	// Check if we need to reallocate buffers
	const size_t requiredVertices = mBatchVertices.size();
	const size_t requiredIndices = mBatchIndices.size();
	const size_t maxVertices = mMaxCharacters * 4;
	const size_t maxIndices = mMaxCharacters * 6;

	if (requiredVertices > maxVertices || requiredIndices > maxIndices)
	{
		// Need more space, reallocate
		mMaxCharacters = std::max(mMaxCharacters * 2, requiredVertices / 4);
		mRenderSystem->Release(*mVertexBuffer);
		mRenderSystem->Release(*mIndexBuffer);
		_AllocateBuffers();
	}

	// Update buffers with batched data
	commandBuffer.UpdateBuffer(
		*mVertexBuffer,
		0,
		mBatchVertices.data(),
		static_cast<uint32_t>(mBatchVertices.size() * sizeof(TextVertex))
	);

	commandBuffer.UpdateBuffer(
		*mIndexBuffer,
		0,
		mBatchIndices.data(),
		static_cast<uint32_t>(mBatchIndices.size() * sizeof(uint32_t))
	);

	// Set up rendering state
	commandBuffer.SetPipelineState(*mPipeline);
	commandBuffer.SetResource(0, mTextureAtlas->GetTextureData());
	commandBuffer.SetResource(1, mTextureAtlas->GetSamplerData());
	commandBuffer.SetVertexBuffer(*mVertexBuffer);
	commandBuffer.SetIndexBuffer(*mIndexBuffer);

	assert(mResourceHeap != nullptr && "No font has been loaded. Call LoadFont in Game init");
	commandBuffer.SetResourceHeap(*mResourceHeap, 0);

	// Use identity transform since we baked positions into vertices
	_UpdateUniforms(commandBuffer, pvMat, glm::vec3(0.0f), glm::vec3(1.0f));

	// Single draw call for all text
	commandBuffer.DrawIndexed(static_cast<uint32_t>(mBatchIndices.size()), 0);
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
}

uint32_t TextPipeline::AddText(const std::string& text, glm::vec2 pos, glm::vec2 scale, glm::vec4 color)
{
	TextEntry entry;
	entry.id = mNextId++;
	entry.text = text;
	entry.position = pos;
	entry.scale = scale;
	entry.color = color;
	entry.visible = true;
	entry.dirty = true;

	mTexts[entry.id] = entry;
	mBatchDirty = true;

	return entry.id;
}

void TextPipeline::UpdateText(uint32_t index, const std::string& text)
{
	auto it = mTexts.find(index);
	if (it != mTexts.end() && it->second.text != text)
	{
		it->second.text = text;
		it->second.dirty = true;
		mBatchDirty = true;
	}
}

void TextPipeline::SetVisible(uint32_t index, bool visible)
{
	auto it = mTexts.find(index);
	if (it != mTexts.end() && it->second.visible != visible)
	{
		it->second.visible = visible;
		mBatchDirty = true;
	}
}

void TextPipeline::SetPosition(uint32_t index, glm::vec2 pos)
{
	auto it = mTexts.find(index);
	if (it != mTexts.end() && it->second.position != pos)
	{
		it->second.position = pos;
		it->second.dirty = true;
		mBatchDirty = true;
	}
}

void TextPipeline::RemoveText(uint32_t index)
{
	if (mTexts.erase(index) > 0)
	{
		mBatchDirty = true;
	}
}

void TextPipeline::Release()
{
	if (mVertexBuffer)
	{
		mRenderSystem->Release(*mVertexBuffer);
		mVertexBuffer = nullptr;
	}
	if (mIndexBuffer)
	{
		mRenderSystem->Release(*mIndexBuffer);
		mIndexBuffer = nullptr;
	}
	mTexts.clear();
	mBatchVertices.clear();
	mBatchIndices.clear();
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

void TextPipeline::_AllocateBuffers()
{
	const size_t maxVertices = mMaxCharacters * 4; // 4 vertices per character
	const size_t maxIndices = mMaxCharacters * 6;  // 6 indices per character

	mVertexBuffer = mRenderSystem->CreateBuffer(
		LLGL::VertexBufferDesc(
			static_cast<uint32_t>(maxVertices * sizeof(TextVertex)),
			mShader->GetVertexFormat()
		)
	);

	mIndexBuffer = mRenderSystem->CreateBuffer(
		LLGL::IndexBufferDesc(
			static_cast<uint32_t>(maxIndices * sizeof(uint32_t)),
			LLGL::Format::R32UInt
		)
	);
}

void TextPipeline::_RebuildBatch()
{
	if (!mBatchDirty) return;

	mBatchVertices.clear();
	mBatchIndices.clear();

	uint32_t currentIndex = 0;

	for (auto& [id, entry] : mTexts)
	{
		if (!entry.visible) continue;

		entry.startIndex = static_cast<uint32_t>(mBatchIndices.size());
		entry.indexCount = 0;

		float offsetX = 0.0f;
		float offsetY = static_cast<float>(_font.mSize) / 2.0f;

		for (const char c : entry.text)
		{
			const auto glyphInfo = _GenerateGlyphInfo(c, offsetX, offsetY);
			offsetX = glyphInfo.mOffsetX;
			offsetY = glyphInfo.mOffsetY;

			// Apply position and scale to each vertex
			for (int i = 0; i < 4; ++i)
			{
				glm::vec2 scaledPos = glyphInfo.mPositions[i] * entry.scale;
				glm::vec2 finalPos = scaledPos + entry.position;

				TextVertex vertex;
				vertex.position = finalPos;
				vertex.texCoord = glyphInfo.mUVs[i];
				vertex.color = entry.color;
				mBatchVertices.push_back(vertex);
			}

			// Add indices for this quad
			mBatchIndices.push_back(currentIndex + 0);
			mBatchIndices.push_back(currentIndex + 1);
			mBatchIndices.push_back(currentIndex + 2);
			mBatchIndices.push_back(currentIndex + 1);
			mBatchIndices.push_back(currentIndex + 2);
			mBatchIndices.push_back(currentIndex + 3);

			currentIndex += 4;
			entry.indexCount += 6;
		}

		entry.dirty = false;
	}

	mBatchDirty = false;
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

TextPipeline::TextPipeline(const LLGL::RenderSystemPtr& renderSystem) : PipelineBase(), mRenderSystem(renderSystem)
{
	// Initialization
	{
		InitConstantBuffer<GUISettings>(renderSystem, guiSettings);
	}

	LLGL::VertexFormat vertexFormat;
	vertexFormat.AppendAttribute({"guiPosition", LLGL::Format::RG32Float});
	vertexFormat.AppendAttribute({"guiTexCoord", LLGL::Format::RG32Float});
	vertexFormat.AppendAttribute({"color", LLGL::Format::RGBA32Float});

	InitShader(renderSystem, vertexFormat, "gui.vert", "gui.frag");

	LLGL::PipelineLayoutDescriptor layoutDesc;
	{
		layoutDesc.heapBindings =
		{
			LLGL::BindingDescriptor{
				"GUISettings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
				LLGL::StageFlags::VertexStage, 0
			},
		};

		layoutDesc.bindings = {
				LLGL::BindingDescriptor{
					"textureAtlas", LLGL::ResourceType::Texture, LLGL::BindFlags::Sampled, LLGL::StageFlags::FragmentStage,
					3
				}
		};
		layoutDesc.staticSamplers = {
			LLGL::StaticSamplerDescriptor{
				"textureAtlasSampler", LLGL::StageFlags::FragmentStage, 3, LLGL::Parse("lod.bias=1")
			}
		};
	}
	InitPipeline(renderSystem, layoutDesc, LLGL::PrimitiveTopology::TriangleList, false);

	const std::vector<LLGL::ResourceViewDescriptor> resourceViews = { mConstantBuffer };
	InitResourceHeap(renderSystem, resourceViews);

	// Pre-allocate buffers for max characters
	_AllocateBuffers();
}
