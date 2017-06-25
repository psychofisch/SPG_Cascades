#include "TerrainCreator.h"

TerrainCreator::TerrainCreator(int width, int height, int depth, int seed)
	:m_cubeData(nullptr),
	m_terrainPlainBuffer(UINT_MAX),
	m_terrainFeedBackVAO(UINT_MAX),
	m_capturedPrimitives(0)
{
	m_size = width * height * depth;
	m_terrain = new float[m_size]{0.0f};
	m_dimension = vec3i(width, height, depth);
	m_rng = new RNGesus(seed, seed * 13, seed * 19);
}

TerrainCreator::~TerrainCreator()
{
	delete m_rng;
	delete[] m_terrain;
	delete[] m_terrainFeedbackData;
}

float* TerrainCreator::getTerrainData()
{
	return m_terrain;
}

float TerrainCreator::getTerrainDataAt(vec3i p)
{
	return m_terrain[p.x + p.y * m_dimension.x + p.z * m_dimension.x * m_dimension.y];
}

size_t TerrainCreator::getNumberOfVertices(bool cube)
{
	if (cube)
	{
		return m_size;
	}

	size_t vSize = 0;
	for (uint i = 0; i < m_size; ++i)
	{
		if (m_terrain[i] == 1.0f)
			vSize += 3;	//3 vertices for every point
	}

	return vSize;///8
}

void TerrainCreator::getVertices(GLfloat* verticesOut, bool cube)
{
	int vCount = 0;

	for (uint z = 0; z < m_dimension.z; z += 1)
	{
		for (uint y = 0; y < m_dimension.y; y += 1)
		{
			for (uint x = 0; x < m_dimension.x; x += 1)
			{
				uint pos = x + y*m_dimension.x + z*(m_dimension.y * m_dimension.x);
				if (cube || m_terrain[pos] == 1.0f)
				{
					verticesOut[vCount] = x - (m_dimension.x * 0.5f);
					verticesOut[vCount + 1] = y - (m_dimension.y * 0.5f);
					verticesOut[vCount + 2] = z - (m_dimension.z * 0.5f);
					vCount += 3;
				}
			}
		}
	}
}

GLfloat* TerrainCreator::getCubeVertices()
{
	if (m_cubeData == nullptr)
	{
		std::cout << __FUNCTION__ << ": generating m_cubeData (" << m_size * sizeof(GLfloat) << " KByte)\n";
		m_cubeData = new GLfloat[m_size * 3];
		getVertices(m_cubeData, true);
	}

	return m_cubeData;
}

TerrainCreator::vec3i TerrainCreator::getDimensions()
{
	return m_dimension;
}

GLuint TerrainCreator::getCubeBuffer()
{
	if (m_terrainPlainBuffer == UINT_MAX)
	{
		GLuint vbo;

		glGenVertexArrays(1, &m_terrainPlainBuffer);
		glBindVertexArray(m_terrainPlainBuffer);

		//VBO
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, getNumberOfVertices(true) * sizeof(GLfloat) * 3, getCubeVertices(), GL_STATIC_DRAW);

		// Position attribute
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

		//CLEANUP
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		std::cout << __FUNCTION__ << ": generating m_terrainPlainBuffer on GPU (" << getNumberOfVertices(true) * sizeof(GLfloat) << " KByte)\n";
	}

	return m_terrainPlainBuffer;
}

GLuint TerrainCreator::getFeedbackBuffer()
{
	return m_terrainFeedbackBuffer;
}

size_t TerrainCreator::getFeedbackSize()
{
	return m_feedbackDataSize;
}

GLfloat * TerrainCreator::feedbackDataPtr()
{
	return m_terrainFeedbackData;
}

GLuint TerrainCreator::feedbackToVAO(size_t capturedPrimitives)
{
	m_capturedPrimitives = capturedPrimitives;

	if (m_terrainFeedBackVAO != UINT_MAX)
	{
		glDeleteVertexArrays(1, &m_terrainFeedBackVAO);
		glDeleteBuffers(1, &m_feedbackVBO);
	}

	glGenVertexArrays(1, &m_terrainFeedBackVAO);
	glBindVertexArray(m_terrainFeedBackVAO);

	//VBO
	glGenBuffers(1, &m_feedbackVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_feedbackVBO);
	glBufferData(GL_ARRAY_BUFFER, capturedPrimitives * sizeof(GLfloat) * 6 * 3, m_terrainFeedbackData, GL_STATIC_DRAW);

	// Position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);

	// Normal attribute
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	//CLEANUP
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return m_terrainFeedBackVAO;
}

GLuint TerrainCreator::getFeedbackVAO()
{
	return m_terrainFeedBackVAO;
}

GLuint TerrainCreator::getVAOSize()
{
	return m_capturedPrimitives * 6 * 3;
}

