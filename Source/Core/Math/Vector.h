#pragma once
#include <glm/vec3.hpp>

namespace MonDev
{
	class Vector3 : public glm::vec3
	{
		Vector3 operator+(float value) const
		{
			Vector3 vec;
			vec.x = this->x + value;
			vec.y = this->y + value;
			vec.z = this->z + value;
			return vec;
		}

		Vector3 operator-(float value) const
		{
			Vector3 vec;
			vec.x = this->x - value;
			vec.y = this->y - value;
			vec.z = this->z - value;
			return vec;
		}

		Vector3 operator*(float value) const
		{
			Vector3 vec;
			vec.x = this->x * value;
			vec.y = this->y * value;
			vec.z = this->z * value;
			return vec;
		}

		Vector3 operator/(float value) const
		{
			Vector3 vec;
			vec.x = this->x / value;
			vec.y = this->y / value;
			vec.z = this->z / value;
			return vec;
		}
	};
}
