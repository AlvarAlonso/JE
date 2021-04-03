#pragma once
#ifndef BULLET_MANAGER_H
#define BULLET_MANAGER_H

#include "includes.h"
#include "camera.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "entity.h"

struct Bullet {

	Entity* objective;

	Mesh* mesh;
	Shader* shader;
	Matrix44 model;
	Vector4 color;
	Vector3 position;
	Vector3 last_position;
	Vector3 velocity;
	float ttl;
	float force;
	int id;
	int damage;
	int author;
	bool inUse;

	Bullet(Vector3 pos, int author, Entity* ent);
	~Bullet();

	void render(Camera* camera);
	void update(double dt);
	void updateMatrix();

};

class BulletManager {
public:

	static BulletManager* instance;

	std::vector<Bullet*> bullets_vector;

	BulletManager();

	static BulletManager* getInstance() {
		if (instance == NULL) instance = new BulletManager();
		return instance;
	}

	void render(Camera* camera);
	void update(double dt);
	void createBullet(Vector3 position, int author, Entity* ent);
	void reset();
};

#endif 