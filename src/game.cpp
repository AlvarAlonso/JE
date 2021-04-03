#include "game.h"
#include "utils.h"
#include "fbo.h"
#include "world.h"
#include <cmath>

HSAMPLE hSample;
HSAMPLE music;

HCHANNEL hSampleChannel;
HCHANNEL musicChannel;

enum inputMode
{
	keyboard = 0,
	controller
};

//some globals
bool clicked = false;
Mesh* mesh = NULL;
Texture* texture = NULL;
Shader* shader = NULL;
float angle = 0;
item selected = nexus;
Game* Game::instance = NULL;
Mesh* GUI = NULL;
//int inputDevice = keyboard; 

Game::Game(int window_width, int window_height, SDL_Window* window)
{
	this->window_width = window_width;
	this->window_height = window_height;
	this->window = window;
	instance = this;
	must_exit = false;

	fps = 0;
	frame = 0;
	time = 0.0f;
	elapsed_time = 0.0f;
	mouse_locked = false;

	//Initialize BASS
	if( BASS_Init(2, 44100, 0, 0, NULL) == false)
	{
		printf("Error when initializing BASS\n");
	}

	hSample = BASS_SampleLoad(false, "data/sounds/countdown.wav", 0, 0, 4, 2);
	music = BASS_SampleLoad(false, "data/sounds/sc.wav", 0, 0, 4, 2);

	//OpenGL flags
	glEnable( GL_CULL_FACE ); //render both sides of every triangle
	glEnable( GL_DEPTH_TEST ); //check the occlusions using the Z buffer

	//create our camera
	global_camera = new Camera();
	
	global_camera->lookAt(Vector3(56.f, 44.f, 52.f), Vector3(136.0f, 7.f, -58.0f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
	global_camera->setPerspective(70.f, window_width / (float)window_height, 0.1f, 10000.f); //set the projection, we want to be perspective
	camera = global_camera;
	
	orthographic_camera = new Camera();
	orthographic_camera->setOrthographic(-window_width/2, window_width/2, -window_height/2, window_height/2, -100, 100);

	//Stages
	stage = 0;
	menu = new Menu();
	hud = new HUD();
	score = new Score();
	instructions = new Instructions();
	cont = new Controller();
	timeToStart = 5.0f;
	
	//players
	World::getInstance()->player1 = new Player(0);
	World::getInstance()->player2 = new Player(1);
	
	//init game
	gameInit();

	//hide the cursor
	SDL_ShowCursor(!mouse_locked); //hide or show the mouse
}

void Game::gameInit() {
	World::getInstance()->generateMap();
	World::getInstance()->player2->nexus = (Nexus*)World::getInstance()->textureMaps.grid[4][0].building;
	World::getInstance()->player1->nexus = (Nexus*)World::getInstance()->textureMaps.grid[3][7].building;
	a = b = i = j = 0;
	mode = 0;
	prev_hat = PAD_CENTERED;
	build_type = 0;
	musicChannel = BASS_SampleGetChannel(music, false);
	BASS_ChannelPlay(musicChannel, true);
}

//what to do when the image has to be draw
void Game::render(void)
{
	//set the clear color (the background color)
	glClearColor(0.0, 0.0, 0.0, 1.0);

	// Clear the window and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//set the camera as default
	camera->enable();

	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	if (stage == 0)
	{
		World::getInstance()->render(camera);
		menu->render(orthographic_camera);
	}
	else if (stage == 1)
	{
		World::getInstance()->render(camera);
		hud->render(orthographic_camera);

		//Draw the floor grid
		//drawGrid();
	}
	else if (stage == 2)
	{
		score->render(orthographic_camera);
	}
	else if (stage == 3)
	{
		World::getInstance()->render(camera);
		instructions->render(orthographic_camera);
	}
	else if (stage == 4)
	{
		cont->render(orthographic_camera);
	}

	
	//render the FPS, Draw Calls, etc
	//drawText(2, 2, getGPUStats(), Vector3(1, 1, 1), 2);

	//swap between front buffer and back buffer
	SDL_GL_SwapWindow(this->window);
}

void Game::update(double seconds_elapsed)
{


	float speed = seconds_elapsed * 100; //the speed is defined by the seconds_elapsed so it goes constant
	if (stage == 0) {
		menu->update(elapsed_time);
		if (Input::gamepads[0].hat != prev_hat) {
			if (Input::gamepads[0].hat == PAD_RIGHT) {
				menu->option++;
				if (menu->option > 2) menu->option = 0;
			}
			if (Input::gamepads[0].hat == PAD_LEFT) {
				menu->option--;
				if (menu->option < 0) menu->option = 2;
			}
		}
	}
	else if (stage == 3) {
		menu->update(elapsed_time);
		if (Input::gamepads[0].hat != prev_hat) {
			if (Input::gamepads[0].hat == PAD_RIGHT) {
				instructions->option += 1;
				if (instructions->option > 2) instructions->option = 0;
			}
			if (Input::gamepads[0].hat == PAD_LEFT) {
				instructions->option -= 1;
				if (instructions->option < 0) instructions->option = 2;
			}
		}
	}
	else if (stage == 1)
	{
		World::getInstance()->update(seconds_elapsed);
		if(playing)
		{
			World::getInstance()->player1->update(seconds_elapsed);
			World::getInstance()->player2->update(seconds_elapsed);
		}
		else
		{
			timeToStart -= seconds_elapsed;

			if (timeToStart < 0)
			{
				timeToStart = 5.0f;
				playing = true;
			}
		}
		if (World::getInstance()->preview != NULL && playing)
		{

			Vector2 mouse_click = Input::mouse_position;
			Vector3 direction = camera->getRayDirection(mouse_click.x, mouse_click.y, window_width, window_height);
			Vector3 col_point;
			Vector3 col_normal;
			Vector3 pos;
			EntityTerrain* floor = World::getInstance()->map;

			if (floor->mesh->testRayCollision(floor->m, camera->eye, direction, col_point, col_normal, 1000, false) == true) {
				pos = col_point;
				int htype;
				int a = (int)pos.x / 40;
				int b = World::getInstance()->genParams.cells + (int)pos.z / 40 - 1;

				Vector3 finalPos = Vector3(20.0f + std::floor(pos.x / 40.0f) * 40.0f, 0.0f, -20.0f + std::floor(pos.z / 40.0f)*40.0f + 40.0f);
				Matrix44 mat;
				mat.setIdentity();
				mat.translate(finalPos.x - (40.0f / 2.0f - 16.0f), finalPos.y, finalPos.z - (33.0f / 2.0f - 8.0f));
				World::getInstance()->preview->m = mat;

				if (!World::getInstance()->textureMaps.grid[a][b].isBuilded)
				{
					World::getInstance()->preview->canPlace = true;
				}
				else
				{
					//printf("Can't build!\n");
					World::getInstance()->preview->canPlace = false;
				}
			}
		}
		if (World::getInstance()->preview2 != NULL && playing) 
		{
			if (stage == 1) {
				if (Input::gamepads[0].hat != prev_hat) {
					if (Input::gamepads[0].hat == PAD_UP) { if (i < 7) i++; }
					if (Input::gamepads[0].hat == PAD_DOWN) { if (i > 0) i--; }
					if (Input::gamepads[0].hat == PAD_RIGHT) { if (j < 7) j++; }
					if (Input::gamepads[0].hat == PAD_LEFT) { if (j > 0) j--; }
				}

				Vector3 finalPos = Vector3(i * 40 + 20, 0.0f, (j - 7) * 40 - 20);
				Matrix44 mat;
				mat.setIdentity();
				mat.translate(finalPos.x - (40.0f / 2 - 16.0f), finalPos.y, finalPos.z - (33.0f / 2.0f - 8.0f));
				World::getInstance()->preview2->m = mat;

				if (!World::getInstance()->textureMaps.grid[i][j].isBuilded)
				{
					World::getInstance()->preview2->canPlace = true;
				}
				else
				{
					//printf("Can't build!\n");
					World::getInstance()->preview2->canPlace = false;
				}
			}
		}
	}
	else if (stage == 2)
	{
		score->update(elapsed_time);
	}
	/*
	//keyboard
	if (true)//inputDevice == keyboard) 
	{
		//mouse input to rotate the cam	
		if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
		{
			camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
			camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
		}

		//async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift
		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);
	}
	*/
	/*
	else if (inputDevice == controller) {
		//camera movement controller
		if (Input::gamepads[0].axis[LEFT_ANALOG_X] < -0.5) { camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed); }
		else if (Input::gamepads[0].axis[LEFT_ANALOG_X] > 0.5) { camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed); }
		else if (Input::gamepads[0].axis[LEFT_ANALOG_Y] > 0.5) { camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed); }
		else if (Input::gamepads[0].axis[LEFT_ANALOG_Y] < -0.5) { camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed); }
		//camera rotation controller
		if (Input::gamepads[0].axis[RIGHT_ANALOG_X] < -0.5) {
			angle += 25 * seconds_elapsed;
			camera->rotate(angle*DEG2RAD, Vector3(0.0f, -1.0f, 0.0f));
			angle = 0;
		}
		else if (Input::gamepads[0].axis[RIGHT_ANALOG_X] > 0.5) {
			angle -= 25 * seconds_elapsed;
			camera->rotate(angle*DEG2RAD, Vector3(0.0f, -1.0f, 0.0f));
			angle = 0;
		}
		else if (Input::gamepads[0].axis[RIGHT_ANALOG_Y] > 0) {
			angle += 25 * seconds_elapsed;
			camera->rotate(angle*DEG2RAD, Vector3(-1.0f, 0.0f, 0.0f));
			angle = 0;
		}
		else if (Input::gamepads[0].axis[RIGHT_ANALOG_Y] < 0) {
			angle -= 25 * seconds_elapsed;
			camera->rotate(angle*DEG2RAD, Vector3(-1.0f, 0.0f, 0.0f));
			angle = 0;
		}
	}
	*/

	prev_hat = Input::gamepads[0].hat;

	//to navigate with the mouse fixed in the middle
	if (mouse_locked)
		Input::centerMouse();

	if(gameOver)
	{
		World::getInstance()->resetGame();
		playing = false;
		gameOver = false;
		stage = 2;
		printf("To stage 2\n");
	}
}

//Keyboard event handler (sync input)
void Game::onKeyDown( SDL_KeyboardEvent event )
{
	switch (event.keysym.sym)
	{
	case SDLK_ESCAPE: must_exit = true; break;
	}

	if(stage == 1)
	{
		switch (event.keysym.sym)
		{
		case SDLK_F1: Shader::ReloadAll(); break;
		}
	}
}

void Game::onKeyUp(SDL_KeyboardEvent event)
{
	if (stage == 0)
	{
		switch (event.keysym.sym)
		{
		case SDLK_LEFT:
			menu->option -= 1;
			if (menu->option < 0)
			{
				menu->option = 2;
			}
			break;
		case SDLK_RIGHT:
			menu->option += 1;
			if (menu->option > 2)
			{
				menu->option = 0;
			}
			break;
		case SDLK_x:
			if (menu->keypressed) {
				if (menu->option == 0)
				{
					printf("To stage 1\n");
					stage = 1;
					global_camera->lookAt(Vector3(-120.f, 200.f, -160.f), Vector3(175.f, 0.f, -160.f), Vector3(0.f, 1.f, 0.f)); //position the camera and point to 0,0,0
					hSampleChannel = BASS_SampleGetChannel(hSample, false);
					BASS_ChannelPlay(hSampleChannel, false);
				}
				else if (menu->option == 1)
				{
					stage = 3;
				}
				else if (menu->option == 2)
				{
					must_exit = true;
				}
			}
			else {
				menu->keypressed = true;
			}
			break;
		}
	}
	else if (stage == 1 && playing)
	{
		switch (event.keysym.sym)
		{
		case SDLK_1:
			if (mode == 0)
			{
				if(World::getInstance()->preview != NULL)
				{
					World::getInstance()->preview->~EntityBuilding();
					World::getInstance()->preview = NULL;
				}
				mode = 1;
			}
			else
			{
				mode = 0;
			}
			break;
		case SDLK_2:
			if(mode == 0)
			{
				if (World::getInstance()->preview != NULL)
				{
					World::getInstance()->preview->~EntityBuilding();
				}
				World::getInstance()->preview = World::getInstance()->createBuilding(Vector3(0, 0, 0), collector);
				World::getInstance()->preview->alive = false;
			}
			else if (World::getInstance()->player1->canSpawnL == true && World::getInstance()->player1->gold >= 50)
			{
				World::getInstance()->player1->gold -= 50;
				World::getInstance()->player1->nexus->spawnMinions(LEFT);
				World::getInstance()->player1->canSpawnL = false;
			}
			break;
		case SDLK_3:
			if(mode == 0)
			{
				if (World::getInstance()->preview != NULL)
				{
					World::getInstance()->preview->~EntityBuilding();
				}
				World::getInstance()->preview = World::getInstance()->createBuilding(Vector3(0, 20, 0), defense);
				World::getInstance()->preview->alive = false;
			}
			else if(World::getInstance()->player1->canSpawnR == true && World::getInstance()->player1->gold >= 50)
			{
				World::getInstance()->player1->gold -= 50;
				World::getInstance()->player1->nexus->spawnMinions(RIGHT);
				World::getInstance()->player1->canSpawnR = false;
			}
			break;
		case SDLK_SPACE:
			if (World::getInstance()->currentPlayer == World::getInstance()->player1)
			{
				World::getInstance()->currentPlayer = World::getInstance()->player2;
				printf("using player2\n");
			}
			else
			{
				World::getInstance()->currentPlayer = World::getInstance()->player1;
				printf("using player1\n");
			}
			break;
		case SDLK_p:
			printf("player1: %f\n", World::getInstance()->player1->gold);
			printf("player2: %f\n", World::getInstance()->player2->gold);
			break;
		}
	}
	else if(stage == 2)
	{
		switch (event.keysym.sym)
		{
		case SDLK_x:
			winner = -1;
			stage = 0;
			global_camera->lookAt(Vector3(56.f, 44.f, 52.f), Vector3(136.0f, 7.f, -58.0f), Vector3(0.f, 1.f, 0.f));
			printf("To stage 0\n");
			break;
		}
	}
	else if (stage == 3) {
		switch (event.keysym.sym)
		{
		case SDLK_LEFT:
			instructions->option -= 1;
			if (instructions->option < 0)
			{
				instructions->option = 2;
			}
			break;
		case SDLK_RIGHT:
			instructions->option += 1;
			if (instructions->option > 2)
			{
				instructions->option = 0;
			}
			break;
		case SDLK_x:
			if (instructions->option == 0)
			{
				cont->device = 0;
				stage = 4;
			}
			else if (instructions->option == 1)
			{
				cont->device = 1;
				stage = 4;
			}
			else
			{
				stage = 0;
			}
			instructions->option = 0;
		}
	}
	else if (stage == 4) {
		switch (event.keysym.sym)
		{
		case SDLK_x:
			if (cont->screen == 0)
			{
				cont->screen = 1;
			}
			else
			{
				cont->screen = 0;
				stage = 3;
			}
		}
	}
}

void Game::onGamepadButtonDown(SDL_JoyButtonEvent event)
{

}

void Game::onGamepadButtonUp(SDL_JoyButtonEvent event)
{
	if (Input::gamepads[0].connected) {

		//std::cout << "gamepad mierder connected" << std::endl;

	switch (event.button) {
	case SDL_CONTROLLER_BUTTON_B:
		if (menu->keypressed) {
			if (stage == 0) {
				if (menu->option == 0)
				{
					printf("To stage 1\n");
					stage = 1;
					global_camera->lookAt(Vector3(-120.f, 200.f, -160.f), Vector3(175.f, 0.f, -160.f), Vector3(0.f, 1.f, 0.f));
					hSampleChannel = BASS_SampleGetChannel(hSample, false);
					BASS_ChannelPlay(hSampleChannel, false);
				}
				else if (menu->option == 1)
				{
					std::cout << "menu option 1" << std::endl;
					stage = 3;
				}
				else if (menu->option == 2)
				{
					must_exit = true;
				}
			}
		}
		else
		{
			menu->keypressed = true;
		}
		
		if (stage == 2) {
			winner = -1;
			stage = 0;
			global_camera->lookAt(Vector3(56.f, 44.f, 52.f), Vector3(136.0f, 7.f, -58.0f), Vector3(0.f, 1.f, 0.f));
		}
		else if (stage == 3) {
			std::cout << "in stage 3" << std::endl;
			std::cout << instructions->option << std::endl;
			if (instructions->option == 0) {
				cont->device = 0;
				stage = 4;
			}
			else if (instructions->option == 1)
			{
				cont->device = 1;
				stage = 4;
			}
			else
			{
				stage = 0;
			}
			instructions->option = 0;
		}
		else if (stage == 4)
		{
			std::cout << "in stage 4" << std::endl;
			if (cont->screen == 0)
			{
				std::cout << "screen 0" << std::endl;
				cont->screen = 1;
			}
			else
			{
				std::cout << "screen 1" << std::endl;
				cont->screen = 0;
				stage = 3;
			}	
		}
		break;
	case SDL_BUTTON_X1: //R1
		std::cout << "entra al pitjar x" << std::endl;
		if (World::getInstance()->player2->canSpawnR && playing && World::getInstance()->player2->gold >= 50) {
			World::getInstance()->player2->gold -= 50;
			World::getInstance()->player2->nexus->spawnMinions(RIGHT);
			World::getInstance()->player2->canSpawnR = false;
		}
		break;
	case SDL_BUTTON_X2: //L1
		if (World::getInstance()->player2->canSpawnL && playing && World::getInstance()->player2->gold >= 50) {
			World::getInstance()->player2->gold -= 50;
			World::getInstance()->player2->nexus->spawnMinions(LEFT);
			World::getInstance()->player2->canSpawnL = false;
		}
		break;

	case SDL_CONTROLLER_BUTTON_Y: //triangle
		std::cout << "entra triangle" << std::endl;
		//entrar mode building o no
		if (playing) {
			if (mode == 0)
			{
				if (World::getInstance()->preview2 != NULL)
				{
					World::getInstance()->preview2->~EntityBuilding();
					World::getInstance()->preview2 = NULL;
				}

				mode = 1;
			}
			else //mode == 1
			{

				if (World::getInstance()->preview2 != NULL)
				{
					World::getInstance()->preview2->~EntityBuilding();
				}

				if (build_type % 2 == 0) World::getInstance()->preview2 = World::getInstance()->createBuilding(Vector3(0, 0, 0), collector);
				else { World::getInstance()->preview2 = World::getInstance()->createBuilding(Vector3(0, 20, 0), defense); }
				World::getInstance()->preview2->alive = false;
				mode = 0;
			}
		}
		break;
	case SDL_CONTROLLER_BUTTON_A: //square
		if (playing) {
			build_type++;
			if (World::getInstance()->preview2 != NULL)
			{
				World::getInstance()->preview2->~EntityBuilding();
			}

			if (build_type % 2 == 0) World::getInstance()->preview2 = World::getInstance()->createBuilding(Vector3(0, 0, 0), collector);
			else { World::getInstance()->preview2 = World::getInstance()->createBuilding(Vector3(0, 20, 0), defense); }
			World::getInstance()->preview2->alive = false;
		}	
		break;
	case SDL_CONTROLLER_BUTTON_X:
		if (stage == 1 && playing) 
		{
			if (World::getInstance()->preview2 != NULL)
			{
				std::cout << "preview 2 is not null " << std::endl;
				Vector3 pos;
				EntityTerrain* floor = World::getInstance()->map;

				if (!World::getInstance()->textureMaps.grid[i][j].isBuilded)
				{
					std::cout << "if is not builded " << std::endl;
					if (i >= 0 && i < World::getInstance()->genParams.cells && j >= 0 && j < World::getInstance()->genParams.cells
						&& World::getInstance()->preview2->cost <= World::getInstance()->player2->gold)
					{
						World::getInstance()->preview2->beingBuilded = true;
						World::getInstance()->preview2->alive = true;
						World::getInstance()->textureMaps.grid[i][j].isBuilded = true;
						World::getInstance()->textureMaps.grid[i][j].content = World::getInstance()->preview2->type;
						World::getInstance()->textureMaps.grid[i][j].idPlayer = World::getInstance()->player2->id;
						World::getInstance()->preview2->owner = World::getInstance()->player2->id;
						World::getInstance()->preview2->gridPosition.x = i;
						World::getInstance()->preview2->gridPosition.y = j;
						World::getInstance()->textureMaps.grid[i][j].building = World::getInstance()->preview2;
						World::getInstance()->player2->gold -= World::getInstance()->preview2->cost;
						World::getInstance()->preview2 = NULL;
					}
				}
			}
		}
		break;
	}
	}
}

void Game::onMouseButtonDown( SDL_MouseButtonEvent event )
{

}


void Game::onMouseButtonUp(SDL_MouseButtonEvent event)
{

	if (event.button == SDL_BUTTON_LEFT && World::getInstance()->preview != NULL && playing)
	{
		//clicked = true;
		Vector2 mouse_click = Input::mouse_position;
		Vector3 direction = camera->getRayDirection(mouse_click.x, mouse_click.y, window_width, window_height);
		Vector3 col_point;
		Vector3 col_normal;
		Vector3 pos;
		EntityTerrain* floor = World::getInstance()->map;

		if (floor->mesh->testRayCollision(floor->m, camera->eye, direction, col_point, col_normal, 1000, false) == true) {
			pos = col_point;
			int htype;
			int a = (int)pos.x / 40;
			int b = World::getInstance()->genParams.cells + (int)pos.z / 40 - 1;

			if (!World::getInstance()->textureMaps.grid[a][b].isBuilded)
			{
				if (a >= 0 && a < World::getInstance()->genParams.cells && b >= 0 && b < World::getInstance()->genParams.cells
					&& World::getInstance()->preview->cost <= World::getInstance()->player1->gold)
				{	
					World::getInstance()->preview->beingBuilded = true;
					World::getInstance()->preview->alive = true;
					World::getInstance()->textureMaps.grid[a][b].isBuilded = true;
					World::getInstance()->textureMaps.grid[a][b].content = World::getInstance()->preview->type;
					World::getInstance()->textureMaps.grid[a][b].idPlayer = World::getInstance()->player1->id;
					World::getInstance()->preview->owner = World::getInstance()->player1->id;
					if(World::getInstance()->preview->type == defense) World::getInstance()->preview->texture = World::getInstance()->mapGenerator->textures["blue_energy"];
					World::getInstance()->preview->gridPosition.x = a; 
					World::getInstance()->preview->gridPosition.y = b;
					World::getInstance()->textureMaps.grid[a][b].building = World::getInstance()->preview;
					World::getInstance()->player1->gold -= World::getInstance()->preview->cost;
					World::getInstance()->preview = NULL;
					
				}
			}
		}
	}
}

void Game::onMouseWheel(SDL_MouseWheelEvent event)
{
}


void Game::onResize(int width, int height)
{
    std::cout << "window resized: " << width << "," << height << std::endl;
	glViewport( 0,0, width, height );
	camera->aspect =  width / (float)height;
	window_width = width;
	window_height = height;
}
