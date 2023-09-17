#pragma once
#include <nlohmann/json.hpp>
#include <fstream>

namespace FileSystem
{
	static nlohmann::basic_json<> ReadJson(const std::string& fileName)
	{
		std::ifstream ifs(fileName.c_str());
		return nlohmann::json::parse(ifs, nullptr, false, true);
	}
}
