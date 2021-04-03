#include "world.h"

World* World::instance = NULL;

World::World()
{
	mapGenerator = MapGenerator::getInstance();
	bulletManager = BulletManager::getInstance();
	textureMaps.heightMap = NULL;
	textureMaps.textureMask = NULL;
	preview = NULL;
	preview2 = NULL;
	sky = NULL;

	idCharacters = 0;

	sgenerationParameters newGenParams;

	newGenParams.planeSize = 160;
	newGenParams.cells = newGenParams.planeSize*2 / 40;

	setGenerationParameters(newGenParams);
}

void World::generateMap()
{
	mapGenerator->generateMap();
	createExtras();
	sky = new Sky();
	setNexus();
}

void World::setGenerationParameters(sgenerationParameters params)
{
	genParams.planeSize = params.planeSize;
	genParams.cells = params.cells;
}

void World::render(Camera* camera)
{
	sky->render(camera);
	map->render(genParams.planeSize);
	cube_map(camera);
	renderExtras(camera);
	renderEntities(camera);
	renderCharacters(camera);
	bulletManager->render(camera);
}

void World::update(double dt) {

	sky->update(dt);
	updateCharacters(dt);
	updateWinnerState(dt);
	bulletManager->update(dt);
}

//create a character and adds it to the vector
void World::createCharacter(Vector3 pos, int owner) {
	Character* character = new Character(pos, owner);
	character->id = idCharacters;
	idCharacters++;
	characters.push_back(character);
}
EntityBuilding* World::createBuilding(Vector3 position, item type)
{
	EntityBuilding* entity;

	switch (type) 
	{
		case nexus :
			entity = new Nexus();
			entity->mesh = mapGenerator->meshes["nexus"];
			entity->texture = mapGenerator->textures["house"];
			break;
		case collector:
			entity = new Collector();
			entity->mesh = mapGenerator->meshes["house2"];
			entity->texture = mapGenerator->textures["house"];
				break;
		case defense:
			entity = new Defense();
			entity->mesh = mapGenerator->meshes["sphere"];
			entity->texture = mapGenerator->textures["energy"];
			break;
	}
	entity->type = type;
	entity->position = Vector3(position.x - (40.0f / 2.0f - 16.0f), position.y, position.z - (33.0f / 2.0f - 8.0f));
	entity->m.translate(entity->position.x, entity->position.y, entity->position.z);
	entity->shader = Shader::Get("data/shaders/basic_buildings.vs", "data/shaders/texture.fs");
	entity->repeat = 1.0f;
	entity->box.xSize = 40;
	entity->box.ySize = 33;
	entity->box.center = Vector2(position.x, position.z);

	entities.push_back(entity);
	return entity;
}

void World::resetGame()
{
	player1->gold = 500.0f;
	player2->gold = 500.0f;

	for (int i = 0; i < characters.size(); i++) 
	{
		characters[i]->~Character();
	}
	characters.clear();

	for (int i = 0; i < gridSize; i++) 
	{
		for (int j = 0; j < gridSize; j++) 
		{
			textureMaps.grid[i][j].building = NULL;
			textureMaps.grid[i][j].idPlayer = 2;
			if (i == 2 || i == 5)
			{
				textureMaps.grid[i][j].isBuilded = true;
				textureMaps.grid[i][j].content = road;
			}
			else
			{
				if(textureMaps.grid[i][j].building != NULL)
				{
					textureMaps.grid[i][j].building->~EntityBuilding();
					textureMaps.grid[i][j].idPlayer = -1;
				}
				textureMaps.grid[i][j].isBuilded = false;
				textureMaps.grid[i][j].content = empty;
			}
		}
	}

	setNexus();
	bulletManager->reset();
	printf("DONE\n");
}

//set both nexus in position
void World::setNexus()
{
	Vector3 finalPos1 = Vector3(20.0f + 4.0f*40.0f, 0.0f, -((genParams.cells - 1) * 40.0f + 20.0f));
	Vector3 finalPos2 = Vector3(20.0f + 3.0f*40.0f, 0.0f, -20.0f);

	if(player2->nexus != NULL)
	{
		player2->nexus->~Nexus();
	}
	player2->nexus = (Nexus*)createBuilding(finalPos1, nexus);
	player2->nexus->m.translate(-20, 0, 0);
	textureMaps.grid[4][0].building = player2->nexus;
	textureMaps.grid[4][0].isBuilded = true;
	textureMaps.grid[3][0].isBuilded = true;
	player2->nexus->isRunning = true;
	player2->nexus->owner = 1;

	if(player1->nexus != NULL)
	{
		player1->nexus->~Nexus();
	}
	player1->nexus = (Nexus*)createBuilding(finalPos2, nexus);
	player1->nexus->m.translate(20, 0, 0);
	textureMaps.grid[3][7].building = player1->nexus;
	textureMaps.grid[3][7].isBuilded = true;
	textureMaps.grid[4][7].isBuilded = true;
	player1->nexus->isRunning = true;
	player1->nexus->owner = 0;

	printf("Nexus setted\n");
}

