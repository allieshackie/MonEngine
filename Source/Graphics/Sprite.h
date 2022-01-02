#pragma once
#include <glm/vec2.hpp>
#include <LLGL/LLGL.h>

struct Vertex
{
	glm::vec2 position;
	glm::vec2 texCoord;
};

class Sprite
{
public:
	// position: top left corner of sprite, size: width, height
	Sprite(LLGL::RenderSystem& renderer, glm::vec2 pos, glm::vec2 size, int textureId);

	void Draw(LLGL::CommandBuffer& commands) const;

private:
	
	void CreateVertexBuffer(LLGL::RenderSystem& renderer, glm::vec2 pos, glm::vec2 size, int textureId);
	
	std::string mTextureName;
	LLGL::Buffer* mVertexBuffer = nullptr;

	std::vector<Vertex> mVertices;
	uint32_t mNumVertices = 0;
};