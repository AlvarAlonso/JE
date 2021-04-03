#pragma once
#ifndef CHARACTER_H
#define CHARACTER_H

#include "entity.h"
#include "animation.h"
#include "game.h"
#include "framework.h"
#include "bullet_manager.h"

enum state { MOVE = 0, ATTACK, IDLE };

class Character : public Entity {
public:
	int state;
	int id;
	//int owner;
	Vector3 pos;
	Vector3 prev_pos;
	Vector3 velocity;
	Vector3 front;
	Vector4 color;

	Entity* objective;

	float vel;
	float yaw;
	float coolDown;

	Mesh* mesh;
	Texture* texture;
	Shader* shader;

	Skeleton skelet;
	std::vector<Matrix44> bone_matrices;

	HSAMPLE hSample;
	HSAMPLE hSample2;
	bool play;
	HCHANNEL hSampleChannel;
	
	Character(Vector3 position, int owner);
	~Character();

	void render(Camera* camera);
	void update(double dt);
	void updateMatrix();
	void animation();
	void testCollision(EntityMesh* mesh, double dt);
	void testCollision(Character* character, double dt);
	void shoot();
	void moveToEnemy(Nexus* n, double dt);
	bool checkEnemies();
	void behaviour();
};

#endif
