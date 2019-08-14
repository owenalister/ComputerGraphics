#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"

class face
{
public:
	face::face() {};
	int index;
	int size;
	vector<glm::vec3> verticies;
	vector<glm::vec2> tex;
	vector<glm::vec3> normals;
	void calcNormal() {
		glm::vec3 v1 = verticies[0] - verticies[1];
		glm::vec3 v2 = verticies[0] - verticies[2];
		for (auto n : normals)
		{
			n = glm::cross(v1, v2);
		}
		
	}
};

// Class for generating a unit cube
class CCube
{
public:
	CCube();
	~CCube();
	void Create(string filename);
	void Render();
	void Release();
private:
	GLuint m_vao;
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	vector<face> faces;
};

