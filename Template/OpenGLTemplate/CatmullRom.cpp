#include "CatmullRom.h"
#define _USE_MATH_DEFINES
#include <math.h>



CCatmullRom::CCatmullRom()
{
	m_vertexCount = 0;
	m_width = 22.0f;
	m_texture.Load("resources\\textures\\road.png");
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_S, GL_REPEAT);
	m_texture.SetSamplerObjectParameter(GL_TEXTURE_WRAP_T, GL_REPEAT);
}

CCatmullRom::~CCatmullRom()
{}

// Perform Catmull Rom spline interpolation between four points, interpolating the space between p1 and p2
glm::vec3 CCatmullRom::Interpolate(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2, glm::vec3 &p3, float t)
{
	float t2 = t * t;
	float t3 = t2 * t;

	glm::vec3 a = p1;
	glm::vec3 b = 0.5f * (-p0 + p2);
	glm::vec3 c = 0.5f * (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3);
	glm::vec3 d = 0.5f * (-p0 + 3.0f*p1 - 3.0f*p2 + p3);

	return a + b*t + c*t2 + d*t3;

}


void CCatmullRom::SetControlPoints()
{
	// Set control points (m_controlPoints) here, or load from disk
	m_controlPoints.push_back(glm::vec3(100, 5, 0));
	m_controlPoints.push_back(glm::vec3(171, 5, 150));
	m_controlPoints.push_back(glm::vec3(0, 5, 100));
	m_controlPoints.push_back(glm::vec3(-121, 5, 71));
	m_controlPoints.push_back(glm::vec3(-140, 5, 0));
	m_controlPoints.push_back(glm::vec3(-71, 5, -71));
	m_controlPoints.push_back(glm::vec3(0, 5, -100));
	m_controlPoints.push_back(glm::vec3(71, 5, -71));
	// Optionally, set upvectors (m_controlUpVectors, one for each control point as well)

	
}
void CCatmullRom::SetObstacles() {
	// set positions of barrels
	m_obstacles.push_back(glm::vec3(-71, 5, -71));
	
}

// Determine lengths along the control points, which is the set of control points forming the closed curve
void CCatmullRom::ComputeLengthsAlongControlPoints()
{
	int M = (int)m_controlPoints.size();

	float fAccumulatedLength = 0.0f;
	m_distances.push_back(fAccumulatedLength);
	for (int i = 1; i < M; i++) {
		fAccumulatedLength += glm::distance(m_controlPoints[i - 1], m_controlPoints[i]);
		m_distances.push_back(fAccumulatedLength);
	}

	// Get the distance from the last point to the first
	fAccumulatedLength += glm::distance(m_controlPoints[M - 1], m_controlPoints[0]);
	m_distances.push_back(fAccumulatedLength);
}


// Return the point (and upvector, if control upvectors provided) based on a distance d along the control polygon
bool CCatmullRom::Sample(float d, glm::vec3 &p, glm::vec3 &up)
{
	if (d < 0)
		return false;

	int M = (int)m_controlPoints.size();
	if (M == 0)
		return false;


	float fTotalLength = m_distances[m_distances.size() - 1];

	// The the current length along the control polygon; handle the case where we've looped around the track
	float fLength = d - (int)(d / fTotalLength) * fTotalLength;

	// Find the current segment
	int j = -1;
	for (int i = 0; i < (int)m_distances.size(); i++) {
		if (fLength >= m_distances[i] && fLength < m_distances[i + 1]) {
			j = i; // found it!
			break;
		}
	}

	if (j == -1)
		return false;

	// Interpolate on current segment -- get t
	float fSegmentLength = m_distances[j + 1] - m_distances[j];
	float t = (fLength - m_distances[j]) / fSegmentLength;

	// Get the indices of the four points along the control polygon for the current segment
	int iPrev = ((j - 1) + M) % M;
	int iCur = j;
	int iNext = (j + 1) % M;
	int iNextNext = (j + 2) % M;

	// Interpolate to get the point (and upvector)
	p = Interpolate(m_controlPoints[iPrev], m_controlPoints[iCur], m_controlPoints[iNext], m_controlPoints[iNextNext], t);
	if (m_controlUpVectors.size() == m_controlPoints.size())
		up = glm::normalize(Interpolate(m_controlUpVectors[iPrev], m_controlUpVectors[iCur], m_controlUpVectors[iNext], m_controlUpVectors[iNextNext], t));


	return true;
}



