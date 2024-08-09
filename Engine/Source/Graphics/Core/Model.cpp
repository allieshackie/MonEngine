#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include "assimp/postprocess.h"
#include "LLGL/Utils/Utility.h"

#include "Graphics/Core/Shader.h"
#include "Graphics/Core/Vertex.h"

#include "Model.h"

Model::Model(const std::string& path, std::string fileName) : mId(std::move(fileName))
{
	// Create an instance of the Importer class
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
	                                         aiProcessPreset_TargetRealtime_Quality);

	// Check if there was errors with 
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << importer.GetErrorString() << std::endl;
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[i];
		_LoadObjModel(mesh);
	}
}

void Model::InitializeBuffers(const LLGL::RenderSystemPtr& renderSystem, const Shader& shader)
{
	for (auto& mesh : mMeshes)
	{
		mesh.mVertexBuffer = renderSystem->CreateBuffer(
			LLGL::VertexBufferDesc(static_cast<std::uint32_t>(mesh.mVertices.size() * sizeof(Vertex)),
			                       shader.GetVertexFormat()), mesh.mVertices.data());

		mesh.mIndexBuffer = renderSystem->CreateBuffer(
			LLGL::IndexBufferDesc(static_cast<std::uint32_t>(mesh.mIndices.size() * sizeof(uint32_t)),
			                      LLGL::Format::R32UInt),
			mesh.mIndices.data());

		mesh.mNumIndices = static_cast<int>(mesh.mIndices.size());
	}
}

void Model::_LoadObjModel(aiMesh* mesh)
{
	MeshData meshData;
	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates
		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		// UVs coordinates
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoord.y = mesh->mTextureCoords[0][i].y;
		}
		else
		{
			vertex.texCoord.x = 0.0f;
			vertex.texCoord.y = 0.0f;
		}

		// Normals
		vertex.normal.x = mesh->mNormals[i].x;
		vertex.normal.y = mesh->mNormals[i].y;
		vertex.normal.z = mesh->mNormals[i].z;

		meshData.mVertices.push_back(vertex);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			meshData.mIndices.push_back(face.mIndices[j]);
		}
	}

	mMeshes.push_back(meshData);
}
