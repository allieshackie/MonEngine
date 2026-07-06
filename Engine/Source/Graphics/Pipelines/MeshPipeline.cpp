#include <filesystem>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "LLGL/Utils/Parse.h"

#include "Core/Camera.h"
#include "Core/World.h"
#include "Entity/Entity.h"
#include "Entity/Components/ModelComponent.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Core/Node.h"

#include "MeshPipeline.h"

void MeshPipeline::Render(LLGL::CommandBuffer& commands, const glm::mat4 projection, std::weak_ptr<World> world)
{
	auto worldPtr = world.lock();
	if (worldPtr == nullptr)
	{
		return;
	}

	_ProcessLights();
	
	commands.SetPipelineState(*mPipeline);
	commands.SetResourceHeap(*mResourceHeap);

	frameSettings.view = worldPtr->GetCamera().GetView();
	frameSettings.projection = projection;
	commands.UpdateBuffer(*mFrameBuffer, 0, &frameSettings, sizeof(frameSettings));

	const auto meshView = worldPtr->GetRegistry().view<const TransformComponent, const ModelComponent>();
	meshView.each([this, &commands, &worldPtr](
		const TransformComponent& transform, const ModelComponent& modelComponent)
		{
			const auto& model = mResourceManager.GetModelFromId(modelComponent.mModelPath);

			_RenderNode(commands, model, model.GetRootSceneIndex(), transform, modelComponent, worldPtr);
		});
}

void MeshPipeline::_RenderNode(LLGL::CommandBuffer& commands, const Model& model, int nodeIndex,
                               const TransformComponent& transform, const ModelComponent& modelComponent,
                               std::shared_ptr<World> world)
{
	const auto node = model.GetNodeAt(nodeIndex);

	if (const auto meshData = model.GetMeshAt(node->mMeshIndex); meshData != nullptr)
	{
		glm::mat4 modelTransform = transform.GetMatrix(model.CalculateModelScaling(transform.mSize));

		int lightsSize = static_cast<int>(mLights.size());
		if (lightsSize != lightSettings.numLights || mUpdateLights)
		{
			lightSettings.numLights = lightsSize;
			mUpdateLights = false;
			UpdateLightBuffer();
		}
		if (lightSettings.viewPos != world->GetCamera().GetPosition())
		{
			lightSettings.viewPos = world->GetCamera().GetPosition();
		}

		if (mCurrentMaterial.emission != startingMaterial.emission || mCurrentMaterial.params1 != startingMaterial.params1 || mCurrentMaterial.params2 != startingMaterial.params2)
		{
			startingMaterial = mCurrentMaterial;
			UpdateMaterialBuffer();
		}

		meshSettings.model = modelTransform;
		std::uint64_t transformOffset = 0;
		for (const auto& finalTransform : modelComponent.mFinalTransforms)
		{
			mRenderSystem->WriteBuffer(*mBoneBuffer, transformOffset, &(finalTransform), sizeof(glm::mat4));
			transformOffset += sizeof(glm::mat4);
		}

		commands.UpdateBuffer(*mLightConstantBuffer, 0, &lightSettings, sizeof(lightSettings));

		// x = hasTexture, y = hasBones, z = gTargetBone
		meshSettings.params.x = meshData->mTextureId != -1;
		meshSettings.params.y = model.GetNumJoints() > 1;
		meshSettings.params.z = static_cast<float>(modelComponent.mCurrentBoneIndex);
		commands.UpdateBuffer(*mConstantBuffer, 0, &meshSettings, sizeof(meshSettings));
		auto& texture = mResourceManager.GetTexture(meshData->mTextureId);
		auto& sampler = mResourceManager.GetSampler(meshData->mTextureId);
		commands.SetResource(0, texture);
		commands.SetResource(1, sampler);
		commands.SetVertexBuffer(*meshData->mVertexBuffer);
		commands.SetIndexBuffer(*meshData->mIndexBuffer);
		commands.DrawIndexed(meshData->mNumIndices, 0);
	}

	for (const auto& child : node->mChildren)
	{
		_RenderNode(commands, model, child, transform, modelComponent, world);
	}
}

