#pragma once
#include <glm/vec2.hpp>

struct Vertex
{
	glm::vec2 position;
	glm::vec2 texCoord;
};

// TODO: Clip info will likely be supplied by Tileset, we can specify rows/columns

static glm::vec2 nullClip[4] = { {0, 0}, { 0,  1 }, { 1, 0 }, { 1,  1 } };

class Sprite
{
public:
	Sprite() = default;
	// position: top left corner of sprite, size: width, height
	Sprite(glm::vec2 pos, glm::vec2 size, const std::string& texturePath, glm::vec2 clip[4] = nullClip);

	void Draw(LLGL::CommandBuffer& commands) const;

private:
	
	void CreateVertexBuffer(glm::vec2 pos, glm::vec2 size, glm::vec2 clip[4] = nullClip);
	
	int mTextureId = 0;
	LLGL::Buffer* mVertexBuffer = nullptr;

	std::vector<Vertex> mVertices;
	uint32_t mNumVertices = 0;
};