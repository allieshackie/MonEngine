#pragma once

#include <random>
#include <tiny_gltf.h>
#include<glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Graphics/Animation/Animation.h"

namespace gltfHelpers
{
	template <typename T>
	static T Lerp(T a, T b, float t)
	{
		return a + t * (b - a);
	}

	// Function to extract the transformation matrix of a glTF node
	static glm::mat4 GetNodeTransform(const tinygltf::Node& gltfNode)
	{
		auto transform = glm::mat4(1.0f); // Default to identity matrix

		// Handle the node's transformation (translation, rotation, scale) or matrix
		if (!gltfNode.matrix.empty())
		{
			transform = glm::make_mat4(gltfNode.matrix.data());
		}
		else
		{
			// If the node uses translation/rotation/scale
			if (!gltfNode.translation.empty())
			{
				auto translation = glm::vec3(
					static_cast<float>(gltfNode.translation[0]),
					static_cast<float>(gltfNode.translation[1]),
					static_cast<float>(gltfNode.translation[2])
				);
				transform = glm::translate(transform, translation);
			}

			if (!gltfNode.rotation.empty())
			{
				auto rotation = glm::quat(
					static_cast<float>(gltfNode.rotation[3]), // w
					static_cast<float>(gltfNode.rotation[0]), // x
					static_cast<float>(gltfNode.rotation[1]), // y
					static_cast<float>(gltfNode.rotation[2]) // z
				);
				transform *= glm::mat4_cast(rotation);
			}

			if (!gltfNode.scale.empty())
			{
				auto scale = glm::vec3(
					static_cast<float>(gltfNode.scale[0]),
					static_cast<float>(gltfNode.scale[1]),
					static_cast<float>(gltfNode.scale[2])
				);
				transform = glm::scale(transform, scale);
			}
		}

		return transform;
	}

	static glm::vec4 _GenerateRandomColor()
	{
		// Create a random number generator
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0.0, 1.0);

		return {
			static_cast<float>(dis(gen)), static_cast<float>(dis(gen)), static_cast<float>(dis(gen)),
			1.0f
		};
	}

	static std::vector<float> GetKeyframeTimes(const tinygltf::Accessor& accessor, const unsigned char* buffer)
	{
		std::vector<float> times;
		auto timeData = reinterpret_cast<const float*>(buffer);
		for (size_t i = 0; i < accessor.count; ++i)
		{
			times.push_back(timeData[i]);
		}
		return times;
	}

	static std::vector<glm::vec3> GetVec3Keyframes(const tinygltf::Accessor& accessor, const unsigned char* buffer)
	{
		std::vector<glm::vec3> values;
		auto data = reinterpret_cast<const float*>(buffer);
		for (size_t i = 0; i < accessor.count; ++i)
		{
			glm::vec3 value(data[i * 3 + 0], data[i * 3 + 1], data[i * 3 + 2]);
			values.push_back(value);
		}
		return values;
	}

	static std::vector<glm::quat> GetQuatKeyframes(const tinygltf::Accessor& accessor, const unsigned char* buffer)
	{
		std::vector<glm::quat> values;
		auto data = reinterpret_cast<const float*>(buffer);
		for (size_t i = 0; i < accessor.count; ++i)
		{
			glm::quat value(data[i * 4 + 3], data[i * 4 + 0], data[i * 4 + 1], data[i * 4 + 2]); // w, x, y, z
			values.push_back(value);
		}
		return values;
	}

	template <typename T>
	static T GetgltfBuffer(const tinygltf::Model& model, int attributeIndex, int& accessorCount)
	{
		const tinygltf::Accessor& accessor = model.accessors[attributeIndex];
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

		accessorCount = static_cast<int>(accessor.count);

		return reinterpret_cast<T>(&(buffer.data[bufferView.byteOffset + accessor.byteOffset]));
	}

	template <typename T>
	static T GetgltfBuffer(const tinygltf::Model& model, int attributeIndex)
	{
		const tinygltf::Accessor& accessor = model.accessors[attributeIndex];
		const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];

		return reinterpret_cast<T>(&(buffer.data[bufferView.byteOffset + accessor.byteOffset]));
	}
};
