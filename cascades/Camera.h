#pragma once

#include <glm\vec3.hpp>
#include <glm\gtx\quaternion.hpp>

class Camera
{
public:
	Camera();
	~Camera();

	glm::vec3 position;
	glm::quat rotation;
};

