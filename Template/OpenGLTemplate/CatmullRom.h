#pragma once
#include "Common.h"
#include "vertexBufferObject.h"
#include "vertexBufferObjectIndexed.h"
#include "Texture.h"


class CCatmullRom
{
public:
	CCatmullRom();
	~CCatmullRom();

	void CreateCentreline();
	void RenderCentreline();

	void CreateOffsetCurves();
	void RenderOffsetCurves();

	void CreateTrack();
	void RenderTrack();
	void RenderBorders();
	void CreateBorders(float heightOffset);//add the verticies for the borders of the track
	void SetObstacles();
	glm::vec3 getObstacle(int i);	// return an obstacle

	int CurrentLap(float d); // Return the currvent lap (starting from 0) based on distance along the control curve.

	bool Sample(float d, glm::vec3 &p, glm::vec3 &up = glm::vec3(0, 0, 0)); // Return a point on the centreline based on a certain distance along the control curve.
	vector<glm::vec3> conePositions;		// Positions of the cones

	
private:
	vector<glm::vec3> m_obstacles;		//positions of the barrles
	void SetControlPoints();
	void ComputeLengthsAlongControlPoints();
	void UniformlySampleControlPoints(int numSamples);
	glm::vec3 Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t);


	vector<float> m_distances;
	CTexture m_texture;

	GLuint m_vaoCentreline;
	GLuint m_vaoLeftOffsetCurve;
	GLuint m_vaoRightOffsetCurve;
	GLuint m_vaoTrack;
	GLuint m_vaoRBorder;
	GLuint m_vaoLBorder;

	vector<glm::vec3> m_controlPoints;		// Control points, which are interpolated to produce the centreline points
	vector<glm::vec3> m_controlUpVectors;	// Control upvectors, which are interpolated to produce the centreline upvectors
	vector<glm::vec3> m_centrelinePoints;	// Centreline points
	vector<glm::vec3> m_centrelineUpVectors;// Centreline upvectors

	vector<glm::vec3> m_leftOffsetPoints;	// Left offset curve points
	vector<glm::vec3> m_rightOffsetPoints;	// Right offset curve points
	vector<glm::vec3> m_rBorder;			// Right border
	vector<glm::vec3> m_lBorder;			// Left border
	vector<glm::vec2> rTexCoords;			// Right texture coordinates
	vector<glm::vec2> lTexCoords;			// Left texture coordinates
	

	float m_width;

	unsigned int m_vertexCount;				// Number of vertices in the track VBO
};
