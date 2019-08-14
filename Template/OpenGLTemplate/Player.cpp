#include "Player.h"



Player::Player(glm::vec3 pos):m_position(pos),m_rotation(glm::vec3(0,0,0))
{
	m_Mesh = new COpenAssetImportMesh;
	m_Mesh->Load("resources\\models\\Horse\\Horse2.obj");
	collider = new CollisionBox(&m_position);
}
void Player::SetCamera(CCamera *cam)
{
	p_camera = cam;
}

Player::~Player()
{
	delete collider;
}

void Player::setPosition(glm::vec3 pos)
{
	m_position = pos;
}

void Player::setRotation(glm::vec3 rot)
{
	m_rotation = rot;
}

glm::vec3 Player::getPosition()
{
	return m_position;
}

glm::vec3 Player::getRotation()
{
	return m_rotation;
}
COpenAssetImportMesh* Player::getMesh()
{
	return m_Mesh;
}

void Player::Render()
{
	if(p_camera->getMode()!=0	)
		m_Mesh->Render();
}

CollisionBox Player::getCollider()
{
	return *collider;
}