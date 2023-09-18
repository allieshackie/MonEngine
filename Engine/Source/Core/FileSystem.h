#pragma once
#include <nlohmann/json.hpp>
#include <fstream>

namespace FileSystem
{
	static nlohmann::basic_json<> ReadJson(const std::string& filePath)
	{
		std::ifstream ifs(filePath.c_str());
		return nlohmann::json::parse(ifs, nullptr, false, true);
	}

	static std::vector<uint8_t> ReadBytes(const std::string& filePath)
	{
		std::ifstream file(filePath.c_str(), std::ios::binary | std::ios::ate);
		if (!file.is_open())
		{
			assert(false);
			return {};
		}

		const auto size = file.tellg();
		file.seekg(0, std::ios::beg);
		auto bytes = std::vector<uint8_t>(size);
		file.read(reinterpret_cast<char*>(&bytes[0]), size);
		file.close();

		return bytes;
	}
}