// Sample a set of control points using an open Catmull-Rom spline, to produce a set of iNumSamples that are (roughly) equally spaced
void CCatmullRom::UniformlySampleControlPoints(int numSamples)
{
	glm::vec3 p, up;

	// Compute the lengths of each segment along the control polygon, and the total length
	ComputeLengthsAlongControlPoints();
	float fTotalLength = m_distances[m_distances.size() - 1];

	// The spacing will be based on the control polygon
	float fSpacing = fTotalLength / numSamples;

	// Call PointAt to sample the spline, to generate the points
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);

	}


	// Repeat once more for truly equidistant points
	m_controlPoints = m_centrelinePoints;
	m_controlUpVectors = m_centrelineUpVectors;
	m_centrelinePoints.clear();
	m_centrelineUpVectors.clear();
	m_distances.clear();
	ComputeLengthsAlongControlPoints();
	fTotalLength = m_distances[m_distances.size() - 1];
	fSpacing = fTotalLength / numSamples;
	for (int i = 0; i < numSamples; i++) {
		Sample(i * fSpacing, p, up);
		m_centrelinePoints.push_back(p);
		if (m_controlUpVectors.size() > 0)
			m_centrelineUpVectors.push_back(up);
	}


}

glm::vec3 CCatmullRom::getObstacle(int i)
{
	
	return m_obstacles[i];
}


void CCatmullRom::CreateCentreline()
{
	// Call Set Control Points
	SetControlPoints();
	SetObstacles();
	// Call UniformlySampleControlPoints with the number of samples required
	UniformlySampleControlPoints(500);
	// Create a VAO called m_vaoCentreline and a VBO to get the points onto the graphics card
	glGenVertexArrays(1, &m_vaoCentreline);
	glBindVertexArray(m_vaoCentreline);
	CVertexBufferObject cvbo;
	cvbo.Create();
	cvbo.Bind();
	glm::vec2 texCoord(0, 0);
	glm::vec3 normal(0, 1, 0);
	for (int i = 0; i < m_centrelinePoints.size(); i++)
	{
		cvbo.AddData(&m_centrelinePoints[i], sizeof(glm::vec3)); 
		cvbo.AddData(&texCoord, sizeof(glm::vec2));
		cvbo.AddData(&normal, sizeof(glm::vec3));
	}
	cvbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
}


void CCatmullRom::CreateOffsetCurves()
{
	// Compute the offset curves, one left, and one right.  Store the points in m_leftOffsetPoints and m_rightOffsetPoints respectively
	for (int i =0; i<m_centrelinePoints.size(); i++)
	{
		glm::vec3 p,pNext,tangent;

		if (i != m_centrelinePoints.size() - 1) 
		{
			p = m_centrelinePoints[i];
			pNext = m_centrelinePoints[i+1];
			tangent = pNext - p;
		}
		else
		{
			p = m_centrelinePoints[i];
			pNext = m_centrelinePoints[0];
			tangent = pNext-p;
		}

		glm::normalize(tangent);
		
		glm::vec3 normal = glm::cross(tangent,glm::vec3(0, 1, 0));
		glm::normalize(normal);
		m_leftOffsetPoints.push_back( p - (m_width / 2.0f)*normal);
		m_rightOffsetPoints.push_back( p + (m_width / 2.0f)*normal);
		conePositions.push_back(p - (m_width / 2.0f)*normal);
		conePositions.push_back(p + (m_width  / 2.0f)*normal);
		
	}
	// VAO & VBO for the left centreline
	glGenVertexArrays(1, &m_vaoLeftOffsetCurve);
	glBindVertexArray(m_vaoLeftOffsetCurve);
	CVertexBufferObject lvbo;
	lvbo.Create();
	lvbo.Bind();
	glm::vec2 texCoord(0, 0);
	glm::vec3 up(0, 1, 0);
	bool b = false;

	float count = 0;
	float maxCount = 21;
	bool countUp = true;
	for (int i = 0; i < m_leftOffsetPoints.size(); i++)
	{
		lvbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
		
		
		
		lvbo.AddData(&up, sizeof(glm::vec3));
		
			lvbo.AddData(&glm::vec2(0, count/ maxCount), sizeof(glm::vec2));
			lTexCoords.push_back(glm::vec2(0, count / maxCount));
			rTexCoords.push_back(glm::vec2(1, count / maxCount));
			if (countUp)
				count++;
			else
				count--;
			if (count == maxCount)
				countUp = false;
			if (countUp == 0)
				countUp = true;
	}
	lvbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));

	// VAO & VBO for the right centreline
	glGenVertexArrays(1, &m_vaoRightOffsetCurve);
	glBindVertexArray(m_vaoRightOffsetCurve);
	CVertexBufferObject rvbo;
	rvbo.Create();
	rvbo.Bind();
	//glm::vec2 texCoord(0, 0);
	b = false;
	count = 0;
	
	for (int i = 0; i < m_rightOffsetPoints.size(); i++)
	{
		rvbo.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
		
		rvbo.AddData(&glm::vec2(1, b), sizeof(glm::vec2));
		
		rvbo.AddData(&up, sizeof(glm::vec3));
		
			//rTexCoords.push_back(glm::vec2(1, count/ maxCount));
		
			
	
		count++;
		if (count == maxCount)
			count = 0;
	}
	rvbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei strideLeft = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideLeft, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, strideLeft, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, strideLeft, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
}


