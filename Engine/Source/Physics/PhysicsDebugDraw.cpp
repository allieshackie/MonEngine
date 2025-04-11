#include "Graphics/RenderContext.h"

#include "PhysicsDebugDraw.h"

// TODO: Improve performance, batch lines 
PhysicsDebugDraw::PhysicsDebugDraw(RenderContext& renderContext)
	: mRenderContext(renderContext), m_debugMode(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawAabb),
	  m_currentLineColor(-1, -1, -1)
{
}

btIDebugDraw::DefaultColors PhysicsDebugDraw::getDefaultColors() const
{
	return m_ourColors;
}

///the default implementation for setDefaultColors has no effect. A derived class can implement it and store the colors.
void PhysicsDebugDraw::setDefaultColors(const DefaultColors& colors)
{
	m_ourColors = colors;
}

void PhysicsDebugDraw::drawLine(const btVector3& from1, const btVector3& to1, const btVector3& color1)
{
	//float from[4] = {from1[0],from1[1],from1[2],from1[3]};
	//float to[4] = {to1[0],to1[1],to1[2],to1[3]};
	//float color[4] = {color1[0],color1[1],color1[2],color1[3]};
	//m_glApp->m_instancingRenderer->drawLine(from,to,color);
	if (m_currentLineColor != color1 || m_linePoints.size() >= BT_LINE_BATCH_SIZE)
	{
		flushLines();
		m_currentLineColor = color1;
	}

	mRenderContext.DrawLine(glm::vec3(from1.x(), from1.y(), from1.z()), glm::vec3(to1.x(), to1.y(), to1.z()),
	                        glm::vec4(color1.x(), color1.y(), color1.z(), 255));

	/*
	 *
	MyDebugVec3 from(from1);
	MyDebugVec3 to(to1);

	m_linePoints.push_back(from);
	m_linePoints.push_back(to);

	m_lineIndices.push_back(m_lineIndices.size());
	m_lineIndices.push_back(m_lineIndices.size());
	 */
}

void PhysicsDebugDraw::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance,
                                        int lifeTime,
                                        const btVector3& color)
{
	drawLine(PointOnB, PointOnB + normalOnB * distance, color);
	btVector3 ncolor(0, 0, 0);
	drawLine(PointOnB, PointOnB + normalOnB * 0.01, ncolor);
}

void PhysicsDebugDraw::reportErrorWarning(const char* warningString)
{
}

void PhysicsDebugDraw::draw3dText(const btVector3& location, const char* textString)
{
}

void PhysicsDebugDraw::setDebugMode(int debugMode)
{
	m_debugMode = debugMode;
}

int PhysicsDebugDraw::getDebugMode() const
{
	return m_debugMode;
}

void PhysicsDebugDraw::flushLines()
{
	/*
	 *
	int sz = m_linePoints.size();
	if (sz)
	{
		float debugColor[4];
		debugColor[0] = m_currentLineColor.x();
		debugColor[1] = m_currentLineColor.y();
		debugColor[2] = m_currentLineColor.z();
		debugColor[3] = 1.f;
		m_glApp->m_renderer->drawLines(&m_linePoints[0].x, debugColor,
									   m_linePoints.size(), sizeof(MyDebugVec3),
									   &m_lineIndices[0],
									   m_lineIndices.size(),
									   1);
		m_linePoints.clear();
		m_lineIndices.clear();
	}
	 */
}
