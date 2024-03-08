#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>

#include "Graphics/Core/Shader.h"

class PipelineBase
{
public:
	PipelineBase(const LLGL::RenderSystemPtr& renderSystem, const char* vertexFile, const char* fragmentFile);

protected:
	void _InitConstantBuffer(const LLGL::RenderSystemPtr& renderSystem);
	void _InitShader(const LLGL::RenderSystemPtr& renderSystem, const char* vertexFile, const char* fragmentFile);
	void _InitPipeline(const LLGL::RenderSystemPtr& renderSystem);
	void _InitResourceHeap(const LLGL::RenderSystemPtr& renderSystem);

	LLGL::PipelineLayout* mPipelineLayout = nullptr;
	LLGL::PipelineState* mPipeline = nullptr;
	LLGL::ResourceHeap* mResourceHeap = nullptr;
	std::unique_ptr<Shader> mShader = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;

	struct Settings
	{
		// projection-view-model matrix
		glm::mat4 pvmMat = glm::mat4();
		// texture clip to render part of texture
		glm::mat4 textureClip = glm::mat4();
		// model only matrix
		glm::mat4 mMat = glm::mat4();
	}
	settings = {};
};
