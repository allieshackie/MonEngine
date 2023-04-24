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

	static constexpr char MAP_PATH[] = "../Data/Maps/";
	static constexpr char LEVEL_PATH[] = "../Data/Levels/";
protected:
	Defines() = default;
	~Defines() = default;

private:
	static Defines* mDefinesInstance;
	static std::mutex mDefinesMutex;

	bool mDebugDraw = false;
	bool mEditMode = false;
};
