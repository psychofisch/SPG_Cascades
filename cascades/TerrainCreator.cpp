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

	int stepLength = 16;

	for (size_t i = 0; i < m_size; i += stepLength)
	{
		m_terrain[i] = glm::clamp((rng.GetNumber() / (ULONG_MAX + 1.0) + 0.5) * 4999 , 0., 1.0);
	}

	return m_terrain;
}
