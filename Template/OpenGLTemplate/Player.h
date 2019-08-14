#pragma once

#include "./include/glm/gtc/type_ptr.hpp"
#include "./include/glm/gtc/matrix_transform.hpp"
#include "OpenAssetImportMesh.h"
#include "Camera.h"
#include "CollisionBox.h"

class Player
{
public:
	Player(glm::vec3 pos);
	~Player();
	void SetCamera(CCamera *cam);
	void setPosition(glm::vec3 pos);
	void setRotation(glm::vec3 rot);
	glm::vec3 getPosition();
	glm::vec3 getRotation();
	COpenAssetImportMesh* getMesh();
	CCamera *p_camera;
	void Render();
	CollisionBox getCollider();
private:
	glm::vec3 m_position;
	glm::vec3 m_rotation;
	COpenAssetImportMesh *m_Mesh;
	CollisionBox *collider;
};

