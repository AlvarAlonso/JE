#include "bullet_manager.h"

BulletManager* BulletManager::instance = NULL;

BulletManager::BulletManager() {

}

Bullet::Bullet(Vector3 pos, int author, Entity* ent) {
	
	mesh = new Mesh();
	mesh->createCube();
	if ( ent != NULL && ent->owner != 0 ) color = Vector4(0, 0, 1, 1);
	else color = Vector4(1, 0, 0, 1);
	velocity = Vector3(0, 0, -1);
	position = pos;
	objective = ent;
	author = author;
	force = 25;
	ttl = 100;
	damage = 10;
	inUse = true;

	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
}

Bullet::~Bullet() {
	//delete this;
}

void Bullet::render(Camera* camera) 
{
	updateMatrix();

	shader->enable();
	shader->setUniform("u_color", color);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_model", model);
	mesh->render(GL_TRIANGLES);
	shader->disable();
}
void Bullet::update(double dt) {
	ttl -= dt;
	if (ttl <= 0 || objective == NULL) { 
		inUse == false; 
	} //destroy
	else {
		if (objective != NULL)
			velocity = objective->getPosition() - position;
		position = position + velocity.normalize() * force * dt;
		if ((objective->getPosition() - position).length() < 10 && inUse == true) {
			if (objective->beingBuilded) objective->takeDamage(damage * 2);
			else objective->takeDamage(damage);
			inUse = false;
		}
	}
}

void Bullet::updateMatrix() {
	model.setIdentity();
	model.translate(position.x, position.y, position.z);
	model.scale(0.25, 0.25, 0.25);
}

void BulletManager::render(Camera* camera) 
{
	for (int i = 0; i < bullets_vector.size(); i++) {
		if(bullets_vector[i]->inUse)
			bullets_vector[i]->render(camera);
	}
}
void BulletManager::update(double dt) {
	for (int i = 0; i < bullets_vector.size(); i++) {
		if (bullets_vector[i]->inUse) bullets_vector[i]->update(dt);
		else bullets_vector.erase(bullets_vector.begin() + i);
	}
}
void BulletManager::createBullet(Vector3 position, int author, Entity* ent ) {
	Bullet* bullet = new Bullet(position, author, ent);
	bullets_vector.push_back(bullet);
}

void BulletManager::reset()
{
	for(int i = 0; i < bullets_vector.size(); i++)
	{
		bullets_vector[i]->~Bullet();
	}
	bullets_vector.clear();
}