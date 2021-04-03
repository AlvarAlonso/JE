#ifndef STAGE_H
#define STAGE_H

#include "camera.h"
#include "input.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"

class Stage
{
public:

	Stage();

	int id;
	Texture* texture;
	virtual void update(float dt);
	virtual void render(Camera* camera);

protected:

	Shader* block;
	Shader* textures;
	Mesh* gui;
};

class HUD : public Stage
{
public:
	HUD();
	void update(float dt);
	void render(Camera* camera);
};

class Menu : public Stage
{
public:
	bool keypressed;
	Menu();
	void update(float dt);
	void render(Camera* camera);
	int option;

private:
	Mesh* playBtn;
	Mesh* instructionsBtn;
	Mesh* exitBtn;
	Mesh* logoMesh;
	Texture* background;
	bool hide;
	float counter;
};

class GameOver : public Stage
{
	GameOver();
	void update(float dt);
	void render(Camera* camera);
};

class Score : public Stage
{
public:
	Score();

	int winner;

	void update(float dt);
	void render(Camera* camera);

private:

	Mesh* continueBtn;
};

class Instructions : public Stage
{
public:
	Instructions();

	int option;

	void update(float dt);
	void render(Camera* camera);

private:
	Mesh * controllerBtn;
	Mesh* keyboardBtn;
	Mesh* cancelBtn;
	Texture* background;
};

class Controller : public Stage
{
public:
	Controller();
	void render(Camera* camera);
	void update(float dt);
	int screen;
	int device;
private:
	Mesh * quad1;
	Mesh* quad2;
	Mesh* quad3;
	Mesh* quad4;
	Mesh* nextBtn;

	Texture* texture1;
	Texture* texture2;
	Texture* texture3;
	Texture* texture4;

};

#endif