#include "Defines.h"

Defines* Defines::mDefinesInstance{nullptr};
std::mutex Defines::mDefinesMutex;

Defines* Defines::GetInstance()
{
	std::lock_guard<std::mutex> lock(mDefinesMutex);
	if (mDefinesInstance == nullptr)
	{
		mDefinesInstance = new Defines();
	}
	return mDefinesInstance;
}

bool Defines::GetDebugDraw() const
{
	return mDebugDraw;
}

void Defines::SetDebugDraw(bool debugDraw)
{
	mDebugDraw = debugDraw;
}

bool Defines::GetEditMode() const
{
	return mEditMode;
}

void Defines::SetEditMode(bool editMode)
{
	mEditMode = editMode;
}
