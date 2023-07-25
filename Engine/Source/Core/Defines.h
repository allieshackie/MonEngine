#pragma once
#include <mutex>

class Defines
{
public:
	Defines(Defines& other) = delete;

	void operator=(const Defines&) = delete;

	static Defines* GetInstance();

	bool GetDebugDraw() const;
	void SetDebugDraw(bool debugDraw);

	bool GetEditMode() const;
	void SetEditMode(bool editMode);

protected:
	Defines() = default;
	~Defines() = default;

private:
	static Defines* mDefinesInstance;
	static std::mutex mDefinesMutex;

	bool mDebugDraw = false;
	bool mEditMode = false;
};
