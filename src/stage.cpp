#include "stage.h"
#include "input.h"
#include "world.h"
#include "game.h"

float window_width = 800.0f;
float window_height = 600.0f;

Stage::Stage()
{
	block = Shader::Get("data/shaders/basic.vs", "data/shaders/flat.fs");
	textures = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	gui = NULL;
}

void Stage::update(float dt) {}

void Stage::render(Camera* camera) {}

Menu::Menu()
{
	option = 0;
	keypressed = false;
	counter = 1.0f;

	gui = new Mesh();
	gui->createQuad(0.0f, -window_height / 2 * 0.75, window_width, window_height*0.30, false);

	logoMesh = new Mesh();
	logoMesh->createQuad(0.0f, 100.0f, 173.0f, 142.0f, false);

	//buttons
	playBtn = new Mesh();
	playBtn->createQuad(-window_width / 2 * 0.6, -window_height / 2 * 0.75, window_width*0.2f, window_height*0.1f, false);

	instructionsBtn = new Mesh();
	instructionsBtn->createQuad(0.0f, -window_height / 2 * 0.75, window_width*0.2f, window_height*0.1f, false);

	exitBtn = new Mesh();
	exitBtn->createQuad(window_width / 2 * 0.6, -window_height / 2 * 0.75, window_width*0.2f, window_height*0.1f, false);

	background = Texture::Get("data/GUI/wall.tga");
}

void Menu::render(Camera* camera)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	Matrix44 model;

	textures->enable();
	textures->setUniform("u_model", model);
	textures->setUniform("u_viewprojection", camera->viewprojection_matrix);
	textures->setUniform("u_color", Vector4(1, 1, 1, 1));
	textures->setUniform("u_mode", 0.0f);

	if (keypressed) {
		textures->setTexture("u_texture", background);

		gui->render(GL_TRIANGLES);

		textures->disable();

		Vector4 color = Vector4(0, 0, 255, 0);

		block->enable();
		block->setUniform("u_model", model);
		block->setUniform("u_viewprojection", camera->viewprojection_matrix);
		block->setUniform("u_color", color);

		playBtn->render(GL_TRIANGLES);
		instructionsBtn->render(GL_TRIANGLES);
		exitBtn->render(GL_TRIANGLES);

		if (option == 0)
		{
			drawText(window_width * 0.2 - 23, window_height * 0.88 - 8, "Play", Vector3(0, 255, 0), 2);
			drawText(window_width / 2 - 62, window_height * 0.88 - 8, "Instructions", Vector3(0, 0, 0), 2);
			drawText(window_width * 0.8 - 23, window_height * 0.88 - 8, "Exit", Vector3(0, 0, 0), 2);
		}
		else if (option == 1)
		{
			drawText(window_width * 0.2 - 23, window_height * 0.88 - 8, "Play", Vector3(0, 0, 0), 2);
			drawText(window_width / 2 - 62, window_height * 0.88 - 8, "Instructions", Vector3(0, 255, 0), 2);
			drawText(window_width * 0.8 - 23, window_height * 0.88 - 8, "Exit", Vector3(0, 0, 0), 2);
		}
		else if (option == 2)
		{
			drawText(window_width * 0.2 - 23, window_height * 0.88 - 8, "Play", Vector3(0, 0, 0), 2);
			drawText(window_width / 2 - 62, window_height * 0.88 - 8, "Instructions", Vector3(0, 0, 0), 2);
			drawText(window_width * 0.8 - 23, window_height * 0.88 - 8, "Exit", Vector3(0, 255, 0), 2);
		}

		block->disable();
	}
	else
	{
		drawText(120, window_height / 2 - 100, "TOWER DEFENSE", Vector3(255, 0, 0), 7);
		if (!hide)
		{
			drawText(window_width*0.25, window_height / 2 + 100, "Press X to start", Vector3(255, 255, 255), 5);
		}

		textures->disable();
	}
	/*
	Vector4 color = Vector4(255, 0, 0, 0);
	Texture* texture = World::getInstance()->mapGenerator->textures["rocks"];

	block->enable();
	block->setTexture("u_texture", texture);
	block->setUniform("u_model", model);
	block->setUniform("u_viewprojection", camera->viewprojection_matrix);
	block->setUniform("u_color", color);
	gui->render(GL_TRIANGLES);

	color = Vector4(0, 0, 255, 0);
	block->setUniform("u_color", color);

	playBtn->render(GL_TRIANGLES);
	instructionsBtn->render(GL_TRIANGLES);
	exitBtn->render(GL_TRIANGLES);

	if (option == 0)
	{
		drawText(window_width*0.3 + 130, window_height*0.25 - 6, "Play", Vector3(0, 255, 0), 2);
		drawText(window_width*0.3 + 100, window_height*0.45 - 6, "Instructions", Vector3(0, 0, 0), 2);
		drawText(window_width*0.3 + 130, window_height*0.75 - 6, "Exit", Vector3(0, 0, 0), 2);
	}
	else if (option == 1)
	{
		drawText(window_width*0.3 + 130, window_height*0.25 - 6, "Play", Vector3(0, 0, 0), 2);
		drawText(window_width*0.3 + 100, window_height*0.45 - 6, "Instructions", Vector3(0, 255, 0), 2);
		drawText(window_width*0.3 + 130, window_height*0.75 - 6, "Exit", Vector3(0, 0, 0), 2);
	}
	else if (option == 2)
	{
		drawText(window_width*0.3 + 130, window_height*0.25 - 6, "Play", Vector3(0, 0, 0), 2);
		drawText(window_width*0.3 + 100, window_height*0.45 - 6, "Instructions", Vector3(0, 0, 0), 2);
		drawText(window_width*0.3 + 130, window_height*0.75 - 6, "Exit", Vector3(0, 255, 0), 2);
	}

	block->disable();
	*/
}

