/*  by Javi Agenjo 2013 UPF  javi.agenjo@gmail.com
	This class encapsulates the game, is in charge of creating the game, getting the user input, process the update and render.
*/

#ifndef GAME_H
#define GAME_H

#include "includes.h"
#include "framework.h"
#include "utils.h"
#include "stage.h"

class Game
{
public:
	static Game* instance;

	//window
	SDL_Window* window;
	int window_width;
	int window_height;

	//some globals
	long frame;
    float time;
	float elapsed_time;
	int fps;
	bool must_exit;
	int stage;
	Menu* menu;
	HUD *hud;
	Score* score;
	Instructions* instructions;
	Controller* cont;
	bool playing = false;
	float timeToStart;
	int mode;
	int build_type;
	int a, b, i, j;
	int prev_hat;

	//game over
	bool gameOver = false;
	int winner;

	//some vars
	Camera* camera; //our global camera
	Camera* global_camera;
	Camera* player_camera;
	Camera* orthographic_camera;
	bool mouse_locked; //tells if the mouse is locked (not seen)

	Game( int window_width, int window_height, SDL_Window* window );

	void gameInit();

	//main functions
	void render( void );
	void update( double dt );

	//events
	void onKeyDown( SDL_KeyboardEvent event );
	void onKeyUp(SDL_KeyboardEvent event);
	void onMouseButtonDown( SDL_MouseButtonEvent event );
	void onMouseButtonUp(SDL_MouseButtonEvent event);
	void onMouseWheel(SDL_MouseWheelEvent event);
	void onGamepadButtonDown(SDL_JoyButtonEvent event);
	void onGamepadButtonUp(SDL_JoyButtonEvent event);
	void onResize(int width, int height);

	Mesh* createGUI();

};


#endif GAME_H