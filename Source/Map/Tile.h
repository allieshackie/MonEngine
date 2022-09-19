#pragma once
#include <glm/vec2.hpp>
#include "Interactable.h"
#include "Sprite.h"

class Tile: public Sprite {
public:
    Tile(glm::vec2 pos, glm::vec2 size, glm::vec2 clip = {0.0f, 0.0f}, glm::vec2 scale = {1.0f, 1.0f});

	glm::vec2 GetClip() const;
	glm::vec2 GetScale() const;
	
	void UpdateTextureClip();

	void UpdateClip(glm::vec2 clip);
	void UpdateScale(glm::vec2 scale);

private:
	glm::vec2 mClip = { 0.0f, 0.0f };
	glm::vec2 mScale = { 1.0f, 1.0f };
};

class InteractiveTile: public Tile, public Interactable
{
public:
	InteractiveTile(glm::vec2 pos, glm::vec2 size, glm::vec2 clip = { 0.0f, 0.0f }, glm::vec2 scale = { 1.0f, 1.0f })
	: Tile(pos, size, clip, scale), Interactable(pos, size)
	{
		
	}

	void OnClick() override
	{
		if (mOnClickCallback)
		{
			mOnClickCallback();
		}
	}

	void SetOnClickCallback(const std::function<void()>& cb)
	{
		mOnClickCallback = cb;
	}

private:
	std::function<void()> mOnClickCallback;
};