void Menu::update(float dt)
{
	if (keypressed == false)
	{
		counter -= dt;
		if (counter < 0)
		{
			if (hide == true)
			{
				hide = false;
			}
			else
			{
				hide = true;
			}
			counter = 1.0f;
		}
	}
}


HUD::HUD() : Stage()
{
	gui = new Mesh();
	gui->createQuad(0.0f, window_height / 2 - 25.0f, window_width, 50.0f, false);
}

void HUD::render(Camera* camera)
{

	Matrix44 model;
	Vector4 color = Vector4(1, 1, 1, 1);
	texture = World::getInstance()->mapGenerator->textures["wall"];

	textures->enable();
	textures->setUniform("u_model", model);
	textures->setUniform("u_viewprojection", camera->viewprojection_matrix);
	textures->setTexture("u_texture", texture);
	textures->setUniform("u_color", color);
	gui->render(GL_TRIANGLES);

	textures->disable();

	drawText(65, 20, "Player 1 gold:", Vector3(1, 1, 1), 2);
	drawText(235, 20, std::to_string((int)World::getInstance()->player2->gold), Vector3(1, 1, 1), 2);

	drawText(window_width - 275.0f, 20, "Player 2 gold:", Vector3(1, 1, 1), 2);
	drawText(window_width - 105.0f, 20, std::to_string((int)World::getInstance()->player1->gold), Vector3(1, 1, 1), 2);

	if(Game::instance->playing == false)
	{
		drawText(window_width / 2.0f - 80.0f, 20, "Starting in: ", Vector3(1, 1, 1), 2);
		drawText(window_width / 2.0f + 50.0f, 20, std::to_string((int)(Game::instance->timeToStart)), Vector3(1, 1, 1), 2);
	}
}

void HUD::update(float dt)
{
}

Score::Score()
{
	gui = new Mesh();
	gui->createQuad(0.0f, 0.0f, window_width*0.8f, window_height*1.0f, false);

	continueBtn = new Mesh();
	continueBtn->createQuad(0.0f, -window_height * 0.25, window_width*0.4f, window_height*0.05f, false);
}

