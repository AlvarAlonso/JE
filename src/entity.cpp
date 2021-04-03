#include "world.h"

Entity::Entity() {
	m.setIdentity();
	type = NULL;
	alive = true;
}

Entity::~Entity() {}

void Entity::takeDamage(int dmg) { 
	health = health - dmg; 
	if (health < 0) { 
		alive = false;
	}
}

Vector3 Entity::getPosition() { return m.getTranslation(); }

floatingCube::floatingCube(Vector3 pos) {
	mesh->createCube();
	texture = Texture::Get("data/soil.tga");
	shader = Shader::Get("data/shaders/basicExtras.vs", "data/shaders/texture.fs");
	position = pos;
	model.translate(position.x, position.y, position.z);
	model.scale(5, 5, 5);
}

void floatingCube::render(Camera* camera) {
	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_time", Game::instance->time);
	shader->setTexture("u_texture", texture);
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	mesh->render(GL_TRIANGLES);
	shader->disable();
}

EntityMesh::EntityMesh() {
	mesh = NULL;
	texture = NULL;
	texture2 = NULL;
	shader = NULL;
	color = Vector4(1, 1, 1, 1);
}

EntityBuilding::EntityBuilding() : EntityMesh()
{
	printf("creating building\n");
	preview = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	if (preview == NULL) 
	{
		printf("WTF man\n");
	}
	canPlace = false;
	health = 0;
	buildingTime = 0;
	isRunning = false;

	hSample = BASS_SampleLoad(false, "data/sounds/building.wav", 0, 0, 4, 2);
}

EntityBuilding::~EntityBuilding() 
{
	printf("Deleting building\n");
}

Nexus::Nexus() : EntityBuilding()
{
	printf("Nexus created\n");
	type = nexus;
	collectionRate = 0.0f;
	mHealth = 50;
	health = mHealth;
	spawnTime = 3;
	side = RIGHT;
	beingBuilded = false;
}

void Nexus::update(float dt) 
{
}

void Nexus::spawnMinions(int side)
{
	int r = (int)(random() * 20);
	if (owner == 1 ) {
		if (side == RIGHT) World::getInstance()->createCharacter(this->getPosition() + Vector3(55 + r, 0, 0), this->owner);
		else World::getInstance()->createCharacter(this->getPosition() + Vector3(-65 + r, 0, 0), this->owner);
	}
	else {
		if (side == RIGHT) World::getInstance()->createCharacter(this->getPosition() + Vector3(-65 + r, 0, 0), this->owner);
		else World::getInstance()->createCharacter(this->getPosition() + Vector3(55 + r, 0, 0), this->owner);
	}
}

Collector::Collector() : EntityBuilding()
{
	printf("Collector created\n");
	type = collector;
	collectionRate = 10.0f;
	cost = 200.0f;
	mHealth = 100;
	health = 0.1;
	beingBuilded = false;
}

void Collector::update(float dt)
{
	if (beingBuilded)
	{
		hSampleChannel = BASS_SampleGetChannel(hSample, false);
		BASS_ChannelPlay(hSampleChannel, false);
		build(dt);
	}
	else {
		hSampleChannel = BASS_SampleFree(hSample);
	}
}

Defense::Defense() : EntityBuilding()
{
	printf("Defense created\n");
	type = defense;
	coolDown = 2;
	damage = 10;
	range = 100.0f;
	cost = 300.0f;
	mHealth = 150;
	health = 0.1;
	target = NULL;
	beingBuilded = false;
	color = Vector4(1, 1, 1, 1);
	position = position + Vector3(0, 10, 0);
}

void Defense::update(float dt)
{

	if(beingBuilded)
	{
		hSampleChannel = BASS_SampleGetChannel(hSample, false);
		BASS_ChannelPlay(hSampleChannel, false);
		build(dt);
	}
	else if(alive && isRunning)
	{
		hSampleChannel = BASS_SampleFree(hSample);
		coolDown -= dt;
		behaviour();
		if (state == ATTACK) {
			if (coolDown < 0) {
				shoot();
				coolDown = 2;
			}
		}
		else if (state == IDLE) {}
	}

}


void Defense::behaviour() {
	if (checkEnemies() == true) {
		state = ATTACK;
		std::cout << "defense check true " << state << std::endl;
	}
	else state == IDLE;
}

bool Defense::checkEnemies() {
	float distance = 150;
	Entity* enemy = NULL;
	Entity* aux = NULL;
	
	for (int i = 0; i < World::getInstance()->entities.size(); i++) {
		aux = World::getInstance()->entities[i];
		if ((aux->owner != this->owner) && (aux->getPosition() - getPosition()).length() < distance && aux->alive == true)
		{
			distance = (aux->getPosition() - getPosition()).length();
			enemy = aux;
		}
	}
	
	for (int i = 0; i < World::getInstance()->characters.size(); i++) {
		aux = World::getInstance()->characters[i];
		if ((aux->owner != owner) && (aux->getPosition() - getPosition()).length() < distance && aux->alive == true)
		{
			distance = (aux->getPosition() - getPosition()).length();
			enemy = aux;
		}
	}
	target = enemy;
	if (enemy == NULL) return false;
	else return true;
}

