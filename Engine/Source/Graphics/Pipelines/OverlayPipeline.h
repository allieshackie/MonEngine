#pragma once
#include "PipelineBase.h"
#include "Graphics/Core/ResourceManager.h"

class Animator;
class Camera;
class Entity;
class MonScene;
class Model;
class SceneManager;
class RenderObject;
class Shader;

struct ModelComponent;
struct TransformComponent;

struct OverlayElement
{
	std::vector<DebugVertex> vertices;
	glm::mat4 transform = glm::mat4(1.0f);
	LLGL::Buffer* mVertexBuffer = nullptr;
	int mId = 0;
};

class OverlayPipeline : public PipelineBase
{
public:
	OverlayPipeline(const LLGL::RenderSystemPtr& renderSystem);

	void Render(LLGL::CommandBuffer& commands, const glm::mat4 projMat);

	int AddOverlay(const std::vector<DebugVertex>& vertices, glm::mat4 transform);
	void UpdateOverlayTransform(int id, glm::mat4 transform) const;
	void DrawOverlay(const OverlayElement& element, LLGL::CommandBuffer& commands, const glm::mat4 projMat);
	void DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color);

	void ClearOverlays()
	{
		mOverlayElements.clear();
		mOverlayVertices.clear();
	}

private:
	const std::array<glm::vec3, 4> mBoxVertices{
		glm::vec3(-1.0f, 1.0f, 0.0f), // Top-left
		glm::vec3(-1.0f, -1.0f, 0.0f), // Bottom-left
		glm::vec3(1.0f, 1.0f, 0.0f), // Top-right
		glm::vec3(1.0f, -1.0f, 0.0f) // Bottom-right
	};

	const LLGL::RenderSystemPtr& mRenderSystem;
	std::vector<DebugVertex> mOverlayVertices;
	std::vector<std::unique_ptr<OverlayElement>> mOverlayElements;
	LLGL::Buffer* mOverlayVertexBuffer = nullptr;

	struct OverlaySettings
	{
		glm::mat4 pvmMat = glm::mat4();
	} overlaySettings = {};

	size_t mNumOverlayVertices = -1;
};
