#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "Sprite.h"

Sprite::Sprite(glm::vec2 pos, glm::vec2 size) : mSize(size)
{
    mPosition = glm::vec2(pos.x + size.x / 2, pos.y + size.y / 2);
    UpdateDrawData();
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
void Sprite::UpdateDrawData()
{
    mModel = glm::mat4(1.0f);
    mModel = glm::translate(mModel, glm::vec3(mPosition, 0.0f));

	// Sprite origin is center, no need to translate
    //mModel = glm::translate(mModel, glm::vec3(0.5f * mSize.x, 0.5f * mSize.y, 0.0f));
    mModel = glm::rotate(mModel, glm::radians(mRotation), glm::vec3(0.0f, 0.0f, 1.0f));
    //mModel = glm::translate(mModel, glm::vec3(-0.5f * mSize.x, -0.5f * mSize.y, 0.0f));

    mModel = glm::scale(mModel, glm::vec3(mSize, 1.0f));
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

void Sprite::UpdatePositionX(float x)
{
    mPosition.x = x;
}

void Sprite::UpdatePositionY(float y)
{
    mPosition.y = y;
}

void Sprite::UpdateSizeX(float x)
{
    mSize.x = x;
}

void Sprite::UpdateSizeY(float y)
{
    mSize.y = y;
}

void Sprite::UpdateRotation(float rot)
{
    mRotation = rot;
}
