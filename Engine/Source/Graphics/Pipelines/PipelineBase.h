#pragma once
#include <LLGL/LLGL.h>
#include <glm/mat4x4.hpp>
#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/Shader.h"

class PipelineBase
{
public:
	PipelineBase() = default;

	LLGL::Buffer* GetConstantBuffer() const { return mConstantBuffer; }
	const Shader& GetShader() const { return *mShader; }

	// Initialization Steps:
	template <typename T>
	void InitConstantBuffer(const LLGL::RenderSystemPtr& renderSystem, T& settings)
	{
		mConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(T)), &settings);
	}

	void InitShader(const LLGL::RenderSystemPtr& renderSystem, const LLGL::VertexFormat& vertexFormat,
	                const char* vertexFile, const char* fragmentFile);
	void InitPipeline(const LLGL::RenderSystemPtr& renderSystem,
	                  const LLGL::PipelineLayoutDescriptor& pipelineLayoutDesc,
	                  LLGL::PrimitiveTopology topology = LLGL::PrimitiveTopology::TriangleStrip,
	                  bool enableDepthTest = true);
	void InitResourceHeap(const LLGL::RenderSystemPtr& renderSystem,
	                      const std::vector<LLGL::ResourceViewDescriptor>& resourceViews);

protected:
	LLGL::PipelineLayout* mPipelineLayout = nullptr;
	LLGL::PipelineState* mPipeline = nullptr;
	LLGL::ResourceHeap* mResourceHeap = nullptr;
	std::unique_ptr<Shader> mShader = nullptr;

	LLGL::Buffer* mConstantBuffer = nullptr;
};
