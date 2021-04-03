
#include "character.h"
#include "input.h"
#include "world.h"

Character::Character(Vector3 position, int owner) 
{
	mesh = World::getInstance()->mapGenerator->meshes["minion"];

	id = 0;
	this->owner = owner;
	if (owner == 1) { 
		texture = World::getInstance()->mapGenerator->textures["redminion"];
		front = Vector3(0, 0, 1); 
		yaw = 0;
	}
	else { 
		texture = World::getInstance()->mapGenerator->textures["blueminion"];
		front = Vector3(0, 0, -1); 
		yaw = 180;
	}
	color = Vector4(1, 1, 1, 1);
	objective = NULL;
	vel = 50;
	pos = position;
	coolDown = 3;
	state = MOVE;
	health = 20;
	shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	beingBuilded = false;

	if (BASS_Init(2, 44100, 0, 0, NULL) == false)
	{
		printf("Error when initializing BASS\n");
	}

	hSample = BASS_SampleLoad(false, "data/sounds/defense-shot.wav", 0, 0, 4, 2);
	play = true;
}

Character::~Character() {
	std::cout << "character deleted" << std::endl;
}

void Character::render(Camera* camera) {

	updateMatrix();
	animation();

	Matrix44 model = this->m;

	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setTexture("u_texture", this->texture);
	shader->setUniform("u_color", color);
	mesh->renderAnimated(GL_TRIANGLES, &skelet);
	shader->disable();
}

void Character::update(double dt) {

	Vector3 movement;
	coolDown -= dt;
	behaviour();

	if (health <= 0 && play) 
	{
		//hSampleChannel = BASS_SampleGetChannel(hSample2, false);
		//BASS_ChannelPlay(hSampleChannel, false);
		play = false;
	}

	if (state == MOVE) {
		if (World::getInstance()->player1->nexus->owner != owner)
			moveToEnemy(World::getInstance()->player1->nexus, dt);
		else moveToEnemy(World::getInstance()->player2->nexus, dt);
	}
	else if (state == ATTACK) {
		coolDown -= dt;
		if (coolDown < 0) {
			shoot();
			coolDown = 5.25;
		}
	}
	else if (state == IDLE) {
	
	}

	this->velocity *= 0.95;

}

void Character::updateMatrix()
{
	m.setIdentity();
	m.translate(pos.x, pos.y, pos.z);
	m.rotate(yaw*DEG2RAD, Vector3(0, 1, 0));
	m.scale(0.5, 0.5, 0.5);
}

void Character::animation() {

	float time = Game::instance->time;
	float speed = velocity.length() * 0.1;

	Camera* camera = Camera::last_enabled;
	Animation* anim = NULL;
	Animation* animB = NULL;
	Skeleton blended_skeleton;
	float w = 0.0;

	const char* name = "data/characters/idle.skanim";

	if (state == IDLE) {
		anim = Animation::Get(name);
		anim->assignTime(time);
		skelet = anim->skeleton;
	}
	else if (state == MOVE) {
		anim = Animation::Get(name);
		anim->assignTime(time);
		animB = Animation::Get("data/characters/walking.skanim");
		animB->assignTime(time);
		w = clamp(speed, 0, 1);
		blendSkeleton(&anim->skeleton, &animB->skeleton, w, &skelet);
	}
	
	else if(state == ATTACK){
		animB = Animation::Get("data/characters/attack.skanim");
		animB->assignTime(-time);
		skelet = animB->skeleton;
	}	
}

void Character::testCollision(EntityMesh* ent, double dt) {
	Mesh* mesh = ent->mesh;
	Vector3 charCenter = pos + Vector3(0, 4, 0);
	Vector3 col_point;
	Vector3 col_normal;
	float max_ray_dist = 10;

	if (mesh->testSphereCollision(ent->m, charCenter, 3, col_point, col_normal) == true) {
		Vector3 push_away = normalize(col_point - charCenter) * dt;
		pos = pos - push_away;
		velocity = reflect(velocity, col_normal) * 0.95;
	}
}

void Character::testCollision(Character* ent, double dt) {
	Mesh* mesh = ent->mesh;
	Vector3 charCenter = pos + Vector3(0, 4, 0);
	Vector3 col_point;
	Vector3 col_normal;
	float max_ray_dist = 10;

	if (mesh->testSphereCollision(ent->m, charCenter, 5, col_point, col_normal) == true) {
		Vector3 push_away = normalize(col_point - charCenter) * dt;
		pos = pos - push_away;
		velocity = reflect(velocity, col_normal) * 0.95;
	}
}

void Character::shoot() {
	Vector3 dir = objective->getPosition() - this->pos;
	dir.normalize();
	if (front.z > 0) dir = dir + Vector3(-5, 10, 5);
	else if (front.z < 0) dir = dir + Vector3(5, 10, -5);
	Vector3 output = this->pos + dir; // Vector3(5, 10, -10);
	BulletManager::getInstance()->createBullet(output, id, objective);
	hSampleChannel = BASS_SampleGetChannel(hSample, false);
	BASS_ChannelPlay(hSampleChannel, false);
}

bool Character::checkEnemies() {
	float distance = 100;
	Entity* enemy = NULL;
	Entity* aux = NULL;

	for (int i = 0; i < World::getInstance()->entities.size(); i ++) {
		aux = World::getInstance()->entities[i];
		if ((aux->owner != this->owner) && (aux->getPosition() - pos).length() < distance && aux->alive == true) 
		{
			distance = (aux->getPosition() - pos).length();
			enemy = World::getInstance()->entities[i];
		}
	}
	for (int i = 0; i < World::getInstance()->characters.size(); i ++) {
		aux = World::getInstance()->characters[i];
		if ((aux->owner != owner) && (aux->getPosition() - pos).length() < distance && aux->alive == true) 
		{
			distance = (aux->getPosition() - pos).length();
			enemy = aux;
		}
	}
	objective = enemy;
	if (enemy == NULL) return false;
	else return true;
}

void Character::behaviour() {
	if (checkEnemies() == false) state = MOVE;
	else if (checkEnemies() == true) state = ATTACK;
	else if(objective->alive == false) state = IDLE;
}

void Character::moveToEnemy(Nexus* n, double dt) {
	
	Vector3 movement;
	Vector3 to_target = n->getPosition() - pos;
	float distance = to_target.length();
	to_target.normalize();
	Matrix44 rotation;
	float angle = 0;
	objective = n;

	if (distance < 100) {
		angle = acos(front.dot(to_target));
		rotation.rotate(angle, Vector3(0, 1, 0));
		to_target.normalize();
		yaw = angle;
	}
	else {
		to_target = front;
	}
	
	to_target = rotation * to_target;
	velocity = velocity + to_target;
	prev_pos = pos;
	pos = pos + velocity * dt;
}