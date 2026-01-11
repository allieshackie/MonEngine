#include "Core/World.h"

#include "TerrainPipeline.h"

TerrainPipeline::TerrainPipeline(const LLGL::RenderSystemPtr& renderSystem, const ResourceManager& resourceManager, std::weak_ptr<World> world)
	: PipelineBase(), mRenderSystem(renderSystem), mResourceManager(resourceManager)
{
	// Initialization
	{
		InitConstantBuffer<FrameSettings>(renderSystem, frameSettings);

		LLGL::VertexFormat vertexFormat;
		vertexFormat.AppendAttribute({ "position", LLGL::Format::RGB32Float, 0, 0, sizeof(StaticVertex), 0 });
		vertexFormat.AppendAttribute({ "normal", LLGL::Format::RGB32Float, 1, 12, sizeof(StaticVertex), 0 });
		vertexFormat.AppendAttribute({ "texCoord", LLGL::Format::RG32Float, 2, 24, sizeof(StaticVertex), 0 });

		InitShader(renderSystem, vertexFormat, "terrain.vert", "terrain.frag");
		LLGL::PipelineLayoutDescriptor layoutDesc;
		{
			layoutDesc.heapBindings =
			{
				LLGL::BindingDescriptor{
					"FrameSettings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
					LLGL::StageFlags::VertexStage | LLGL::StageFlags::FragmentStage, 1
				}
			};
		}
		InitPipeline(renderSystem, layoutDesc, LLGL::PrimitiveTopology::TriangleList);

		const std::vector<LLGL::ResourceViewDescriptor> resourceViews = { mConstantBuffer };
		InitResourceHeap(renderSystem, resourceViews);
	}
}

void TerrainPipeline::Render(LLGL::CommandBuffer& commands, const glm::mat4 projection, std::weak_ptr<World> world)
{
	auto worldPtr = world.lock();
	if (worldPtr == nullptr || worldPtr->GetTerrain() == nullptr)
	{
		return;
	}

	if (!worldPtr->GetTerrain()->Initialized())
	{
		worldPtr->GetTerrain()->InitializeBuffers(mRenderSystem, *mShader);
	}

	commands.SetPipelineState(*mPipeline);
	commands.SetResourceHeap(*mResourceHeap);

	frameSettings.view = worldPtr->GetCamera().GetView();
	frameSettings.projection = projection;
	commands.UpdateBuffer(*mConstantBuffer, 0, &frameSettings, sizeof(frameSettings));
}
