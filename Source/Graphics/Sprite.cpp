#include "LLGL/Utility.h"
#include "ResourceManager.h"

#include "Sprite.h"

Sprite::Sprite(LLGL::RenderSystem& renderer, glm::vec2 pos, glm::vec2 size, int textureId)
{
    CreateVertexBuffer(renderer, pos, size, textureId);
}

void Sprite::Draw(LLGL::CommandBuffer& commands) const
{
    commands.SetVertexBuffer(*mVertexBuffer);
    commands.Draw(mNumVertices, 0);
}

void Sprite::CreateVertexBuffer(LLGL::RenderSystem& renderer, glm::vec2 pos, glm::vec2 size, int textureId)
{
    LLGL::VertexFormat vertexFormat;
    vertexFormat.AppendAttribute({ "position", LLGL::Format::RG32Float });
    vertexFormat.AppendAttribute({ "texCoord", LLGL::Format::RG32Float });

    ResourceManager::LoadShaderProgram(renderer, vertexFormat, "texture.vert", "texture.frag");

    mVertices = {
        { { pos.x,  pos.y }, { -2, -2 } },
        { { pos.x + size.x, pos.y }, { -2,  2 } },
        { {  pos.x,  pos.y - size.y }, {  2, -2 } },
        { {  pos.x + size.x, pos.y - size.y }, {  2,  2 } },
    };

    mNumVertices = static_cast<std::uint32_t>(mVertices.size());

    mVertexBuffer = renderer.CreateBuffer(
        LLGL::VertexBufferDesc(static_cast<std::uint32_t>(mVertices.size() * sizeof(Vertex)), vertexFormat),
        mVertices.data()
    );
}
