#pragma once
class TerrainCreator
{
public:
	struct vec3i {
		int x, y, z;

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

	TerrainCreator(int width, int height, int depth);
	~TerrainCreator();

	float* getTerrainData();
	float getTerrainDataAt(vec3i position);

private:
	Mode m_mode;
	float* m_terrain;
	vec3i m_dimension;
};

