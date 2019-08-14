#include "obstacle.h"



obstacle::obstacle(glm::vec3 pos) :m_position(pos)
{
	m_Mesh = new COpenAssetImportMesh;
	collider = new CollisionBox(&m_position);
}


obstacle::~obstacle()
{
	delete collider;
}


void obstacle::setPosition(glm::vec3 pos)
{
	m_position = pos;
}



glm::vec3 obstacle::getPosition()
{
	return m_position;
}

COpenAssetImportMesh* obstacle::getMesh()
{
	return m_Mesh;
}

void obstacle::Render()
{
		m_Mesh->Render();
}
CollisionBox obstacle::getCollider()
{
	return *collider;
}