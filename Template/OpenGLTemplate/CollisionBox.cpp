#include "CollisionBox.h"



CollisionBox::CollisionBox(glm::vec3* pos):offset(glm::vec3(0,0,0)), scale(glm::vec3(1,1,1))
{
	position = pos;
}


CollisionBox::~CollisionBox()
{
}

bool CollisionBox::isColliding(CollisionBox &other)
{
	
	return
		offset.x - scale.x / 2 <= other.offset.x + other.scale.x / 2 && other.offset.x - other.scale.x / 2 <= offset.x + scale.x / 2 &&
		offset.y - scale.y / 2 <= other.offset.y + other.scale.y / 2 && other.offset.y - other.scale.y / 2 <= offset.y + scale.y / 2 &&
		offset.z - scale.z / 2 <= other.offset.z + other.scale.z / 2 && other.offset.z - other.scale.z / 2 <= offset.z + scale.z / 2;
}

void CollisionBox::updatePosition(glm::vec3 p)
{
	offset = p;
}

