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

class OverlayPipeline : public PipelineBase
{
public:
	OverlayPipeline(const LLGL::RenderSystemPtr& renderSystem);

	void Render(LLGL::CommandBuffer& commands);

	void DrawOverlay(glm::vec2 pos, glm::vec4 color);
	void DrawLine(glm::vec2 from, glm::vec2 to, glm::vec4 color);

	void ClearOverlay() { mOverlayElements.clear(); }

private:
	const std::array<glm::vec2, 4> mBoxVertices{
		glm::vec2(-1.0f, 1.0f), // Top-left
		glm::vec2(-1.0f, -1.0f), // Bottom-left
		glm::vec2(1.0f, 1.0f), // Top-right
		glm::vec2(1.0f, -1.0f) // Bottom-right
	};

	std::vector<OverlayVertex> mOverlayElements;
	LLGL::Buffer* mOverlayVertexBuffer = nullptr;

	size_t mNumOverlayElements = -1;
};
