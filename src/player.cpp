#include "player.h"
#include "world.h"

Player::Player(int id)
{
	nexus = new Nexus();
	this->id = id;
	gold = 500;
	canSpawnL = true;
	canSpawnR = true;
	rightTimer = 2.0f;
	leftTimer = 2.0f;
}

void Player::update(float dt)
{
	if(!canSpawnR)
	{
		rightTimer -= dt;
		if(rightTimer < 0)
		{
			rightTimer = 2.0f;
			canSpawnR = true;
		}
	}

	if (!canSpawnL)
	{
		leftTimer -= dt;
		if (leftTimer < 0)
		{
			leftTimer = 2.0f;
			canSpawnL = true;
		}
	}
	
	if (nexus != NULL) 
	{
		gold += nexus->collectionRate*dt;
	}
	
	for (int i = 0; i < gridSize; i++) 
	{
		for (int j = 0; j < gridSize; j++) 
		{
			if (World::getInstance()->textureMaps.grid[i][j].content == collector && World::getInstance()->textureMaps.grid[i][j].idPlayer == id) 
			{
				Collector* collector = (Collector*)World::getInstance()->textureMaps.grid[i][j].building;
				if(collector->isRunning)
				{
					gold += collector->collectionRate*dt;
				}
			}
		}
	}
}