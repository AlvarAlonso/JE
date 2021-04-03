#pragma once
#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include "player.h"

const int gridSize = 8;

struct sCell 
{
	bool isBuilded;
	item content;
	uint8 idPlayer;
	EntityBuilding* building;
};

struct sgenerationTextures
{
	Texture* heightMap;
	Texture* textureMask;
	sCell grid[gridSize][gridSize];
};

struct sgenerationParameters
{
	//plane generation
	int planeSize;
	int cells;
};

class MapGenerator
{
	private:

		static MapGenerator* instance;
		sgenerationParameters genParams;
		sgenerationTextures genTextures;

		MapGenerator();

		void loadAssets();

		//methods for the map generation
		//core methods
		void createPlane();
		void generateRoads();

	public:

		//meshes and textures loaded
		std::map<std::string, Mesh*> meshes;
		std::map<std::string, Texture*> textures;

		static MapGenerator* getInstance()
		{
			if (instance == NULL)
			{
				instance = new MapGenerator();
			}
			return instance;
		}

		//GenerateMap
		void generateMap();
};

#endif