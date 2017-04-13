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

float* TerrainCreator::createTerrain(int seed, int iterations)
{
	RNGesus rng(seed, seed * 13, seed * 19);

	//blobs
	{
		glm::vec3 pillars[3];
		pillars[0] = glm::vec3(m_dimension.x * rng.getZeroToOne(), m_dimension.y * rng.getZeroToOne(), m_dimension.z * rng.getZeroToOne());
		for (uint z = 0; z < m_dimension.z; ++z)
		{
			for (uint y = 0; y < m_dimension.y; ++y)
			{
				for (uint x = 0; x < m_dimension.x; ++x)
				{
					uint pos = x + y*m_dimension.x + z*m_dimension.y;
					glm::vec3 tmpVec(x, y, z);
					tmpVec -= pillars[0];
					m_terrain[pos] = 1.0 / glm::length(tmpVec);
					if ( m_terrain[pos] > 0.5f)
						std::cout << "#";
					else
						std::cout << " ";
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	}

	return m_terrain;
}
