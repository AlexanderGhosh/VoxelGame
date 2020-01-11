#pragma once
#include "Player/Player.h"
#include "World/World.h"
class Game
{
public:
	Game();
	void createGame();
	void generateWorld();
	void setupPlayer();
	void update();
private:
	World world;
	Player player;
	// blockupdates
};

