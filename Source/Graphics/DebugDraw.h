#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <vector>

struct DebugDrawable
{
	virtual ~DebugDrawable() = default;
	glm::vec3 color;
};

struct Line : virtual DebugDrawable
{
	glm::vec2 pointA;
	glm::vec2 pointB;
};

struct Box : virtual DebugDrawable
{
	glm::vec2 pointA;
	glm::vec2 pointB;
	glm::vec2 pointC;
	glm::vec2 pointD;
};

struct Grid : virtual DebugDrawable
{
	Box mOutline;
	std::vector<Line> mLines;
};