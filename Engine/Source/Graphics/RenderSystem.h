#pragma once

#include "Core/ISystem.h"
#include "Graphics/Pipelines/ImmediatePipeline.h"
#include "Graphics/Pipelines/MeshPipeline.h"
#include "Pipelines/OverlayPipeline.h"
#include "Graphics/Pipelines/TextPipeline.h"

class Camera;
class InputHandler;
class ResourceManager;
class World;

struct OverlayElement;

class RenderSystem : public ISystem
{
public:
	RenderSystem();

	void Render(std::weak_ptr<World> world) override;

	void Shutdown() override;

	void LoadFont(const char* fontFileName) const;
	void ClearOverlay() const;

	int AddOverlay(const std::vector<DebugVertex>& vertices, glm::mat4 transform) const;
	void UpdateOverlayTransform(int id, glm::mat4 transform) const;

	void DrawPoint(glm::vec3 pos, glm::vec4 color, float size) const;
	void DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color) const;
	void DrawBox(glm::vec3 pos, glm::vec3 size, glm::vec4 color, bool filled) const;
	void DrawCircle(glm::vec3 position, float radius, glm::vec4 color) const;
	void DrawGrid() const;
	void DrawOverlayLine(glm::vec3 from, glm::vec3 to, glm::vec4 color) const;
	// Text Draw
	uint32_t AddText(const char* text, glm::vec2 position, glm::vec2 size, glm::vec4 color) const;
	void UpdateText(uint32_t index, const std::string& text);
	void SetVisible(uint32_t index, bool visible);
	void SetPosition(uint32_t index, glm::vec2 pos);
	void RemoveText(uint32_t index);

	const Shader& GetMeshShader() const { return mMeshPipeline->GetShader(); }

	Material& GetMaterial();
	void UpdateLights();

	LLGL::CommandBuffer& GetCommands() const { return *mCommands; }
	LLGL::CommandQueue& GetCommandQueue() const { return *mCommandQueue; }
	const LLGL::RenderSystemPtr& GetSystem() const { return mSystem; }
	glm::mat4 GetPerspectiveProjection() const { return mPerspectiveProjection; }
	glm::mat4 GetOrthoProjection() const { return mOrthoProjection; }

	void UpdateProjections(const LLGL::Extent2D res);

	bool GetBackendNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;
	bool GetCommandBufferNativeHandle(void* nativeHandle, std::size_t nativeHandleSize) const;

	void OnWorldCreated(World* world);
	void OnWindowCreated(ResourceManager& resourceManager);

private:

	LLGL::RenderSystemPtr mSystem; // Render system
	LLGL::CommandBuffer* mCommands = nullptr; // Main command buffer
	LLGL::CommandQueue* mCommandQueue = nullptr; // Command queue
	glm::mat4 mPerspectiveProjection = glm::identity<glm::mat4>();
	glm::mat4 mOrthoProjection = glm::identity<glm::mat4>();

	std::unique_ptr<MeshPipeline> mMeshPipeline;
	std::unique_ptr<OverlayPipeline> mOverlayPipeline;
	std::unique_ptr<ImmediatePipeline> mImmediatePipeline;
	std::unique_ptr<TextPipeline> mTextPipeline;
};
