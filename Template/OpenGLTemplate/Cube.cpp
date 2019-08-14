#include "Cube.h"
CCube::CCube()
{

	glm::vec3 v0(-1, -1, -1);
	glm::vec3 v1(1, -1, -1);
	glm::vec3 v2(-1, 1, -1);
	glm::vec3 v3(1, 1, -1);
	glm::vec3 v4(-1, -1, 1);
	glm::vec3 v5(1, -1, 1);
	glm::vec3 v6(-1, 1, 1);
	glm::vec3 v7(1, 1, 1);

	// front face
	face f1;
	f1.verticies.push_back(v4);
	f1.verticies.push_back(v5);
	f1.verticies.push_back(v6);
	f1.verticies.push_back(v7);
	f1.tex.push_back(glm::vec2(0, 0));
	f1.tex.push_back(glm::vec2(1, 0));
	f1.tex.push_back(glm::vec2(0, 1));
	f1.tex.push_back(glm::vec2(1, 1));
	f1.normals.push_back(glm::vec3(0, 0, 1));
	f1.normals.push_back(glm::vec3(0, 0, 1));
	f1.normals.push_back(glm::vec3(0, 0, 1));
	f1.normals.push_back(glm::vec3(0, 0, 1));
	f1.index = 0;
	f1.size = 4;
	faces.push_back(f1);

	//top face
	face f2;
	f2.verticies.push_back(v2);
	f2.verticies.push_back(v3);
	f2.verticies.push_back(v6);
	f2.verticies.push_back(v7);
	f2.tex.push_back(glm::vec2(0, 0));
	f2.tex.push_back(glm::vec2(1, 0));
	f2.tex.push_back(glm::vec2(0, 1));
	f2.tex.push_back(glm::vec2(1, 1));
	f2.normals.push_back(glm::vec3(0, 1, 0));
	f2.normals.push_back(glm::vec3(0, 1, 0));
	f2.normals.push_back(glm::vec3(0, 1, 0));
	f2.normals.push_back(glm::vec3(0, 1, 0));
	f2.index = 4;
	f2.size = 4;
	faces.push_back(f2);

	//rear face
	face f3;
	f3.verticies.push_back(v2);
	f3.verticies.push_back(v3);
	f3.verticies.push_back(v0);
	f3.verticies.push_back(v1);
	f3.tex.push_back(glm::vec2(0, 0));
	f3.tex.push_back(glm::vec2(1, 0));
	f3.tex.push_back(glm::vec2(0, 1));
	f3.tex.push_back(glm::vec2(1, 1));
	f3.normals.push_back(glm::vec3(0, 0, -1));
	f3.normals.push_back(glm::vec3(0, 0, -1));
	f3.normals.push_back(glm::vec3(0, 0, -1));
	f3.normals.push_back(glm::vec3(0, 0, -1));
	f3.index = 8;
	f3.size = 4;
	faces.push_back(f3);

	//bottom face
	face f4;
	f4.verticies.push_back(v0);
	f4.verticies.push_back(v1);
	f4.verticies.push_back(v4);
	f4.verticies.push_back(v5);
	f4.tex.push_back(glm::vec2(0, 0));
	f4.tex.push_back(glm::vec2(1, 0));
	f4.tex.push_back(glm::vec2(0, 1));
	f4.tex.push_back(glm::vec2(1, 1));
	f4.normals.push_back(glm::vec3(0, -1, 0));
	f4.normals.push_back(glm::vec3(0, -1, 0));
	f4.normals.push_back(glm::vec3(0, -1, 0));
	f4.normals.push_back(glm::vec3(0, -1, 0));
	f4.index = 12;
	f4.size = 4;
	faces.push_back(f4);

	// right face
	face f5;
	f5.verticies.push_back(v7);
	f5.verticies.push_back(v3);
	f5.verticies.push_back(v5);
	f5.verticies.push_back(v1);
	f5.tex.push_back(glm::vec2(0, 0));
	f5.tex.push_back(glm::vec2(1, 0));
	f5.tex.push_back(glm::vec2(0, 1));
	f5.tex.push_back(glm::vec2(1, 1));
	f5.normals.push_back(glm::vec3(1, 0, 0));
	f5.normals.push_back(glm::vec3(1, 0, 0));
	f5.normals.push_back(glm::vec3(1, 0, 0));
	f5.normals.push_back(glm::vec3(1, 0, 0));
	f5.index = 16;
	f5.size = 4;
	faces.push_back(f5);

	// left face
	face f6;
	f6.verticies.push_back(v6);
	f6.verticies.push_back(v2);
	f6.verticies.push_back(v4);
	f6.verticies.push_back(v0);
	f6.tex.push_back(glm::vec2(0, 0));
	f6.tex.push_back(glm::vec2(1, 0));
	f6.tex.push_back(glm::vec2(0, 1));
	f6.tex.push_back(glm::vec2(1, 1));
	f6.normals.push_back(glm::vec3(-1, 0, 0));
	f6.normals.push_back(glm::vec3(-1, 0, 0));
	f6.normals.push_back(glm::vec3(-1, 0, 0));
	f6.normals.push_back(glm::vec3(-1, 0, 0));
	f6.index = 20;
	f6.size = 4;
	faces.push_back(f6);

}
CCube::~CCube()
{
	Release();
}
void CCube::Create(string filename)
{
	m_texture.Load(filename);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER,
		GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	m_vbo.Create();
	m_vbo.Bind();
	// Write the code to add interleaved vertex attributes to the VBO
	for (int j = 0; j < faces.size(); j++) {
		for (int i = 0; i < faces[j].size; i++){
			m_vbo.AddData(&faces[j].verticies[i], sizeof(glm::vec3));
			m_vbo.AddData(&faces[j].tex[i], sizeof(glm::vec2));
			m_vbo.AddData(&faces[j].normals[i], sizeof(glm::vec3));
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
void CCube::Render()
{
	glBindVertexArray(m_vao);
	m_texture.Bind();
	// Call glDrawArrays to render each side
	
	for (auto f : faces)
	{
		glDrawArrays(GL_TRIANGLE_STRIP, f.index, f.size);
	}
	
	//glDrawArrays(GL_TRIANGLE_STRIP, faces[5].index, faces[2].size);
}
void CCube::Release()
{
	m_texture.Release();
	glDeleteVertexArrays(1, &m_vao);
	m_vbo.Release();
}