void TerrainCreator::initFeedback()
{
	m_feedbackDataSize = sizeof(GL_FLOAT) * 6 * m_size * 3;
	glGenBuffers(1, &m_terrainFeedbackBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_terrainFeedbackBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_feedbackDataSize, nullptr, GL_STATIC_READ);
	m_terrainFeedbackData = new GLfloat[m_feedbackDataSize]{ 0 };

	std::cout << __FUNCTION__ << ": generating m_feedbackBuffer and m_terrainFeedbackData (" << m_feedbackDataSize/3 << " KByte)\n";
}

void TerrainCreator::smoothFeedbackData(GLuint primitives)
{
	//GLfloat* sameVertex = new GLfloat[3];
	glm::vec3 sameVertex;
	size_t* affectedVertices = new size_t[primitives * 6];
	size_t count = 0;
	for (size_t i = 0; i < primitives * 18; i += 6)
	{
		affectedVertices[0] = i;
		sameVertex.x = m_terrainFeedbackData[i + 3];
		sameVertex.y = m_terrainFeedbackData[i + 4];
		sameVertex.z = m_terrainFeedbackData[i + 5];
		count = 1;

		for (size_t j = i; j <  primitives * 18; j += 6)
		{
			if (m_terrainFeedbackData[i] == m_terrainFeedbackData[j]
				&& m_terrainFeedbackData[i+1] == m_terrainFeedbackData[j +1]
				&& m_terrainFeedbackData[i+2] == m_terrainFeedbackData[j + 2])
			{
				affectedVertices[count] = j;
				sameVertex.x += m_terrainFeedbackData[j + 3];
				sameVertex.y += m_terrainFeedbackData[j + 4];
				sameVertex.z += m_terrainFeedbackData[j + 5];
				count++;
			}
		}

		/*sameVertex[0] /= count;
		sameVertex[1] /= count;
		sameVertex[2] /= count;*/
		sameVertex = glm::normalize(sameVertex);

		m_terrainFeedbackData[i + 3] = sameVertex[0];
		m_terrainFeedbackData[i + 4] = sameVertex[1];
		m_terrainFeedbackData[i + 5] = sameVertex[2];
	}

	//delete[] sameVertex;
	delete[] affectedVertices;
}

float* TerrainCreator::createTerrain()
{
	double diagonal = glm::sqrt(glm::pow(m_dimension.x, 2) + glm::pow(m_dimension.z, 2) + glm::pow(m_dimension.y, 2));

	//blobs
	{
		glm::vec3 pillars[3];
		pillars[0] = glm::vec3(m_dimension.x * m_rng->getZeroToOne(), m_dimension.y * m_rng->getZeroToOne(), m_dimension.z * m_rng->getZeroToOne());
		pillars[1] = glm::vec3(m_dimension.x * m_rng->getZeroToOne(), m_dimension.y * m_rng->getZeroToOne(), m_dimension.z * m_rng->getZeroToOne());
		pillars[2] = glm::vec3(m_dimension.x * m_rng->getZeroToOne(), m_dimension.y * m_rng->getZeroToOne(), m_dimension.z * m_rng->getZeroToOne());
		
		float zCenter = m_dimension.z * 0.5f;
		float xCenter = m_dimension.x * 0.5f;

#pragma omp parallel for
		for (int y = 0; y < m_dimension.y; ++y)
		{
			float sinAdd = glm::sin(float(y) / m_dimension.y * 20);
			for (uint z = 0; z < m_dimension.z; ++z)
			{
				float zDistance2Wall = glm::abs(zCenter - z) / m_dimension.z;
				for (uint x = 0; x < m_dimension.x; ++x)
				{
					float xDistance2Wall = glm::abs(xCenter - x) / m_dimension.x;
					uint pos = x + y*m_dimension.x + z*(m_dimension.y * m_dimension.x);
					glm::vec3 tmpVec(x, y, z);
					glm::vec3 pillarVec[3];
					pillarVec[0] = tmpVec - pillars[0];
					pillarVec[1] = tmpVec - pillars[1];
					pillarVec[2] = tmpVec - pillars[2];
					float f = diagonal / glm::length(pillarVec[0]);
					f += diagonal / glm::length(pillarVec[1]);
					f += diagonal / glm::length(pillarVec[2]);
					f *= 0.3f;
					m_terrain[pos] = f + sinAdd - zDistance2Wall - xDistance2Wall;
					//if (f > 4.f)
					//{
					//	m_terrain[pos] = 1.0f;
					//	//std::cout << "#";
					//}
					//else
					//{
					//	m_terrain[pos] = 0.0f;
					//	//std::cout << " ";
					//}
				}
				//std::cout << std::endl;
			}
			//std::cout << std::endl;
		}
	}

	return m_terrain;
}