//create the extras floating around the map
void World::createExtras() {
	floatingCube* cube = new floatingCube(Vector3(200, 45, 20));
	floatingCube* cube1 = new floatingCube(Vector3(300, -30, -500));
	floatingCube* cube2 = new floatingCube(Vector3(500, 60, -200));
	floatingCube* cube3 = new floatingCube(Vector3(500, 75, -400));
	floatingCube* cube4 = new floatingCube(Vector3(-10, 75, -300));
	cubes.push_back(cube);
	cubes.push_back(cube1);
	cubes.push_back(cube2);
	cubes.push_back(cube3);
	cubes.push_back(cube4);

	EntityTree* tree = new EntityTree(cube->position);
	EntityTree* tree1 = new EntityTree(cube1->position);
	EntityTree* tree2 = new EntityTree(cube2->position);
	EntityTree* tree3 = new EntityTree(cube3->position);
	EntityTree* tree4 = new EntityTree(cube4->position);
	trees.push_back(tree);
	trees.push_back(tree1);
	trees.push_back(tree2);
	trees.push_back(tree3);
	trees.push_back(tree4);
}

//render all extras floating around
void World::renderExtras(Camera* camera) {

	for (int i = 0; i < cubes.size(); i ++) {
		cubes[i]->render(camera);
		trees[i]->render();
	}
}

//create a cube to simulate depth in the plane
void World::cube_map(Camera* camera){
	Mesh* front_wall = new Mesh();
	Mesh* right_wall = new Mesh();
	front_wall->createCube();
	right_wall->createCube();
	Matrix44 model_front;
	Matrix44 model_right;
	model_front.translate(1, -30.5, -159);
	model_right.translate(160, -30.5, -1);
	model_front.scale(1, 30, 160);
	model_right.scale(160, 30, 1);
	Texture* textureSides = mapGenerator->textures["soil"];
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture_extras.fs");

	shader->enable();
	shader->setUniform("u_model", model_front);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_time", Game::instance->time);
	shader->setTexture("u_texture", textureSides);
	shader->setUniform("u_repeat", 10);
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));

	front_wall->render(GL_TRIANGLES);

	shader->setUniform("u_model", model_right);

	right_wall->render(GL_TRIANGLES);

	shader->disable();
}

//render all characters in vector
void World::renderCharacters(Camera* camera) {
	for (int i = 0; i < characters.size(); i++) {
		Character* c = characters[i];
		if (c->alive)
			c->render(camera);
	}
}

void World::updateCharacters(double dt) {
	Character* c;
	EntityMesh* ent;
	for (int i = 0; i < characters.size(); i++) {
		c = characters[i];
		if (c->alive) {
			c->update(dt);
			for (int j = 0; j < entities.size(); j++) {
				if ((entities[j]->getPosition() - c->getPosition()).length() < 100) {
					ent = entities[j];
					c->testCollision(ent, dt);
				}
			}
			for (int j = 0; j < characters.size(); j++) {
				if ((characters[j]->pos - c->getPosition()).length() < 50 && characters[j]->id != c->id && characters[j]->alive)
					c->testCollision(characters[j], dt);
			}
		}
		else {
			characters.erase(characters.begin() + i);
			std::cout << "Character deleted: " + characters.size() << std::endl;
		}
	}
}

//render all entities in vector
void World::renderEntities(Camera* camera) {
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			if (textureMaps.grid[i][j].building != NULL)
			{
				EntityBuilding* ent = textureMaps.grid[i][j].building;
				Vector3 box_center = ent->m * ent->mesh->box.center;
				Vector3 box_halfsize = ent->mesh->box.halfsize;

				if (camera->testBoxInFrustum(box_center, box_halfsize) == false)
					continue;
				if ((ent->getPosition().length() - camera->eye.length()) > 300)
					continue;

				ent->render();
			}
		}
	}
	if (preview != NULL)
	{
		preview->render();
	}
	if (preview2 != NULL) { preview2->render(); }
}

void World::updateWinnerState(double dt) {
	for (int i = 0; i < gridSize; i++)
	{
		for (int j = 0; j < gridSize; j++)
		{
			if (World::getInstance()->textureMaps.grid[i][j].building != NULL)
			{
				EntityBuilding* building = World::getInstance()->textureMaps.grid[i][j].building;
				building->update(dt);
				if (building->health <= 0.0f)
				{
					if (building->type == nexus)
					{
						if (building->owner == 0)
						{
							player1->nexus == NULL;
							Game::instance->winner = 1;
						}
						else
						{
							player2->nexus == NULL;
							Game::instance->winner = 0;
						}
						Game::instance->gameOver = true;
					}

					building->~EntityBuilding();
					World::getInstance()->textureMaps.grid[i][j].building = NULL;
					World::getInstance()->textureMaps.grid[i][j].isBuilded = false;
					World::getInstance()->textureMaps.grid[i][j].content = empty;
					World::getInstance()->textureMaps.grid[i][j].idPlayer = 2;
				}
			}
		}
	}
}