#include <filesystem>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "LLGL/Utils/Parse.h"

#include "Core/Camera.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/SpriteComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "MeshPipeline.h"

MeshPipeline::MeshPipeline(LLGL::RenderSystemPtr& renderSystem, EntityRegistry& entityRegistry,
                           const ResourceManager& resourceManager) : PipelineBase()
{
	// Initialization
	{
		InitConstantBuffer<MeshSettings>(renderSystem, meshSettings);
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
					"MeshSettings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
					LLGL::StageFlags::VertexStage, 0
				},
				LLGL::BindingDescriptor{
					"LightSettings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
					LLGL::StageFlags::FragmentStage, 1
				},
				LLGL::BindingDescriptor{
					"BoneBuffer", LLGL::ResourceType::Buffer, LLGL::BindFlags::Storage,
					LLGL::StageFlags::VertexStage,
					2
				},
				LLGL::BindingDescriptor{
					"LightBuffer", LLGL::ResourceType::Buffer, LLGL::BindFlags::Storage,
					LLGL::StageFlags::FragmentStage,
					3
				},
				LLGL::BindingDescriptor{
					"MaterialBuffer", LLGL::ResourceType::Buffer, LLGL::BindFlags::Storage,
					LLGL::StageFlags::FragmentStage,
					4
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

		renderSystem->WriteBuffer(*mMaterialBuffer, 0, &(material), sizeof(Material));

		std::vector<LLGL::ResourceViewDescriptor> resourceViews = {
			mConstantBuffer, mLightConstantBuffer, mBoneBuffer, mLightBuffer, mMaterialBuffer
		};
		InitResourceHeap(renderSystem, resourceViews);
	}

	resourceManager.InitModelVertexBuffers(renderSystem, *mShader);
	entityRegistry.GetEnttRegistry().on_construct<LightComponent>().connect<&MeshPipeline::AddLight>(this);
}

void MeshPipeline::Render(LLGL::CommandBuffer& commands, const Camera& camera,
                          const glm::mat4 projection, EntityRegistry& entityRegistry,
                          ResourceManager& resourceManager, const LLGL::RenderSystemPtr& renderSystem)
{
	if (!mQueuedLightEntities.empty())
	{
		_ProcessLights(entityRegistry);
	}

	commands.SetPipelineState(*mPipeline);

	// TODO: Sort the entities based on their mesh component file names
	const auto meshView = entityRegistry.GetEnttRegistry().view<
		const TransformComponent, MeshComponent, const SpriteComponent>(
		entt::exclude<MapComponent>);

	// Should either be a sprite or basic color
	meshView.each([this, &commands, &camera, &renderSystem, &resourceManager, &projection](
		const TransformComponent& transform,
		MeshComponent& mesh,
		const SpriteComponent& sprite)
		{
			// Set resources
			auto& texture = resourceManager.GetTexture(sprite.mTexturePath);
			commands.SetResourceHeap(*mResourceHeap);
			commands.SetResource(0, texture);
			_RenderModel(commands, mesh, camera, renderSystem, resourceManager, projection, transform);
		});
}

void MeshPipeline::RenderMap(LLGL::CommandBuffer& commands, const Camera& camera,
                             const LLGL::RenderSystemPtr& renderSystem, const ResourceManager& resourceManager,
                             const glm::mat4 projection, MeshComponent& meshComponent,
                             const TransformComponent& transform)
{
	_RenderModel(commands, meshComponent, camera, renderSystem, resourceManager, projection, transform);
}

void MeshPipeline::SetPipeline(LLGL::CommandBuffer& commands) const
{
	commands.SetPipelineState(*mPipeline);
}

void MeshPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, const Camera& camera,
                                                    const LLGL::RenderSystemPtr& renderSystem,
                                                    const glm::mat4 projection, const TransformComponent& transform,
                                                    MeshComponent& mesh)
{
	// Update
	auto model = glm::mat4(1.0f);

	// Translation matrix
	model = glm::translate(model, transform.mPosition);

	// Apply rotation in ZXY order
	model = glm::rotate(model, glm::radians(transform.mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(transform.mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(transform.mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

	model = glm::scale(model, transform.mSize);

	// Light Settings
	const auto lightsSize = mLights.size();
	if (lightsSize != lightSettings.numLights)
	{
		lightSettings.numLights = lightsSize;
		UpdateLightBuffer(renderSystem);
	}
	if (lightSettings.viewPos != camera.GetPosition())
	{
		lightSettings.viewPos = camera.GetPosition();
	}

	// Mesh Settings
	meshSettings.model = model;
	meshSettings.view = camera.GetView();
	meshSettings.projection = projection;
	meshSettings.textureClip = glm::mat4(1.0f);
	meshSettings.hasBones[0] = mesh.mHasBones;

	meshSettings.hasBones[1] = mesh.mCurrentBoneIndex;

	std::uint64_t transformOffset = 0;
	for (const auto& finalTransform : mesh.mFinalTransforms)
	{
		renderSystem->WriteBuffer(*mBoneBuffer, transformOffset, &(finalTransform), sizeof(glm::mat4));
		transformOffset += sizeof(glm::mat4);
	}


	commands.UpdateBuffer(*mConstantBuffer, 0, &meshSettings, sizeof(meshSettings));
	commands.UpdateBuffer(*mLightConstantBuffer, 0, &lightSettings, sizeof(lightSettings));
}

void MeshPipeline::SetResourceHeapTexture(LLGL::CommandBuffer& commands, LLGL::Texture& texture) const
{
	commands.SetResourceHeap(*mResourceHeap);
	commands.SetResource(0, texture);
}

void MeshPipeline::AddLight(EnTTRegistry& registry, EntityId entity)
{
	mQueuedLightEntities.push_back(entity);
}

void MeshPipeline::UpdateLightBuffer(const LLGL::RenderSystemPtr& renderSystem) const
{
	std::uint64_t lightOffset = 0;
	for (const auto& light : mLights)
	{
		renderSystem->WriteBuffer(*mLightBuffer, lightOffset, &(light), sizeof(LightUniform));
		lightOffset += sizeof(LightUniform);
	}
}

void MeshPipeline::_ProcessLights(EntityRegistry& entityRegistry)
{
	for (auto it = mQueuedLightEntities.begin(); it != mQueuedLightEntities.end();)
	{
		const auto& light = entityRegistry.GetComponent<LightComponent>(*it);
		const auto transform = entityRegistry.TryGetComponent<TransformComponent>(*it);
		if (transform != nullptr)
		{
			mLights.push_back({
				transform->mPosition, 1.0f, glm::vec4(light.mColor, 1.0f)
			});
			it = mQueuedLightEntities.erase(it);
		}
		else
		{
			// Move to the next element
			++it;
		}
	}
}

void MeshPipeline::_RenderModel(LLGL::CommandBuffer& commands, MeshComponent& meshComponent, const Camera& camera,
                                const LLGL::RenderSystemPtr& renderSystem, const ResourceManager& resourceManager,
                                const glm::mat4 projection, const TransformComponent& transform)
{
	const auto model = resourceManager.GetModelFromId(meshComponent.mMeshPath);
	UpdateProjectionViewModelUniform(commands, camera, renderSystem, projection, transform, meshComponent);

	for (const auto mesh : model.GetMeshes())
	{
		commands.SetVertexBuffer(*mesh->mVertexBuffer);
		commands.SetIndexBuffer(*mesh->mIndexBuffer);
		commands.DrawIndexed(mesh->mNumIndices, 0);
	}
}