void MeshPipeline::SetPipeline(LLGL::CommandBuffer& commands) const
{
	commands.SetPipelineState(*mPipeline);
}

MeshPipeline::MeshPipeline(const LLGL::RenderSystemPtr& renderSystem, const ResourceManager& resourceManager, EventPublisher& eventPublisher)
	: PipelineBase(), mRenderSystem(renderSystem), mResourceManager(resourceManager)
{
	// Initialization
	{
		InitConstantBuffer<MeshSettings>(renderSystem, meshSettings);
		mFrameBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(FrameSettings)), &frameSettings);
		mLightConstantBuffer = renderSystem->CreateBuffer(LLGL::ConstantBufferDesc(sizeof(LightSettings)),
		                                                  &lightSettings);

		LLGL::VertexFormat vertexFormat;
		vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float, 0, 0, sizeof(Vertex), 0});
		vertexFormat.AppendAttribute({"normal", LLGL::Format::RGB32Float, 1, 12, sizeof(Vertex), 0});
		vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float, 2, 24, sizeof(Vertex), 0});
		vertexFormat.AppendAttribute({"boneIds", LLGL::Format::RGBA32SInt, 3, 32, sizeof(Vertex), 0});
		vertexFormat.AppendAttribute({"weights", LLGL::Format::RGBA32Float, 4, 48, sizeof(Vertex), 0});

		InitShader(renderSystem, vertexFormat, "mesh.vert", "mesh.frag");
		LLGL::PipelineLayoutDescriptor layoutDesc;
		{
			layoutDesc.heapBindings =
			{
				LLGL::BindingDescriptor{
					"FrameSettings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
					LLGL::StageFlags::VertexStage, 0
				},
				LLGL::BindingDescriptor{
					"MeshSettings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
					LLGL::StageFlags::VertexStage | LLGL::StageFlags::FragmentStage, 1
				},
				LLGL::BindingDescriptor{
					"LightSettings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
					LLGL::StageFlags::FragmentStage, 2
				},
				LLGL::BindingDescriptor{
					"BoneBuffer", LLGL::ResourceType::Buffer, LLGL::BindFlags::Storage,
					LLGL::StageFlags::VertexStage, 3
				},
				LLGL::BindingDescriptor{
					"LightBuffer", LLGL::ResourceType::Buffer, LLGL::BindFlags::Storage,
					LLGL::StageFlags::FragmentStage, 4
				},
				LLGL::BindingDescriptor{
					"MaterialBuffer", LLGL::ResourceType::Buffer, LLGL::BindFlags::Storage,
					LLGL::StageFlags::FragmentStage, 5
				},
			};
			layoutDesc.bindings = {
				LLGL::BindingDescriptor{
					"colorMap", LLGL::ResourceType::Texture, LLGL::BindFlags::Sampled, LLGL::StageFlags::FragmentStage,
					3
				}
			};
			layoutDesc.staticSamplers = {
				LLGL::StaticSamplerDescriptor{
					"colorMapSampler", LLGL::StageFlags::FragmentStage, 3, LLGL::Parse("lod.bias=1")
				}
			};
		}
		InitPipeline(renderSystem, layoutDesc, LLGL::PrimitiveTopology::TriangleList);

		// Resource Heap
		// Create light buffer
		LLGL::BufferDescriptor lightBufferDesc;
		{
			// Need to define max lights so that buffer is created with appropriate space
			lightBufferDesc.size = sizeof(LightUniform) * MAX_LIGHTS;
			lightBufferDesc.stride = sizeof(LightUniform);
			lightBufferDesc.bindFlags = LLGL::BindFlags::Storage;
			lightBufferDesc.cpuAccessFlags = LLGL::CPUAccessFlags::Write;
		}
		mLightBuffer = renderSystem->CreateBuffer(lightBufferDesc);

		LLGL::BufferDescriptor materialBufferDesc;
		{
			materialBufferDesc.size = sizeof(Material);
			materialBufferDesc.stride = sizeof(Material);
			materialBufferDesc.bindFlags = LLGL::BindFlags::Storage;
			materialBufferDesc.cpuAccessFlags = LLGL::CPUAccessFlags::Write;
		}
		mMaterialBuffer = renderSystem->CreateBuffer(materialBufferDesc);

		LLGL::BufferDescriptor boneBufferDesc;
		{
			boneBufferDesc.size = sizeof(glm::mat4) * MAX_BONES;
			boneBufferDesc.stride = sizeof(glm::mat4);
			boneBufferDesc.bindFlags = LLGL::BindFlags::Storage;
			boneBufferDesc.cpuAccessFlags = LLGL::CPUAccessFlags::Write;
		}
		mBoneBuffer = renderSystem->CreateBuffer(boneBufferDesc);

		renderSystem->WriteBuffer(*mMaterialBuffer, 0, &(mCurrentMaterial), sizeof(Material));

		const std::vector<LLGL::ResourceViewDescriptor> resourceViews = {
			mFrameBuffer, mConstantBuffer, mLightConstantBuffer, mBoneBuffer, mLightBuffer, mMaterialBuffer
		};
		InitResourceHeap(renderSystem, resourceViews);
	}

	eventPublisher.AddWorldCreatedListener(
		[this](std::weak_ptr<World> world) {
			mLightEntities.clear();
			if (const auto worldShared = world.lock())
			{
				World* worldPtr = worldShared.get();
				EventFunc func = [this, worldPtr](entt::entity entityId)
				{
					if (Entity* entity = worldPtr->GetEntityForId(entityId))
					{
						AddLight(entity);
					}
				};
				worldShared->ConnectOnConstruct<TransformComponent>(func);

				EventFunc removeFunc = [this, worldPtr](entt::entity entityId)
				{
					if (Entity* entity = worldPtr->GetEntityForId(entityId))
					{
						RemoveLight(entity);
					}
				};
				worldShared->ConnectOnDestroy<LightComponent>(removeFunc);
			}
		});
}

