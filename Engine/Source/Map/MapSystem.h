#pragma once
class Map;

class MapSystem
{
public:
	MapSystem(std::string mapsFolderPath);
	~MapSystem() = default;

	void CreateMap(const std::string& mapPath, glm::vec3 pos, glm::vec3 rotation, float tileSize);
	void CreateMapEditMode(const std::string& mapPath);
	const std::vector<std::shared_ptr<Map>>& GetAllMaps();
	std::shared_ptr<Map> GetCurrentMap();

	void RegisterOnCreateCallback(std::function<void(std::shared_ptr<Map>&)> cb);
	void RegisterOnUpdateCallback(std::function<void()> cb);

	void CloseCurrentMap();

private:
	std::vector<std::shared_ptr<Map>> mMaps;

	std::function<void(std::shared_ptr<Map>&)> mOnCreateCallback;
	std::function<void()> mOnUpdateCallback;

	int mCurrentFocusedMapIndex = -1;

	std::string mMapsFolderPath;
};
