#pragma once
#pragma once

#ifndef ENTITY_H
#define ENTITY_H

#include "includes.h"
#include "camera.h"
#include "utils.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

struct sbox {
	Vector2 center;
	int xSize;
	int ySize;
};

struct floatingCube {
	Mesh* mesh = new Mesh();
	Texture* texture;
	Matrix44 model;
	Vector3 position;
	Shader* shader;

	floatingCube(Vector3 pos);
	void render(Camera* camera);
};

struct Sky {
	Mesh* sphere = new Mesh();
	Texture* texture;
	Matrix44 model;
	Shader* shader;
	Vector3 position;
	Sky();
	void render(Camera* camera);
	void update(double dt);
};

enum{LEFT= 0, RIGHT};

class Entity {
public:
	Entity();
	virtual ~Entity();

	virtual void render() {}
	virtual void update(float dt) {}

	const char* type;
	Matrix44 m;
	float health;
	float mHealth;
	int owner;
	bool alive;
	bool beingBuilded;

	Vector3 getPosition();
	void takeDamage(int dmg);
};

class EntityMesh : public Entity {
public:

	Mesh * mesh;
	Texture* texture;
	Texture* texture2;
	Shader* shader;
	Vector4 color;
	sbox box;
	float repeat;

	EntityMesh();

	virtual void render();
	virtual void update(float dt);
};

class EntityTree : public EntityMesh {
public:
	Mesh * mesh2;
	Mesh* mesh3;
	Texture* texture3;

	EntityTree(Vector3 pos);

	virtual void render();
};

class EntityTerrain : public Entity {
public:
	Mesh * mesh;
	Texture* textureMap;
	Texture* texture1;
	Texture* texture2;
	Texture* texture3;
	Shader* shader;
	Vector4 color;

	EntityTerrain();

	virtual void render(int sizePlane);
};

class EntityBuilding : public EntityMesh 
{
public:
	EntityBuilding();
	~EntityBuilding();
	Shader* preview;
	Vector3 position;
	ivector2 gridPosition;
	bool canPlace;
	item type;
	float cost;
	HSAMPLE hSample;
	HCHANNEL hSampleChannel;

	int state;
	int buildingTime;
	bool isRunning;

	virtual void render();
	void build(float dt);

};

class Nexus : public EntityBuilding 
{
public:

	float collectionRate;
	float spawnTime;
	int side;

	Nexus();
	//~Nexus();
	void update(float dt);
	void spawnMinions(int side);

private:
	void collect();
};

class Collector : public EntityBuilding
{
public:

	float collectionRate;

	Collector();
	//~Collector();
	void update(float dt);

private:
	void collect();
};

class Defense : public EntityBuilding
{
public:

	float coolDown;
	float damage;
	float range;
	Entity* target;

	Defense();
	//~Defense();
	void update(float dt);
	void behaviour();
	bool checkEnemies();
	void shoot();

};

#endif ENTITY_H