void Defense::shoot() {
	BulletManager::getInstance()->createBullet(this->getPosition() + Vector3(20, 20, 20), -1, target);
}

void EntityMesh::render() {

	Camera* camera = Camera::last_enabled;
	Matrix44 model = this->m;

	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setTexture("u_texture", texture);
	shader->setUniform("u_color", color);

	mesh->render(GL_TRIANGLES);

	shader->disable();
}

void EntityBuilding::render()
{
	Camera* camera = Camera::last_enabled;
	Matrix44 model = this->m;
	Shader* current;
	float isDefense = 0;

	if ((beingBuilded || isRunning) && alive)
	{
		current = shader;
		current->enable();
		current->setUniform("u_color", color);
		if (World::getInstance()->player1->id != owner)
		{
			current->setUniform("u_mode", 1.0f);
		}
		else 
		{
			current->setUniform("u_mode", 0.0f);
		}
	}
	else 
	{
		current = preview;
		current->enable();
		current->setUniform("u_mode", 0.0f);
		if (canPlace) 
		{
			current->setUniform("u_color", Vector4(0, 200, 0, 1));
		}
		else 
		{
			current->setUniform("u_color", Vector4(200, 0, 0, 1));
		}
	}
	if (this->type == defense) {
		model.translate(0, 15, 0);
		model.scale(10, 10, 10);
		isDefense = 1;
	}

	float percentage = (float)health / (float)mHealth;
	current->setUniform("u_defense", isDefense);
	current->setUniform("u_time", Game::instance->time);
	current->setUniform("u_percentage", percentage);
	current->setUniform("u_model", model);
	current->setUniform("u_viewprojection", camera->viewprojection_matrix);
	current->setTexture("u_texture", texture);

	mesh->render(GL_TRIANGLES);

	shader->disable();

}

void EntityMesh::update(float dt) {}

EntityTree::EntityTree(Vector3 pos)
{
	mesh = World::getInstance()->mapGenerator->meshes["lod_tree"];
	texture = World::getInstance()->mapGenerator->textures["lod_olive"];
	texture2 = World::getInstance()->mapGenerator->textures["trunk"];
	texture3 = World::getInstance()->mapGenerator->textures["tree"];
	shader = Shader::Get("data/shaders/basicExtras.vs", "data/shaders/texture.fs");

	pos = pos;
	m.translate(pos.x, pos.y + 5, pos.z);
	m.rotate(-90 * RAD2DEG, Vector3(0, 1, 0));

}

void EntityTree::render()
{
	Camera* camera = Camera::last_enabled;
	Matrix44 model = this->m;
	model.scale(2, 2, 2);

	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_repeat", 1.0f);
	shader->setTexture("u_texture", texture);
	mesh->render(GL_TRIANGLES);

	shader->disable();
}

EntityTerrain::EntityTerrain() {
	mesh = NULL;
	texture1 = NULL;
	texture2 = NULL;
	textureMap = NULL;
	shader = NULL;
	color = Vector4(1, 1, 1, 1);
}

void EntityTerrain::render(int sizePlane) {

	Camera* camera = Camera::last_enabled;
	Matrix44 model = this->m;

	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setTexture("u_mask", textureMap);
	shader->setTexture("u_grass", texture1);
	shader->setTexture("u_rocks", texture2);
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));

	mesh->render(GL_TRIANGLES);

	shader->disable();
}

void EntityBuilding::build(float dt)
{
	health += 10.0f * dt;
	if(health >= mHealth)
	{
		health = mHealth;
		beingBuilded = false;
		isRunning = true;
	}
}

Sky::Sky() {
	sphere = World::getInstance()->mapGenerator->meshes["sphere"];
	texture = World::getInstance()->mapGenerator->textures["cielo"];
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	position = Game::instance->camera->eye;
	model.translate(position.x + 10, position.y - 10, position.z);
	model.scale(20, 20, 20);
}

void Sky::render(Camera* camera) {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setTexture("u_texture", texture);
	shader->setUniform("u_color", Vector4(1,1,1,1));
	shader->setUniform("u_mode", 0.0f);
	sphere->render(GL_TRIANGLES);
	shader->disable();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void Sky::update(double dt) {
	position = Game::instance->camera->eye;
	model.setIdentity();
	model.setTranslation(position.x + 1, position.y , position.z);
}
