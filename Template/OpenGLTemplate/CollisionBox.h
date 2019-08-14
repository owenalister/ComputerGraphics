#pragma once
#include "Common.h"
class CollisionBox
{
private:
	glm::vec3 offset,scale;
	glm::vec3 *position;
public:
	CollisionBox(glm::vec3 *pos);
	CollisionBox(glm::vec3 s);
	CollisionBox(glm::vec3 p, glm::vec3 s);
	~CollisionBox();
	bool isColliding(CollisionBox &other);
	void updatePosition(glm::vec3 p);
	
	
};

