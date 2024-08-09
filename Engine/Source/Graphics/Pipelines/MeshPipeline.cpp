#include <filesystem>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "LLGL/Utils/Parse.h"

#include "Core/Camera.h"
#include "Entity/Components/LightComponent.h"
#include "Entity/Components/MapComponent.h"
#include "Entity/Components/SpriteComponent.h"
#include "Entity/Components/TransformComponent.h"

#include "MeshPipeline.h"

MeshPipeline::MeshPipeline(LLGL::RenderSystemPtr& renderSystem, EntityRegistry& entityRegistry) : PipelineBase()
{
	// Initialization
	{
		InitConstantBuffer<Settings>(renderSystem, settings);

		LLGL::VertexFormat vertexFormat;
		vertexFormat.AppendAttribute({"position", LLGL::Format::RGB32Float, 0, 0, sizeof(Vertex), 0});
		vertexFormat.AppendAttribute({"normal", LLGL::Format::RGB32Float, 1, 12, sizeof(Vertex), 0});
		vertexFormat.AppendAttribute({"texCoord", LLGL::Format::RG32Float, 2, 24, sizeof(Vertex), 0});
		InitShader(renderSystem, vertexFormat, "mesh.vert", "mesh.frag");
		LLGL::PipelineLayoutDescriptor layoutDesc;
		{
			layoutDesc.heapBindings =
			{
				LLGL::BindingDescriptor{
					"Settings", LLGL::ResourceType::Buffer, LLGL::BindFlags::ConstantBuffer,
					LLGL::StageFlags::VertexStage | LLGL::StageFlags::FragmentStage, 0
				},
				LLGL::BindingDescriptor{
					"LightBuffer", LLGL::ResourceType::Buffer, LLGL::BindFlags::Storage,
					LLGL::StageFlags::FragmentStage,
					1
				},
				LLGL::BindingDescriptor{
					"MaterialBuffer", LLGL::ResourceType::Buffer, LLGL::BindFlags::Storage,
					LLGL::StageFlags::FragmentStage,
					2
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

		renderSystem->WriteBuffer(*mMaterialBuffer, 0, &(material), sizeof(Material));

		mTextures = ResourceManager::LoadAllTexturesFromFolder(renderSystem);

		std::vector<LLGL::ResourceViewDescriptor> resourceViews = {mConstantBuffer, mLightBuffer, mMaterialBuffer};
		InitResourceHeap(renderSystem, resourceViews);
	}

	for (const auto& entry : std::filesystem::directory_iterator(MODELS_FOLDER))
	{
		auto fullPath = entry.path().string();
		auto model = Model(fullPath, entry.path().filename().string());
		model.InitializeBuffers(renderSystem, *mShader);
		mModelVertexBuffers[model.GetId()] = model;
	}

	entityRegistry.GetEnttRegistry().on_construct<LightComponent>().connect<&MeshPipeline::AddLight>(this);
}

void MeshPipeline::Render(LLGL::CommandBuffer& commands, const Camera& camera,
                          const glm::mat4 projection, EntityRegistry& entityRegistry,
                          const LLGL::RenderSystemPtr& renderSystem)
{
	if (!mQueuedLightEntities.empty())
	{
		_ProcessLights(entityRegistry);
	}

	commands.SetPipelineState(*mPipeline);

	// TODO: Sort the entities based on their mesh component file names
	const auto meshView = entityRegistry.GetEnttRegistry().view<
		const TransformComponent, const MeshComponent, const SpriteComponent>(
		entt::exclude<MapComponent>);

	// Should either be a sprite or basic color
	meshView.each([this, &commands, &camera, &renderSystem, &projection](const TransformComponent& transform,
	                                                                     const MeshComponent& mesh,
	                                                                     const SpriteComponent& sprite)
	{
		// Set resources
		const auto textureId = ResourceManager::GetTextureId(sprite.mTexturePath);
		commands.SetResourceHeap(*mResourceHeap);
		commands.SetResource(0, mTextures[textureId]->GetTextureData());
		_RenderModel(commands, mesh, camera, renderSystem, projection, transform);
	});
}

void MeshPipeline::RenderMap(LLGL::CommandBuffer& commands, const Camera& camera,
                             const LLGL::RenderSystemPtr& renderSystem, const glm::mat4 projection,
                             const MeshComponent& meshComponent, const TransformComponent& transform)
{
	_RenderModel(commands, meshComponent, camera, renderSystem, projection, transform);
}

void MeshPipeline::SetPipeline(LLGL::CommandBuffer& commands) const
{
	commands.SetPipelineState(*mPipeline);
}

void MeshPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, const Camera& camera,
                                                    const LLGL::RenderSystemPtr& renderSystem,
                                                    const glm::mat4 projection,
                                                    const TransformComponent& transform)
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

	const auto lightsSize = mLights.size();
	if (lightsSize != settings.numLights)
	{
		settings.numLights = lightsSize;
		UpdateLightBuffer(renderSystem);
	}

	settings.model = model;
	settings.view = camera.GetView();
	settings.projection = projection;
	settings.textureClip = glm::mat4(1.0f);

	if (settings.viewPos != camera.GetPosition())
	{
		settings.viewPos = camera.GetPosition();
	}
	commands.UpdateBuffer(*mConstantBuffer, 0, &settings, sizeof(settings));
}

void MeshPipeline::SetResourceHeapTexture(LLGL::CommandBuffer& commands, int textureId) const
{
	commands.SetResourceHeap(*mResourceHeap);
	commands.SetResource(0, mTextures[textureId]->GetTextureData());
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

void MeshPipeline::_RenderModel(LLGL::CommandBuffer& commands, const MeshComponent& meshComponent, const Camera& camera,
                                const LLGL::RenderSystemPtr& renderSystem,
                                const glm::mat4 projection, const TransformComponent& transform)
{
	UpdateProjectionViewModelUniform(commands, camera, renderSystem, projection, transform);
	const auto model = mModelVertexBuffers.find(meshComponent.mMeshPath);
	if (model != mModelVertexBuffers.end())
	{
		for (const auto& mesh : model->second.GetMeshes())
		{
			commands.SetVertexBuffer(*mesh.mVertexBuffer);
			commands.SetIndexBuffer(*mesh.mIndexBuffer);
			commands.DrawIndexed(mesh.mNumIndices, 0);
		}
	}
}
