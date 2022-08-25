#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "Sprite.h"

Sprite::Sprite(glm::vec2 pos, glm::vec2 size) : mSize(size)
{
    mPosition = glm::vec2(pos.x + size.x / 2, pos.y + size.y / 2);
    UpdateDrawData();
}

void Sprite::UpdateDrawData()
{
    mModel = glm::mat4(1.0f);
    mModel = glm::translate(mModel, glm::vec3(mPosition, 0.0f));

    mModel = glm::translate(mModel, glm::vec3(0.5f * mSize.x, 0.5f * mSize.y, 0.0f));
    mModel = glm::rotate(mModel, glm::radians(mRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    mModel = glm::translate(mModel, glm::vec3(-0.5f * mSize.x, -0.5f * mSize.y, 0.0f));

    mModel = glm::scale(mModel, glm::vec3(mSize, 1.0f));
}

void Sprite::UpdatePosition(glm::vec2 pos)
{
    mPosition = pos;
}

void Sprite::UpdateSize(glm::vec2 size)
{
    mSize = size;
}

const glm::mat4& Sprite::GetSpriteModelData() const
{
    return mModel;
}

glm::mat4 Sprite::GetTextureClip() const
{
    return mTextureClip;
}

glm::vec2 Sprite::GetSize() const
{
    return mSize;
}

glm::vec2 Sprite::GetPosition() const
{
    return mPosition;
}

float Sprite::GetRotation() const
{
    return mRotation;
}


void Sprite::UpdateRotation(float rot)
{
    mRotation = rot;
}
