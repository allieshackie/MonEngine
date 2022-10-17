#pragma once

class RenderObject
{
public:
	virtual ~RenderObject() = default;

	virtual void Draw() = 0;
};
