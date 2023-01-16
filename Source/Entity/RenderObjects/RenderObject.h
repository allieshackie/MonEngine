#pragma once
#include <LLGL/LLGL.h>
#include "LLGL/Buffer.h"
#include "Core/Vertex.h"

class Renderer;

class RenderObject
{
public:
	RenderObject(glm::vec3 pos, glm::vec3 size, int textureId = 0);

	virtual ~RenderObject();

	virtual void Draw(const Renderer& renderer, LLGL::CommandBuffer& commands) = 0;
	void Update();

	glm::vec3 GetPosition() const;
	void SetPosition(glm::vec3 pos);

	glm::vec3 GetSize() const;
	void SetSize(glm::vec3 size);

	float GetRotation() const;
	void SetRotation(float rot);

	glm::mat4 GetModel() const;

	LLGL::Buffer& GetVertexBuffer() const;
	void SetVertexBuffer(LLGL::Buffer* buffer);
	const std::vector<Vertex>& GetVertices() const;

	int getTextureId() const;

protected:
	void _UpdateModelMatrix();

	glm::vec3 mPosition = {0, 0, 0};
	glm::vec3 mSize = {1, 1, 1};
	float mRotation = 0.0f;

	glm::mat4 mModel = glm::mat4(1.0);

	LLGL::Buffer* mVertexBuffer = nullptr;
	std::vector<Vertex> mVertices;

	int mTextureId = 0;
};
