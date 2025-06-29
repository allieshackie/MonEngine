#include <filesystem>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "LLGL/Utils/Parse.h"

#include "Core/Camera.h"
#include "Core/Scene.h"
#include "Core/SceneManager.h"
#include "Entity/Entity.h"
#include "Entity/Components/TransformComponent.h"
#include "Graphics/Animation/Animation.h"

#include "MeshPipeline.h"

void MeshPipeline::Render(LLGL::CommandBuffer& commands, const Camera& camera,
                          const glm::mat4 projection, MonScene* scene,
                          ResourceManager& resourceManager, const LLGL::RenderSystemPtr& renderSystem)
{
	if (!mQueuedLightEntities.empty())
	{
		_ProcessLights();
	}

	commands.SetPipelineState(*mPipeline);

	frameSettings.view = camera.GetView();
	frameSettings.projection = projection;
	commands.UpdateBuffer(*mFrameBuffer, 0, &frameSettings, sizeof(frameSettings));

	const auto meshView = scene->GetRegistry().view<const TransformComponent, ModelComponent>();

	// Should either be a sprite or basic color
	meshView.each([this, &commands, &camera, &renderSystem, &resourceManager](
		const TransformComponent& transform, ModelComponent& modelComponent)
		{
			commands.SetResourceHeap(*mResourceHeap);
			const auto& model = resourceManager.GetModelFromId(modelComponent.mModelPath);
			// Set resources
			UpdateProjectionViewModelUniform(commands, camera, renderSystem, transform, modelComponent, model);

			for (const auto mesh : model.GetMeshes())
			{
				// Mesh Settings
				//meshSettings.solidColor = model.mColor;
				meshSettings.hasBones = model.GetNumJoints() > 0;
				meshSettings.hasTexture = mesh->mTextureId != -1;
				meshSettings.gTargetBone = static_cast<float>(modelComponent.mCurrentBoneIndex);
				commands.UpdateBuffer(*mConstantBuffer, 0, &meshSettings, sizeof(meshSettings));
				auto& texture = resourceManager.GetTexture(mesh->mTextureId);
				auto& sampler = resourceManager.GetSampler(mesh->mTextureId);
				commands.SetResource(0, texture);
				commands.SetResource(1, sampler);
				commands.SetVertexBuffer(*mesh->mVertexBuffer);
				commands.SetIndexBuffer(*mesh->mIndexBuffer);
				commands.DrawIndexed(mesh->mNumIndices, 0);
			}
		});
}

void MeshPipeline::SetPipeline(LLGL::CommandBuffer& commands) const
{
	commands.SetPipelineState(*mPipeline);
}

void MeshPipeline::UpdateProjectionViewModelUniform(LLGL::CommandBuffer& commands, const Camera& camera,
                                                    const LLGL::RenderSystemPtr& renderSystem,
                                                    const TransformComponent& transform, ModelComponent& mesh,
                                                    const Model& meshModel)
{
	// Update
	auto model = glm::mat4(1.0f);

	// Translation matrix
	model = glm::translate(model, transform.mPosition);

	// Apply rotation in ZXY order
	model = glm::rotate(model, glm::radians(transform.mRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, glm::radians(transform.mRotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(transform.mRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));

	auto calculatedSize = meshModel.CalculateModelScaling(transform.mSize);
	model = glm::scale(model, calculatedSize);

	// Light Settings
	int lightsSize = static_cast<int>(mLights.size());
	if (lightsSize != lightSettings.numLights)
	{
		lightSettings.numLights = lightsSize;
		UpdateLightBuffer(renderSystem);
	}
	if (lightSettings.viewPos != camera.GetPosition())
	{
		lightSettings.viewPos = camera.GetPosition();
	}

	meshSettings.model = model;

	std::uint64_t transformOffset = 0;
	for (const auto& finalTransform : mesh.mFinalTransforms)
	{
		renderSystem->WriteBuffer(*mBoneBuffer, transformOffset, &(finalTransform), sizeof(glm::mat4));
		transformOffset += sizeof(glm::mat4);
	}

	commands.UpdateBuffer(*mLightConstantBuffer, 0, &lightSettings, sizeof(lightSettings));
}

MeshPipeline::MeshPipeline(const LLGL::RenderSystemPtr& renderSystem,
                           const ResourceManager& resourceManager) : PipelineBase()
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

		renderSystem->WriteBuffer(*mMaterialBuffer, 0, &(material), sizeof(Material));

		const std::vector<LLGL::ResourceViewDescriptor> resourceViews = {
			mFrameBuffer, mConstantBuffer, mLightConstantBuffer, mBoneBuffer, mLightBuffer, mMaterialBuffer
		};
		InitResourceHeap(renderSystem, resourceViews);
	}

	resourceManager.InitModelVertexBuffers(renderSystem, *mShader);
}

void MeshPipeline::SetResourceHeapTexture(LLGL::CommandBuffer& commands, LLGL::Texture& texture) const
{
	commands.SetResourceHeap(*mResourceHeap);
	commands.SetResource(0, texture);
}

void MeshPipeline::AddLight(Entity* entity)
{
	mQueuedLightEntities.push_back(entity);
}

void MeshPipeline::SetSceneCallbacks(const SceneManager& sceneManager)
{
	EventFunc func = [this](Entity* entity)
	{
		AddLight(entity);
	};
	sceneManager.ConnectOnConstruct<LightComponent>(func);
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

void MeshPipeline::_ProcessLights()
{
	for (auto it = mQueuedLightEntities.begin(); it != mQueuedLightEntities.end();)
	{
		const auto& light = (*it)->GetComponent<LightComponent>();
		const auto transform = (*it)->TryGetComponent<TransformComponent>();
		if (transform != nullptr)
		{
			mLights.push_back({
				light.mAmbient, light.mDiffuse, light.mSpecular, transform->mPosition, light.mIntensity,
				{static_cast<int>(light.mLightType), 0, 0, 0}
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
