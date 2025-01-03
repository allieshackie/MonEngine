#include "Entity/Components/MapComponent.h"
#include "Entity/Descriptions/AnimationDescription.h"
#include "Entity/Descriptions/CollisionDescription.h"
#include "Entity/Descriptions/LightDescription.h"
#include "Entity/Descriptions/MeshDescription.h"
#include "Entity/Descriptions/PhysicsDescription.h"
#include "Entity/Descriptions/PlayerDescription.h"
#include "Entity/Descriptions/SpriteDescription.h"
#include "Entity/Descriptions/TransformDescription.h"
#include "Graphics/Animation/Animator.h"
#include "GUI/GUISystem.h"
#include "GameInterface.h"

#include "EngineContext.h"

void EngineContext::_Init(const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
                          const LLGL::ColorRGBAf backgroundClearColor, bool usePerspective)
{
	// Init all systems **without** dependencies
	mInputHandler = std::make_shared<InputHandler>();
	mResourceManager = std::make_unique<ResourceManager>();
	mDescriptionFactory = std::make_unique<DescriptionFactory>();
	mEventPublisher = std::make_unique<EventPublisher>();
	mMapRegistry = std::make_unique<MapRegistry>();
	mTimer = std::make_unique<Timer>();
	mLuaSystem = std::make_unique<LuaSystem>();
	mRenderContext = std::make_unique<RenderContext>(screenSize, backgroundClearColor, usePerspective);

	// Init all systems **with** dependencies
	_InitDescriptions(); // Must be called before SceneManager sets up description factory
	mAnimator = std::make_unique<Animator>(*mResourceManager);
	mSceneManager = std::make_unique<SceneManager>(*mDescriptionFactory);
	mPhysicsSystem = std::make_unique<PhysicsSystem>(*this);

	// Remaining system setup
	mInputHandler->RegisterButtonUpHandler(LLGL::Key::Escape, [=]() { mRunning = false; });
	GUISystem::InitGUI(*mRenderContext);
	mResourceManager->LoadAllResources(mRenderContext->GetRenderSystem());
	mRenderContext->InitPipelines(title, mInputHandler, *mResourceManager);

	// TODO: Comment/uncomment for editor gui menu
	OpenEditorMenu();
}

void EngineContext::SetGUIMenu(std::unique_ptr<GUIBase> gui)
{
	mGUIMenu = std::move(gui);
}

/*  
 * Draw the XYZ axis at 0,0,0
 * Note: Not ideal for debugging since the axises are drawn in world 
 * space and don't follow the camera
 * Ideally, this could be draw directly to screen space and remain in the view
 * like UI
 */
void EngineContext::_DrawAxis() const
{
	// X Axis: Red.  Box on positive end
	DrawLine({-1, 0, 0}, {1, 0, 0}, {255, 0, 0, 1});
	DrawBox({1, 0, 0}, {0.1f, 0.1f, 1.0f}, {255, 0, 0, 1});

	// Y Axis: Green
	DrawLine({0, -1, 0}, {0, 1, 0}, {0, 255, 0, 1});
	DrawBox({0, 1, 0}, {0.1f, 0.1f, 1.0f}, {0, 255, 0, 1});

	// Z Axis: Blue
	DrawLine({0, 0, -1}, {0, 0, 1}, {0, 0, 255, 1});
	DrawBox({0, 0, 1}, {0.1f, 0.1f, 1.0f}, {0, 0, 255, 1});
}

