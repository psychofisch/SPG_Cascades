#include "TerrainCreator.h"

TerrainCreator::TerrainCreator(int width, int height, int depth)
{
	m_terrain = new float[width * height * depth];
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