void MeshPipeline::SetResourceHeapTexture(LLGL::CommandBuffer& commands, LLGL::Texture& texture) const
{
	commands.SetResourceHeap(*mResourceHeap);
	commands.SetResource(0, texture);
}

Material& MeshPipeline::GetMaterial()
{
	return mCurrentMaterial;
}

void MeshPipeline::UpdateMaterialBuffer() const
{
	mRenderSystem->WriteBuffer(*mMaterialBuffer, 0, &(mCurrentMaterial), sizeof(Material));
}

void MeshPipeline::AddLight(Entity* entity)
{
	const auto light = entity->TryGetComponent<LightComponent>();
	if (light != nullptr)
	{
		mQueuedLightEntities.push_back(entity);
	}
}

void MeshPipeline::RemoveLight(Entity* entity)
{
	auto it = std::find(mLightEntities.begin(), mLightEntities.end(), entity);
	if (it != mLightEntities.end())
	{
		mLightEntities.erase(it);
		mLightsDirty = true;
	}
}

void MeshPipeline::UpdateLightBuffer()
{
	std::uint64_t lightOffset = 0;
	for (const auto& light : mLights)
	{
		mRenderSystem->WriteBuffer(*mLightBuffer, lightOffset, &(light), sizeof(LightUniform));
		lightOffset += sizeof(LightUniform);
	}
}

void MeshPipeline::_RebuildLights()
{
	mLights.clear();
	for (const auto& entity : mLightEntities)
	{
		const auto& light = entity->GetComponent<LightComponent>();
		const auto transform = entity->TryGetComponent<TransformComponent>();
		if (transform != nullptr)
		{
			mLights.push_back({ light.mColor, glm::vec4(transform->mPosition, 1.0),
				{light.mIntensity, static_cast<int>(light.mLightType), 0, 0} });
		}
	}
}

void MeshPipeline::_ProcessLights()
{
	for (auto it = mQueuedLightEntities.begin(); it != mQueuedLightEntities.end();)
	{
		const auto transform = (*it)->TryGetComponent<TransformComponent>();
		if (transform != nullptr)
		{
			mLightEntities.push_back(*it);
			it = mQueuedLightEntities.erase(it);
			mLightsDirty = true;
		}
		else
		{
			++it;
		}
	}

	if (mLightsDirty)
	{
		_RebuildLights();
		UpdateLightBuffer();
		mLightsDirty = false;
	}
}
