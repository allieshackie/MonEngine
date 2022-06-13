#include "LLGL/Utility.h"
#include "Core/ResourceManager.h"
#include "Core/RendererInstance.h"

#include "Sprite.h"


Sprite::Sprite(glm::vec2 pos, glm::vec2 size, const std::string& texturePath, glm::vec2 clip[4])
{
    mTextureId = ResourceManager::GetTextureId(texturePath);
    CreateVertexBuffer(pos, size, clip);
}

void Sprite::Draw(LLGL::CommandBuffer& commands) const
{
    ResourceManager::SetTexture(commands, mTextureId);
    commands.SetVertexBuffer(*mVertexBuffer);
    commands.Draw(mNumVertices, 0);
}

/*
 * 
// EXAMPLE DATA: BASE VALUES
mVertices = {
    { { -1,  1 }, { 0, 0 } }, // top left
    { { -1, -1 }, { 0,  1 } }, // bottom left
    { {  1,  1 }, {  1, 0 } }, // top right
    { {  1, -1 }, {  1,  1 } }, // bottom right
};
 */

void Sprite::CreateVertexBuffer(glm::vec2 pos, glm::vec2 size, glm::vec2 clip[4])
{
    const auto& renderSystem = RendererInstance::GetInstance()->GetRendererSystem();

    const float normalizeWidth = ResourceManager::Normalize(size.x);
    const float normalizeHeight = ResourceManager::Normalize(size.y);

	// NOTE: Texture coords are mapped like DirectX not OpenGL
    mVertices = {
		{ { pos.x, pos.y }, { clip[0].x, clip[0].y } }, // top left
        { { pos.x, pos.y - normalizeHeight }, { clip[1].x, clip[1].y } }, // bottom left
        { { pos.x + normalizeWidth, pos.y }, { clip[2].x, clip[2].y } }, // top right
        { { pos.x + normalizeWidth, pos.y - normalizeHeight }, { clip[3].x, clip[3].y } }, // bottom right
    };

    mNumVertices = static_cast<std::uint32_t>(mVertices.size());

    mVertexBuffer = renderSystem->CreateBuffer(
        LLGL::VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)), ResourceManager::GetVertexFormat()),
        mVertices.data()
    );
}
