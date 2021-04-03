#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include "framework.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "entity.h"
#include "character.h"

class Player 
{
private:
	
	std::vector<Character*> minions;

public:
	Nexus* nexus;
	float gold;
	int id;
	Player(int id);
	void update(float dt);
	bool canSpawnR;
	bool canSpawnL;
	float rightTimer;
	float leftTimer;
};

#endif