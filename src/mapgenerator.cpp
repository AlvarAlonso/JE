#include "world.h"

MapGenerator* MapGenerator::instance = NULL;

MapGenerator::MapGenerator()
{
	loadAssets();
}

void MapGenerator::generateMap()
{
	genParams = World::getInstance()->genParams;
	createPlane();
	generateRoads();
}

//methods for the map generation
void MapGenerator::createPlane()
{
	printf("Creating plane\n");

	EntityTerrain* map = new EntityTerrain();
	Mesh* plane = new Mesh();
	plane->createPlane(genParams.planeSize);
	map->m.translate(genParams.planeSize, 0, -genParams.planeSize);

	map->mesh = plane;
	map->texture1 = textures["grass"];
	map->texture2 = textures["rocks"];
	map->shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture_mask.fs");

	World::getInstance()->map = map;
}

void MapGenerator::generateRoads()
{
	printf("Generating roads\n");
	//int size = World::getInstance()->genParams.planeSize;
	int cells = genParams.cells;

	for (int i = 0; i < cells; i++) 
	{
		for (int j = 0; j < cells; j++) 
		{
			World::getInstance()->textureMaps.grid[i][j].building = NULL;
			World::getInstance()->textureMaps.grid[i][j].idPlayer = 2;
			if (i == 2 || i == 5) 
			{
				World::getInstance()->textureMaps.grid[i][j].isBuilded = true;
				World::getInstance()->textureMaps.grid[i][j].content = road;
			}
			else
			{
				World::getInstance()->textureMaps.grid[i][j].isBuilded = false;
				World::getInstance()->textureMaps.grid[i][j].content = empty;
			}
		}
	}

	Texture* mask = Texture::Get("data/town/mask.tga");

	World::getInstance()->textureMaps.textureMask = mask;
	World::getInstance()->map->textureMap = World::getInstance()->textureMaps.textureMask;

}

void MapGenerator::loadAssets()
{
	this->meshes["house1"] = Mesh::Get("data/meshes/house1.obj");
	this->meshes["house2"] = Mesh::Get("data/meshes/house2.obj");
	this->meshes["house3"] = Mesh::Get("data/meshes/house3.obj");
	this->meshes["house4"] = Mesh::Get("data/meshes/house4.obj");
	this->meshes["tower"] = Mesh::Get("data/meshes/tower.obj");
	this->meshes["barn"] = Mesh::Get("data/meshes/barn.obj");
	this->meshes["wall"] = Mesh::Get("data/meshes/wall.obj");
	this->meshes["sphere"] = Mesh::Get("data/meshes/sphere.obj");
	this->meshes["trunk"] = Mesh::Get("data/meshes/trunk.obj");
	this->meshes["leaves"] = Mesh::Get("data/meshes/leaves.obj");
	this->meshes["lod_tree"] = Mesh::Get("data/meshes/lod_tree.obj");
	this->meshes["minion"] = Mesh::Get("data/characters/male.mesh");
	this->meshes["nexus"] = Mesh::Get("data/meshes/nexus.obj");

	this->textures["house"] = Texture::Get("data/town/house.tga");
	this->textures["leaves_olive"] = Texture::Get("data/trees/leaves_olive.tga");
	this->textures["lod_olive"] = Texture::Get("data/trees/lod_olive.tga");
	this->textures["trunk"] = Texture::Get("data/trees/trunk.tga");
	this->textures["tree"] = Texture::Get("data/trees/tree.tga");
	this->textures["grass"] = Texture::Get("data/grass.tga");
	this->textures["rocks"] = Texture::Get("data/rocks.tga");
	this->textures["cielo"] = Texture::Get("data/cielo.tga");
	this->textures["minion"] = Texture::Get("data/characters/male.png");
	this->textures["redminion"] = Texture::Get("data/characters/redminion.png");
	this->textures["blueminion"] = Texture::Get("data/characters/blueminion.png");
	this->textures["energy"] = Texture::Get("data/energy_texture.tga");
	this->textures["blue_energy"] = Texture::Get("data/blue_energy.tga");
	this->textures["soil"] = Texture::Get("data/soil.tga");
	this->textures["wall"] = Texture::Get("data/wall.png");
	this->textures["player1"] = Texture::Get("data/player1_v2.png");
	this->textures["player2"] = Texture::Get("data/player2_v2.png");
}
