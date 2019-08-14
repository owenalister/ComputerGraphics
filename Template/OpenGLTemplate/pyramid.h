#pragma once
#include "Common.h"
#include "Texture.h"
#include "VertexBufferObject.h"
#include "VertexBufferObjectIndexed.h"
#include "pyramid.h"

#define _USE_MATH_DEFINES
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

#include "Sphere.h"
#include <math.h>

class pVertex {
public:
	glm::vec3 pos;
	glm::vec3 colour;
	glm::vec3 normal;
	glm::vec2 tex;
	pVertex(glm::vec3 p, glm::vec3 c, glm::vec2 t) :pos(p), colour(c), tex(t) {};
	glm::vec3 getPos() { return pos; };
	
};


class polygon
{
public:
	vector<pVertex> verts;
	glm::vec3 normal;
	polygon() {};
	void calcNormal() {
		glm::vec3 v1 = verts[0].pos- verts[1].pos;
		glm::vec3 v2 = verts[0].pos - verts[2].pos;
		normal = glm::cross(v1, v2);
	}
};

class pFace
{
public:
	pFace::pFace() {};
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


class pyramid
{
public:
	pyramid();
	~pyramid();
	void pyramid::Create(string filename);
	vector<pVertex> verticies;
	
	void render();
	CVertexBufferObject m_vbo;
	CTexture m_texture;
	UINT m_pvao;
	vector<pFace> faces;
};


