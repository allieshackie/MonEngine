#pragma once
#include <glm/mat4x4.hpp>
#include "Graphics/Animation/Animation.h"

struct MeshComponent
{
	// data driven
	std::string mMeshPath;
	bool mHasBones = false;

	// runtime
	int mMeshId = 0;
	std::vector<glm::mat4> mFinalTransforms;
	AnimationStates mPrevAnimState = AnimationStates::NONE;
	AnimationStates mCurrentAnimState = AnimationStates::IDLE;
	float mBlendFactor = 1.0f;

	// debug runtime
	int mCurrentBoneIndex = 0;
};