void CCatmullRom::CreateTrack()
{

	// Generate a VAO called m_vaoTrack and a VBO to get the offset curve points and indices on the graphics card
	glGenVertexArrays(1, &m_vaoTrack);
	glBindVertexArray(m_vaoTrack);
	CVertexBufferObject tvbo;
	tvbo.Create();
	tvbo.Bind();
	glm::vec3 up(0, 1, 0);
	

	//add verticies
	for (int i = 0; i < m_leftOffsetPoints.size(); i++)
	{
		tvbo.AddData(&m_leftOffsetPoints[i], sizeof(glm::vec3));
		tvbo.AddData(&lTexCoords[i], sizeof(glm::vec2));
		tvbo.AddData(&up, sizeof(glm::vec3));

		tvbo.AddData(&m_rightOffsetPoints[i], sizeof(glm::vec3));
		tvbo.AddData(&rTexCoords[i], sizeof(glm::vec2));
		tvbo.AddData(&up, sizeof(glm::vec3));
	}

	// join the end of the track with the begining
	tvbo.AddData(&m_leftOffsetPoints[0], sizeof(glm::vec3));
	tvbo.AddData(&lTexCoords.back(), sizeof(glm::vec2));
	tvbo.AddData(&up, sizeof(glm::vec3));
	tvbo.AddData(&m_rightOffsetPoints[0], sizeof(glm::vec3));
	tvbo.AddData(&rTexCoords.back(), sizeof(glm::vec2));
	tvbo.AddData(&up, sizeof(glm::vec3));

	m_vertexCount = (m_leftOffsetPoints.size() * 2) + 2;

	tvbo.UploadDataToGPU(GL_STATIC_DRAW);
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
}

void CCatmullRom::CreateBorders(float heightOffset)
{
	for (auto a : m_leftOffsetPoints)
	{
		m_lBorder.push_back(a);
		m_lBorder.push_back(a + glm::vec3(0, heightOffset, 0));
	}

	for (auto a : m_rightOffsetPoints)
	{
		m_rBorder.push_back(a);
		m_rBorder.push_back(a + glm::vec3(0, heightOffset, 0));

	}
	glGenVertexArrays(0, &m_vaoLBorder);
	glGenVertexArrays(0, &m_vaoRBorder);
	CVertexBufferObject lBVBO;
	CVertexBufferObject rBVBO;
	lBVBO.Create();
	lBVBO.Bind();
	rBVBO.Create();
	rBVBO.Bind();
	for (auto a : m_rBorder)
	{
		rBVBO.AddData(&a, sizeof(glm::vec3));
		rBVBO.AddData(&glm::vec2(0, 0), sizeof(glm::vec2));
		rBVBO.AddData(&glm::vec3(1,0,0), sizeof(glm::vec3));
	}
	for (auto a : m_lBorder)
	{
		lBVBO.AddData(&a, sizeof(glm::vec3));
		lBVBO.AddData(&glm::vec2(0, 0), sizeof(glm::vec2));
		lBVBO.AddData(&glm::vec3(1, 0, 0), sizeof(glm::vec3));
	}
	GLsizei stride = 2 * sizeof(glm::vec3) + sizeof(glm::vec2);
	lBVBO.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
	rBVBO.UploadDataToGPU(GL_STATIC_DRAW);
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(glm::vec3)
		+ sizeof(glm::vec2)));
	

}

void CCatmullRom::RenderCentreline()
{
	
	// Bind the VAO m_vaoCentreline and render it
	glBindVertexArray(m_vaoCentreline);
	glLineWidth(3.0f);
	glDrawArrays(GL_LINE_LOOP, 0, m_centrelinePoints.size());
	glDrawArrays(GL_POINT, 0, m_centrelinePoints.size());


}

void CCatmullRom::RenderBorders()
{
	glBindVertexArray(m_vaoRBorder);
	
	glLineWidth(3.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_rBorder.size());
	glBindVertexArray(m_vaoLBorder);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_lBorder.size());
}

void CCatmullRom::RenderOffsetCurves()
{
	
	// Bind the VAO m_vaoLeftOffsetCurve and render it
	glBindVertexArray(m_vaoLeftOffsetCurve);
	glLineWidth(3.0f);
	glDrawArrays(GL_LINE_LOOP, 0, m_leftOffsetPoints.size());
	glDrawArrays(GL_POINT, 0, m_leftOffsetPoints.size());


	// Bind the VAO m_vaoRightOffsetCurve and render it
	glBindVertexArray(m_vaoRightOffsetCurve);
	glLineWidth(3.0f);
	glDrawArrays(GL_LINE_LOOP, 0, m_rightOffsetPoints.size());
	glDrawArrays(GL_POINT, 0, m_rightOffsetPoints.size());
	
}


void CCatmullRom::RenderTrack()
{
	// Bind the VAO m_vaoTrack and render it
	glBindVertexArray(m_vaoTrack);
	m_texture.Bind();
	glLineWidth(3.0f);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, m_vertexCount);
	//RenderBorders();
}

int CCatmullRom::CurrentLap(float d)
{

	return (int)(d / m_distances.back());

}