#pragma once
#ifndef WORLD_H
#define WORLD_H

#include "utils.h"
#include "mapgenerator.h"

class World
{
private:

	static World* instance;
	
	BulletManager* bulletManager;

	World();

public:
	MapGenerator* mapGenerator;
	//returns static instance of world
	static World* getInstance()
	{
		if (instance == NULL)
		{
			instance = new World();
		}
		return instance;
	}

	sgenerationParameters genParams;
	sgenerationTextures textureMaps;

	Player* player1;
	Player* player2;
	Player* currentPlayer;

	//Entities
	std::vector<EntityBuilding*> entities;
	std::vector<Character*> characters;
	std::vector<floatingCube*> cubes;
	std::vector<EntityTree*> trees;
	EntityBuilding* preview;
	EntityBuilding* preview2;
	int idCharacters;
	Sky* sky;
	EntityTerrain* map;
	EntityMesh* background;

	void generateMap();
	void resetGame();
	void setNexus();
	void setGenerationParameters(sgenerationParameters parameters);
	void createCharacter(Vector3 pos, int owner);
	EntityBuilding* createBuilding(Vector3 position, item type);
	void createExtras();
	void renderExtras(Camera* camera);
	void renderCharacters(Camera* camera);
	void updateCharacters(double dt);
	void renderEntities(Camera* camera);
	void cube_map(Camera* camera);
	void updateWinnerState(double dt);

	void render(Camera* camera);
	void update(double dt);
};

#endif 