#pragma once

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

// Example: Debug color is 0-255 range
struct DebugDrawable
{
	virtual ~DebugDrawable() = default;
	glm::vec3 color = {1, 1, 1};
};

struct Line : virtual DebugDrawable
{
	glm::vec3 pointA = {0, 0, 0};
	glm::vec3 pointB = {0, 0, 0};
};

struct Box : virtual DebugDrawable
{
	glm::vec3 pointA = {-1, 1, 1};
	glm::vec3 pointB = {-1, -1, 1};
	glm::vec3 pointC = {1, 1, 1};
	glm::vec3 pointD = {1, -1, 1};
};

struct Grid : virtual DebugDrawable
{
	Box mOutline;
	std::vector<Line> mLines;
};