void EngineContext::_RenderModelBones(Model& model, const MeshComponent& mesh, int nodeIndex,
                                      const glm::mat4 parentTransform) const
{
	const auto node = model.GetJointNodeAt(nodeIndex);

	if (node == nullptr)
	{
		return;
	}

	glm::mat4 globalTransform = parentTransform * node->mTransformation;
	auto bonePosition = glm::vec3(globalTransform[3]);
	auto parentPosition = glm::vec3(parentTransform[3]);

	// Default length for visualization purposes
	float defaultLength = 1.0f; // Adjust this as needed
	glm::vec3 direction = glm::normalize(bonePosition - parentPosition) * defaultLength;
	glm::vec3 endPosition = bonePosition + direction;

	glm::vec4 color = {1, 1, 1, 1};
	if (strcmp(node->mId.c_str(), "Head") == 0)
	{
		color = {1, 0, 0, 1};
	}

	DrawLine(bonePosition, endPosition, color);

	for (const auto child : node->mChildren)
	{
		_RenderModelBones(model, mesh, child, globalTransform);
	}
}

void EngineContext::_InitDescriptions() const
{
	mDescriptionFactory->RegisterDescription<AnimationDescription>(AnimationDescription::JsonName);
	mDescriptionFactory->RegisterDescription<CollisionDescription>(CollisionDescription::JsonName);
	mDescriptionFactory->RegisterDescription<LightDescription>(LightDescription::JsonName);
	mDescriptionFactory->RegisterDescription<MeshDescription>(MeshDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PhysicsDescription>(PhysicsDescription::JsonName);
	mDescriptionFactory->RegisterDescription<PlayerDescription>(PlayerDescription::JsonName);
	mDescriptionFactory->RegisterDescription<SpriteDescription>(SpriteDescription::JsonName);
	mDescriptionFactory->RegisterDescription<TransformDescription>(TransformDescription::JsonName);
}

void EngineContext::_FixedUpdate(float dt) const
{
	mPhysicsSystem->Update(dt, mSceneManager->GetCurrentScene());
}

EngineContext::EngineContext(GameInterface* game, const LLGL::Extent2D screenSize, const LLGL::UTF8String& title,
                             const LLGL::ColorRGBAf backgroundClearColor, bool usePerspective)
{
	_Init(screenSize, title, backgroundClearColor, usePerspective);
	game->Init(this);
	// NOTE: Scene callbacks need to be set before scene gets loaded in StartGame()
	SetSceneCallbacks(game);
	game->RegisterEntityDescriptions();
	game->StartGame();
}

void EngineContext::Run(GameInterface* game)
{
	// Init current time
	mTimer->mCurrentTime = Clock::now();

	while (mRenderContext->ProcessEvents() && mRunning)
	{
		auto frameTime = Clock::now();
		Duration elapsed = frameTime - mTimer->mCurrentTime;
		mTimer->mCurrentTime = frameTime;

		double deltaTime = elapsed.count();
		if (deltaTime > 0.25)
		{
			// Clamp large deltaTime to prevent "spiral of death"
			// where we could potentially always be "catching up"
			deltaTime = 0.25;
		}

		mTimer->mAccumulator += deltaTime;

		while (mTimer->mAccumulator >= deltaTime)
		{
			_FixedUpdate(mTimer->mDT);
			mTimer->mAccumulator -= mTimer->mDT;
		}

		mInputHandler->Update();
		game->Update(mTimer->mDT);
		mSceneManager->GetCamera().Update();

		mAnimator->Update(deltaTime, mSceneManager->GetCurrentScene(), *mResourceManager);

		// TODO: Debug draw model bones

		/*
		 *
		const auto meshView = mEntityRegistry->GetEnttRegistry().view<const MeshComponent>(entt::exclude<MapComponent>);

		// Should either be a sprite or basic color
		meshView.each([=](const MeshComponent& mesh)
		{
			auto model = mResourceManager->GetModelFromId(mesh.mMeshPath);
			_RenderModelBones(model, mesh, model.GetRootNodeIndex(), glm::mat4(1.0f));
		});

		 */

		// TODO: Debug draw axis
		//_DrawAxis();

		mRenderContext->BeginFrame();

		game->Render();

		if (mSceneManager->GetCurrentScene())
		{
			mRenderContext->Render(mSceneManager->GetCamera(), mSceneManager->GetCurrentScene(), *mResourceManager);
		}

		// Render GUI last so menus draw on top
		GUISystem::GUIStartFrame();
		GUISystem::RenderMenus();
		// GUISystem::RenderGuiElements();  DEBUG GUI MENU
		if (mEditorGUI != nullptr)
		{
			mEditorGUI->Render(mSceneManager->GetCurrentScene(), *mResourceManager);
		}
		GUISystem::GUIEndFrame();

		mRenderContext->EndFrame();

		if (mLuaSystem->QueueCloseScripts())
		{
			mLuaSystem->CloseAllScripts();
		}
	}

	GUISystem::CloseGUI();
}

void EngineContext::LoadFont(const char* fontFileName) const
{
	mRenderContext->LoadFont(fontFileName);
}

void EngineContext::OpenEditorMenu()
{
	mEditorGUI = std::make_unique<EditorGUI>();
}

void EngineContext::SetSceneCallbacks(const GameInterface* game) const
{
	// loading new scene means setting up callbacks for various systems
	mAnimator->SetSceneCallbacks(*mSceneManager);
	mPhysicsSystem->SetSceneCallbacks(*mSceneManager);
	mRenderContext->SetSceneCallbacks(*mSceneManager);
	game->SetSceneCallbacks(*mSceneManager);
}

// ========== ACCESSORS FOR GAME INTERFACE ==============
InputHandler& EngineContext::GetInputHandler() const
{
	return *mInputHandler;
}

Camera& EngineContext::GetCamera() const
{
	return mSceneManager->GetCamera();
}

const std::vector<const char*>& EngineContext::GetSceneNames() const
{
	return mSceneManager->GetSceneNames();
}

void EngineContext::LoadScene(const char* sceneName) const
{
	mSceneManager->LoadScene(sceneName, *this, *mMapRegistry, *mLuaSystem);
}

MonScene* EngineContext::GetScene() const
{
	return mSceneManager->GetCurrentScene();
}

void EngineContext::GenerateMapTexture(Entity* mapEntity) const
{
	mRenderContext->GenerateMapTexture(*mResourceManager, mapEntity);
}

void EngineContext::DrawPoint(glm::vec3 position, float size, glm::vec4 color) const
{
	mRenderContext->DrawPoint(position, color, size);
}

void EngineContext::DrawLine(glm::vec3 from, glm::vec3 to, glm::vec4 color) const
{
	mRenderContext->DrawLine(from, to, color);
}

void EngineContext::DrawBox(glm::vec3 position, glm::vec3 size, glm::vec4 color, bool filled) const
{
	mRenderContext->DrawBox(position, size, color, filled);
}

void EngineContext::DrawCircle(glm::vec3 position, float radius, glm::vec4 color) const
{
	mRenderContext->DrawCircle(position, radius, color);
}

void EngineContext::DrawGrid(glm::vec3 position, glm::vec3 size, int rows, int columns, glm::vec4 color) const
{
	mRenderContext->DrawGrid(position, size, color, rows, columns);
}

void EngineContext::DrawText2D(const char* text, glm::vec2 position, glm::vec2 size, glm::vec4 color) const
{
	mRenderContext->DrawText(text, position, size, color);
}

void EngineContext::DrawText3D(const char* text, glm::vec3 position, glm::vec3 size) const
{
	// TODO: This will just render 2D text for now
	mRenderContext->DrawText(text, position, size, {1.0f, 1.0f, 1.0f, 1.0f});
}

void EngineContext::SetBackgroundClearColor(const LLGL::ColorRGBAf color) const
{
	mRenderContext->SetBackgroundClearColor(color);
}

void EngineContext::ToggleEditorMode(bool toggle) const
{
	if (toggle)
	{
		if (mSceneManager->GetCurrentScene())
		{
			mInputHandler->AddEditorInputs(mSceneManager->GetCamera());
		}
	}
}