void Score::render(Camera* camera)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	Matrix44 model;
	Vector4 color = Vector4(1, 1, 1, 1);

	if (Game::instance->winner == 0) { texture = World::getInstance()->mapGenerator->textures["player1"]; }
	else texture = World::getInstance()->mapGenerator->textures["player2"];

	textures->enable();
	textures->setUniform("u_model", model);
	textures->setUniform("u_viewprojection", camera->viewprojection_matrix);
	textures->setTexture("u_texture", texture);
	textures->setUniform("u_color", color);
	gui->render(GL_TRIANGLES);

	textures->disable();

	block->enable();
	color = Vector4(0, 0, 255, 0);
	block->setUniform("u_color", color);

	//continueBtn->render(GL_TRIANGLES);
	block->disable();

	if(Game::instance->winner == 0)
	{
		drawText(window_width*0.3 + 100, window_height*0.45 - 6, "Player 1 wins!", Vector3(0, 0, 0), 2);
	}
	else
	{
		drawText(window_width*0.3 + 100, window_height*0.45 - 6, "Player 2 wins!", Vector3(0, 0, 0), 2);
	}

	drawText(window_width*0.3 + 130, window_height*0.75 - 6, "Continue", Vector3(0, 255, 0), 2);
}

void Score::update(float dt) {}

Instructions::Instructions()
{
	option = 0;
	gui = new Mesh();
	gui->createQuad(0.0f, -window_height / 2 * 0.75, window_width, window_height*0.30, false);

	background = new Texture();
	background = Texture::Get("data/GUI/wall.tga");

	controllerBtn = new Mesh();
	controllerBtn->createQuad(-window_width / 2 * 0.6, -window_height / 2 * 0.75, window_width*0.2f, window_height*0.1f, false);

	keyboardBtn = new Mesh();
	keyboardBtn->createQuad(0.0f, -window_height / 2 * 0.75, window_width*0.2f, window_height*0.1f, false);

	cancelBtn = new Mesh();
	cancelBtn->createQuad(window_width / 2 * 0.6, -window_height / 2 * 0.75, window_width*0.2f, window_height*0.1f, false);
}

void Instructions::render(Camera* camera)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	Matrix44 model;

	textures->enable();
	textures->setUniform("u_model", model);
	textures->setUniform("u_viewprojection", camera->viewprojection_matrix);
	textures->setUniform("u_color", Vector4(1, 1, 1, 1));
	textures->setUniform("u_mode", 0.0f);

	textures->setTexture("u_texture", background);

	gui->render(GL_TRIANGLES);

	textures->disable();

	Vector4 color = Vector4(0, 0, 255, 0);

	block->enable();
	block->setUniform("u_model", model);
	block->setUniform("u_viewprojection", camera->viewprojection_matrix);
	block->setUniform("u_color", color);

	controllerBtn->render(GL_TRIANGLES);
	keyboardBtn->render(GL_TRIANGLES);
	cancelBtn->render(GL_TRIANGLES);

	if (option == 0)
	{
		drawText(window_width * 0.2 - 47, window_height * 0.88 - 8, "Controller", Vector3(0, 255, 0), 2);
		drawText(window_width / 2 - 48, window_height * 0.88 - 8, "Keyboard", Vector3(0, 0, 0), 2);
		drawText(window_width * 0.8 - 30, window_height * 0.88 - 8, "Cancel", Vector3(0, 0, 0), 2);
	}
	else if (option == 1)
	{
		drawText(window_width * 0.2 - 47, window_height * 0.88 - 8, "Controller", Vector3(0, 0, 0), 2);
		drawText(window_width / 2 - 48, window_height * 0.88 - 8, "Keyboard", Vector3(0, 255, 0), 2);
		drawText(window_width * 0.8 - 30, window_height * 0.88 - 8, "Cancel", Vector3(0, 0, 0), 2);
	}
	else if (option == 2)
	{
		drawText(window_width * 0.2 - 47, window_height * 0.88 - 8, "Controller", Vector3(0, 0, 0), 2);
		drawText(window_width / 2 - 48, window_height * 0.88 - 8, "Keyboard", Vector3(0, 0, 0), 2);
		drawText(window_width * 0.8 - 30, window_height * 0.88 - 8, "Cancel", Vector3(0, 255, 0), 2);
	}

	block->disable();
}

