#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <vector>

struct DebugDrawable
{
	glm::vec2 color;
};

struct Line : DebugDrawable
{
	glm::vec2 x;
	glm::vec2 y;
};

struct Box : DebugDrawable
{
	glm::vec4 xy;
	glm::vec4 zw;
};

struct Grid : DebugDrawable
{
	Box mOutline;
	std::vector<Line> mLines;
};