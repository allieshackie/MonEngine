#pragma once
#define BT_LINE_BATCH_SIZE 512
#include "bullet/LinearMath/btAlignedObjectArray.h"
#include "bullet/LinearMath/btIDebugDraw.h"

class RenderSystem;

struct MyDebugVec3
{
	MyDebugVec3(const btVector3& org)
		: x(org.x()),
		  y(org.y()),
		  z(org.z())
	{
	}

	float x;
	float y;
	float z;
};

class PhysicsDebugDraw : public btIDebugDraw
{
public:
	PhysicsDebugDraw(RenderSystem& renderSystem);

	~PhysicsDebugDraw() override = default;

	DefaultColors getDefaultColors() const override;

	///the default implementation for setDefaultColors has no effect. A derived class can implement it and store the colors.
	void setDefaultColors(const DefaultColors& colors) override;
	void drawLine(const btVector3& from1, const btVector3& to1, const btVector3& color1) override;
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime,
	                      const btVector3& color) override;

	void reportErrorWarning(const char* warningString) override;

	void draw3dText(const btVector3& location, const char* textString) override;

	void setDebugMode(int debugMode) override;
	int getDebugMode() const override;
	void flushLines() override;

private:
	RenderSystem& mRenderSystem;

	int m_debugMode;

	btAlignedObjectArray<MyDebugVec3> m_linePoints;
	btAlignedObjectArray<unsigned int> m_lineIndices;

	btVector3 m_currentLineColor;
	DefaultColors m_ourColors;
};
