#include <sstream>

#include "Mesh.h"

Mesh::Mesh(const std::string& path, std::string fileName) : mId(std::move(fileName))
{
	_LoadObjModel(path);
}

void Mesh::_LoadObjModel(const std::string& path)
{
	std::ifstream file(path);
	if (!file.is_open())
	{
		std::cerr << "Error opening file: " << path << std::endl;
		return;
	}

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string type;
		iss >> type;

		if (type == "v")
		{
			TexturedVertex vertex;
			iss >> vertex.position.x >> vertex.position.y >> vertex.position.z;
			mVertices.push_back(vertex);
		}
		else if (type == "vt")
		{
			// Handle texture coordinates if needed
		}
		else if (type == "vn")
		{
			// Handle normals if needed
		}
		else if (type == "f")
		{
			std::string vertexData;
			while (iss >> vertexData)
			{
				std::istringstream vertexIss(vertexData);
				std::string indexStr;
				int index;
				std::getline(vertexIss, indexStr, '/');
				std::istringstream(indexStr) >> index;
				std::getline(vertexIss, indexStr, '/');
				if (!indexStr.empty())
				{
					//std::istringstream(indexStr) >> index.texCoordIndex;
				}
				std::getline(vertexIss, indexStr, '/');
				if (!indexStr.empty())
				{
					//std::istringstream(indexStr) >> index.normalIndex;
				}
				mIndices.push_back(index);
			}
		}
	}
}