void Instructions::update(float dt) {}

Controller::Controller()
{
	screen = 0;
	device = 0;
	quad1 = new Mesh();
	quad1->createQuad(-window_width / 2 + 140, window_height / 2 * 0.5, 200, 140, false);
	quad2 = new Mesh();
	quad2->createQuad(-window_width / 2 + 140, -window_height / 2 * 0.3, 189, 150, false);

	quad3 = new Mesh();
	quad3->createQuad(-window_width / 2 + 140, window_height / 2 * 0.5, 225, 225, false);
	quad4 = new Mesh();
	quad4->createQuad(-window_width / 2 + 140, -window_height / 2 * 0.3, 206, 140, false);

	nextBtn = new Mesh();
	nextBtn->createQuad(window_width / 2 * 0.6, -window_height / 2 * 0.75, window_width*0.2f, window_height*0.1f, false);

	texture1 = Texture::Get("data/GUI/build.tga");
	texture2 = Texture::Get("data/GUI/nexus.tga");
	texture3 = Texture::Get("data/GUI/collector.tga");
	texture4 = Texture::Get("data/GUI/defense.tga");
}

void Controller::render(Camera* camera)
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	Matrix44 model;

	textures->enable();
	textures->setUniform("u_model", model);
	textures->setUniform("u_viewprojection", camera->viewprojection_matrix);
	textures->setUniform("u_color", Vector4(1, 1, 1, 1));
	textures->setUniform("u_mode", 0.0f);

	if (screen == 0)
	{
		textures->setTexture("u_texture", texture1);
		quad1->render(GL_TRIANGLES);

		textures->setTexture("u_texture", texture2);
		quad2->render(GL_TRIANGLES);

		if (device == 0)
		{
			drawText(window_width * 0.34, window_height * 0.15, "Press triangle to enter build mode.", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.25, "Change the structure you want to build with", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.35, " square and press circle to start building it.", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.5, "If the Nexus is destroyed, you lose.", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.6, "You can spawn minions from the nexus", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.7, "with L1 and R1.", Vector3(1, 1, 1), 2);
		}
		else
		{
			drawText(window_width * 0.34, window_height * 0.15, "Press 1 to switch between build and spawn mode.", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.25, "Change the structure you want to build (2-3)", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.35, "and left click on the ground to start building it.", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.5, "If the Nexus is destroyed, you lose.", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.6, "You can spawn minions from the nexus", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.7, "with 2 and 3 (in spawn mode).", Vector3(1, 1, 1), 2);
		}
	}
	else
	{
		textures->setTexture("u_texture", texture3);
		quad3->render(GL_TRIANGLES);

		textures->setTexture("u_texture", texture4);
		quad4->render(GL_TRIANGLES);

		if (device == 0)
		{
			drawText(window_width * 0.34, window_height * 0.15, "Collectors costs 200 gold", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.25, "and they produce gold every second.", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.5, "Defenses attack enemy minions and buildings", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.6, "at certain range. They are harder", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.7, "to destroy than collectors.", Vector3(1, 1, 1), 2);
		}
		else
		{
			drawText(window_width * 0.34, window_height * 0.15, "Collectors costs 200 gold", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.25, "and they produce gold every second.", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.5, "Defenses attack enemy minions and buildings", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.6, "at certain range. They are harder", Vector3(1, 1, 1), 2);
			drawText(window_width * 0.34, window_height * 0.7, "to destroy than collectors.", Vector3(1, 1, 1), 2);
		}
	}

	textures->disable();

	Vector4 color = Vector4(0, 0, 255, 0);

	block->enable();
	block->setUniform("u_model", model);
	block->setUniform("u_viewprojection", camera->viewprojection_matrix);
	block->setUniform("u_color", color);

	nextBtn->render(GL_TRIANGLES);

	block->disable();

	drawText(window_width * 0.8 - 22, window_height * 0.88 - 6, "Continue", Vector3(0, 255, 0), 2);
}

void Controller::update(float dt) {}