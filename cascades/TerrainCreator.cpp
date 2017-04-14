#include "TerrainCreator.h"

TerrainCreator::TerrainCreator(int width, int height, int depth)
{
	m_size = width * height * depth;
	m_terrain = new float[m_size]{0.0f};
	m_dimension = vec3i(width, height, depth);
}

TerrainCreator::~TerrainCreator()
{
}

float* TerrainCreator::getTerrainData()
{
	return m_terrain;
}

float TerrainCreator::getTerrainDataAt(vec3i p)
{
	return m_terrain[p.x + p.y * m_dimension.x + p.z * m_dimension.x * m_dimension.y];
}

int TerrainCreator::getNumberOfVertices(bool cube)
{
	if (cube)
	{
		return m_size * 3;
	}

	int vSize = 0;
	for (uint i = 0; i < m_size; ++i)
	{
		if (m_terrain[i] == 1.0f)
			vSize += 3;	//3 vertices for every point
	}

	return vSize;
}

void TerrainCreator::getVertices(GLfloat* verticesOut, bool cube)
{
	int vCount = 0;

	for (uint z = 0; z < m_dimension.z; ++z)
	{
		for (uint y = 0; y < m_dimension.y; ++y)
		{
			for (uint x = 0; x < m_dimension.x; ++x)
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

float* TerrainCreator::createTerrain(int seed, int iterations)
{
	RNGesus rng(seed, seed * 13, seed * 19);

	double diagonal = glm::sqrt(glm::pow(m_dimension.x, 2) + glm::pow(m_dimension.z, 2) + glm::pow(m_dimension.y, 2));

	//blobs
	{
		glm::vec3 pillars[3];
		pillars[0] = glm::vec3(m_dimension.x * rng.getZeroToOne(), m_dimension.y * rng.getZeroToOne(), m_dimension.z * rng.getZeroToOne());
		pillars[1] = glm::vec3(m_dimension.x * rng.getZeroToOne(), m_dimension.y * rng.getZeroToOne(), m_dimension.z * rng.getZeroToOne());
		pillars[2] = glm::vec3(m_dimension.x * rng.getZeroToOne(), m_dimension.y * rng.getZeroToOne(), m_dimension.z * rng.getZeroToOne());
		
		for (uint z = 0; z < m_dimension.z; ++z)
		{
			for (uint y = 0; y < m_dimension.y; ++y)
			{
				for (uint x = 0; x < m_dimension.x; ++x)
				{
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
					if (f > 4.f)
					{
						m_terrain[pos] = 1.0f;
						//std::cout << "#";
					}
					else
					{
						m_terrain[pos] = 0.0f;
						//std::cout << " ";
					}
				}
				//std::cout << std::endl;
			}
			//std::cout << std::endl;
		}
	}

	return m_terrain;
}
