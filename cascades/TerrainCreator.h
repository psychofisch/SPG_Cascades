#pragma once

#include <climits>
#include <iostream>
#include <omp.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "RNGesus.h"

typedef uint32_t uint;

class TerrainCreator
{
public:
	struct vec3i {
		uint x, y, z;

		vec3i(){}
		vec3i(int px, int py, int pz)
		{
			x = px;
			y = py;
			z = pz;
		}
	};

	enum Mode {
		MODE_2D,
		MODE_3D
	};

	TerrainCreator(int width, int height, int depth, int seed);
	~TerrainCreator();

	float* getTerrainData();
	float getTerrainDataAt(vec3i position);
	int getNumberOfVertices(bool cube = false);
	void getVertices(GLfloat* verticesOut, bool cube = false);
	GLfloat* getCubeVertices();
	vec3i getDimensions();
	GLuint getCubeBuffer();
	GLuint getFeedbackBuffer();
	size_t getFeedbackSize();
	GLfloat* feedbackDataPtr();
	GLuint feedbackToVAO(size_t capturedPrimitives);
	GLuint getFeedbackVAO();
	GLuint getVAOSize();
	void initFeedback();

	float* createTerrain();

private:
	Mode m_mode;
	float* m_terrain;
	vec3i m_dimension;
	size_t m_size;
	RNGesus* m_rng;
	GLuint m_terrainPlainBuffer,
		m_terrainFeedbackBuffer,
		m_terrainFeedBackVAO,
		m_feedbackVBO;
	GLfloat* m_terrainFeedbackData;
	GLfloat* m_cubeData;
	size_t m_feedbackDataSize,
		m_capturedPrimitives;
};

