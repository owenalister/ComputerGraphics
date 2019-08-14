#include "pyramid.h"

pyramid::pyramid()
{

	//load texture 
	m_texture.Load("resources\\textures\\Tile41a.jpg");

	//create verticies 
	glm::vec3 v0(-1, 0, 1);
	glm::vec3 v1(1, 0, 1);
	glm::vec3 v2(-1, 0, -1);
	glm::vec3 v3(-1, 0, -1);
	glm::vec3 v4(0, 1, 0);

	pFace f0;
	f0.verticies.push_back(v0);
	f0.verticies.push_back(v1);
	f0.verticies.push_back(v2);
	f0.verticies.push_back(v3);
	f0.size = 4;
	f0.index = 0;
	/*
	pVertex v0(glm::vec3(-1, 0, 1), glm::vec3(1, 1, 1), glm::vec2(1, 0));
	pVertex v1(glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), glm::vec2(0, 0));
	pVertex v2(glm::vec3(-1, 0, -1), glm::vec3(1, 1, 1), glm::vec2(1, 0));
	pVertex v3(glm::vec3(1, 0, -1), glm::vec3(1, 1, 1), glm::vec2(0, 0));
	pVertex v4(glm::vec3(0, 1, 0), glm::vec3(1, 1, 1), glm::vec2(0.5, 1));

	//Create texture coordinates
	v0.tex = glm::vec2(1, 0);;
	v1.tex = glm::vec2(0, 0);;
	v2.tex = glm::vec2(1, 0);;
	v3.tex = glm::vec2(0, 0);;
	v4.tex = glm::vec2(0.5, 1);


	verticies.push_back(v0);
	verticies.push_back(v1);
	verticies.push_back(v2);
	verticies.push_back(v3);
	verticies.push_back(v4);
	//create polygons
	polygon f0, f1, f2, f3, f4, f5;

	//manage faces
	f0.verts.push_back(v1);
	f0.verts.push_back(v0);
	f0.verts.push_back(v4);

	f1.verts.push_back(v2);
	f1.verts.push_back(v1);
	f1.verts.push_back(v4);

	f2.verts.push_back(v4);
	f2.verts.push_back(v3);
	f2.verts.push_back(v2);

	f3.verts.push_back(v0);
	f3.verts.push_back(v3);
	f3.verts.push_back(v4);

	f4.verts.push_back(v0);
	f4.verts.push_back(v2);
	f4.verts.push_back(v3);

	f5.verts.push_back(v0);
	f5.verts.push_back(v1);
	f5.verts.push_back(v2);



	//add all the faces
	faces.push_back(f0);
	faces.push_back(f1);
	faces.push_back(f2);
	faces.push_back(f3);
	faces.push_back(f4);
	faces.push_back(f5);
	*/
	// calculate the normals of the faces

	for (auto f:faces) {
		f.calcNormal();
	}
	
}

void pyramid::Create(string filename) {

	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenVertexArrays(1, &m_pvao);
	glBindVertexArray(m_pvao);
	m_vbo.Create();
	m_vbo.Bind();
	// Write the code to add interleaved vertex attributes to the VBO
	for (auto f : faces)
	{
		for (int i = 0; i < f.size; i++) {

			m_vbo.AddData(&f.verticies[i], sizeof(glm::vec3));
			m_vbo.AddData(&f.tex[i], sizeof(glm::vec2));
			m_vbo.AddData(&f.normals[i], sizeof(glm::vec3));
		}


	}

	// Upload data to GPU
	m_vbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,
		(void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));
}

void pyramid::render()
{
	glBindVertexArray(m_pvao);
	m_texture.Bind();
	//glDisable(GL_CULL_FACE);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


pyramid::~pyramid()
{
}
