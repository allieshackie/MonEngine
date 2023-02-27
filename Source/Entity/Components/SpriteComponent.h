#pragma once
#include <string>
#include <glm/vec2.hpp>

struct SpriteComponent {
	std::string mTexturePath;
	glm::vec2 mTexClip = { 0.0f, 0.0f };
	glm::vec2 mTexScale = { 1.0f, 1.0f };
};
