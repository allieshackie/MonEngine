#pragma once
#include "Core/Renderer.h"
#include "RenderObject.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>

class DebugDrawable : public RenderObject
{
public:
	// position: top left corner of sprite, size: width, height
	DebugDrawable(glm::vec3 pos, glm::vec3 size, glm::vec3 colorParam) : RenderObject(
		                                                                     pos, size), color(colorParam)
	{
	}

	glm::vec3 GetColor() const { return color; }

	void Draw(const Renderer& renderer, LLGL::CommandBuffer& commands) override
	{
		Update();
		renderer.UpdateDebugProjectionViewModelUniform(mModel);
		commands.Draw(mVertices.size(), 0);
	}

protected:
	glm::vec3 color = {1, 1, 1};
};

class Line : public DebugDrawable
{
public:
	Line(glm::vec3 A, glm::vec3 B, glm::vec3 pos, glm::vec3 size, glm::vec3 colorParam)
		: DebugDrawable(pos, size, colorParam), pointA(A), pointB(B)
	{
		mVertices = {
			{A, color, {0, 0}},
			{B, color, {0, 0}}
		};
	}

	glm::vec3 GetPointA() const { return pointA; }
	glm::vec3 GetPointB() const { return pointB; }

private:
	glm::vec3 pointA = {0, 0, 0};
	glm::vec3 pointB = {0, 0, 0};
};

class Box : public DebugDrawable
{
public:
	Box(glm::vec3 pos, glm::vec3 size, glm::vec3 colorParam)
		: DebugDrawable(pos, size, colorParam)
	{
		mVertices = {
			{pointA, color, {0, 0}},
			{pointB, color, {0, 0}},
			{pointB, color, {0, 0}},
			{pointD, color, {0, 0}},
			{pointD, color, {0, 0}},
			{pointC, color, {0, 0}},
			{pointC, color, {0, 0}},
			{pointA, color, {0, 0}},
		};
	}

	glm::vec3 GetPointA() const { return pointA; }
	glm::vec3 GetPointB() const { return pointB; }
	glm::vec3 GetPointC() const { return pointC; }
	glm::vec3 GetPointD() const { return pointD; }

private:
	glm::vec3 pointA = {-0.5, 0.5, 1};
	glm::vec3 pointB = {-0.5, -0.5, 1};
	glm::vec3 pointC = {0.5, 0.5, 1};
	glm::vec3 pointD = {0.5, -0.5, 1};
};

class Grid : public DebugDrawable
{
public:
	Grid(Box box, std::vector<Line> lines, glm::vec3 pos, glm::vec3 size, glm::vec3 colorParam)
		: DebugDrawable(pos, size, colorParam), mOutline(std::move(box)), mLines(std::move(lines))
	{
		mVertices = {
			{mOutline.GetPointA(), color, {0, 0}},
			{mOutline.GetPointB(), color, {0, 0}},
			{mOutline.GetPointB(), color, {0, 0}},
			{mOutline.GetPointD(), color, {0, 0}},
			{mOutline.GetPointD(), color, {0, 0}},
			{mOutline.GetPointC(), color, {0, 0}},
			{mOutline.GetPointC(), color, {0, 0}},
			{mOutline.GetPointA(), color, {0, 0}},
		};

		for (auto& line : mLines)
		{
			mVertices.push_back({line.GetPointA(), line.GetColor(), {0, 0}});
			mVertices.push_back({line.GetPointB(), line.GetColor(), {0, 0}});
		}
	}

	const Box& GetOutline() { return mOutline; }
	const std::vector<Line>& GetLines() { return mLines; }

private:
	Box mOutline;
	std::vector<Line> mLines;
};
