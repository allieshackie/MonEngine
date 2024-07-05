#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "Graphics/Core/Shader.h"

class PipelineBase
{
public:
	PipelineBase(const LLGL::RenderSystemPtr& renderSystem, const char* vertexFile, const char* fragmentFile,
	             LLGL::PrimitiveTopology topology = LLGL::PrimitiveTopology::TriangleStrip);

	LLGL::PipelineLayout& GetPipelineLayout() const { return *mPipelineLayout; }
	LLGL::Buffer* GetConstantBuffer() const { return mConstantBuffer; }

protected:
	void _InitConstantBuffer(const LLGL::RenderSystemPtr& renderSystem);
	void _InitShader(const LLGL::RenderSystemPtr& renderSystem, const char* vertexFile, const char* fragmentFile);
	void _InitPipeline(const LLGL::RenderSystemPtr& renderSystem, LLGL::PrimitiveTopology topology);
	void _InitResourceHeap(const LLGL::RenderSystemPtr& renderSystem);

	LLGL::PipelineLayout* mPipelineLayout = nullptr;
	LLGL::PipelineState* mPipeline = nullptr;
	LLGL::ResourceHeap* mResourceHeap = nullptr;
	std::unique_ptr<Shader> mShader = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;

	struct Settings
	{
		// projection-view-model matrix
		glm::mat4 model = glm::mat4();
		glm::mat4 view = glm::mat4();
		glm::mat4 projection = glm::mat4();
		// texture clip to render part of texture
		glm::mat4 textureClip = glm::mat4();

		glm::vec3 lightPos = {0, 0, 0};
		float padding1 = 0.0f; // Padding for alignment
		glm::vec3 viewPos = {0, 0, 0};
		float padding2 = 0.0f; // Padding for alignment
		glm::vec3 lightColor = {1.0, 1.0, 1.0};
		float padding3 = 0.0f; // Padding for alignment
	}
	settings = {};
};
