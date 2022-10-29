#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "RenderObject.h"

RenderObject::RenderObject(glm::vec3 pos, glm::vec3 size) : mPosition(pos), mSize(size)
{
}

void RenderObject::Update()
{
	_UpdateModelMatrix();
}

glm::vec3 RenderObject::GetPosition() const
{
	return mPosition;
}

void RenderObject::SetPosition(glm::vec3 pos)
{
	mPosition = pos;
}

glm::vec3 RenderObject::GetSize() const
{
	return mSize;
}

void RenderObject::SetSize(glm::vec3 size)
{
	mSize = size;
}

float RenderObject::GetRotation() const
{
	return mRotation;
}

void RenderObject::SetRotation(float rot)
{
	mRotation = rot;
}

void RenderObject::_UpdateModelMatrix()
{
	mModel = glm::mat4(1.0f);

	mModel = translate(mModel, mPosition);

	mModel = translate(mModel, glm::vec3(0.5f * mSize.x, 0.5f * mSize.y, 0.0f));
	mModel = rotate(mModel, glm::radians(mRotation), glm::vec3(0.0f, 0.0f, 1.0f));
	mModel = translate(mModel, glm::vec3(-0.5f * mSize.x, -0.5f * mSize.y, 0.0f));

	mModel = scale(mModel, mSize);
}
