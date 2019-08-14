#pragma once
#include "./include/glm/gtc/type_ptr.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"
#include "OpenAssetImportMesh.h"
#include "CollisionBox.h"
class obstacle
{
public:
	obstacle(glm::vec3 pos);
	~obstacle();
	
	void setPosition(glm::vec3 pos);
	
	glm::vec3 getPosition();
	
	COpenAssetImportMesh* getMesh();
	void Render();
	CollisionBox getCollider();
private:
	glm::vec3 m_position;
	COpenAssetImportMesh *m_Mesh;
	CollisionBox *collider;

};

