#pragma once
#ifndef __Game__
#define __Game__

// Core Libraries
#include <iostream>
#include <vector>

#include <SDL.h>
#include <SDL_image.h>

// Game Managers
#include "TextureManager.h"
#include "CollisionManager.h"

// Game Objects
#include "Player.h"
#include "Enemy.h"
#include "Island.h"
#include "Ocean.h"
#include "Cloud.h"

class Game
{
public:
	

	static Game* Instance()
	{
		if (s_pInstance == 0)
		{
			s_pInstance = new Game();
			return s_pInstance;
		}

		return s_pInstance;
	}

	// simply set the running variable to true
	void Init() { m_bRunning = true; }

	bool Init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);

	// public functions
	void Render();
	void Update();
	void HandleEvents();
	void Clean();

	// a function to access the private running variable
	bool IsRunning() { return m_bRunning; }
	

	// getters
	SDL_Renderer* GetRenderer();
	glm::vec2 GetMousePosition();

private:
	Game();
	~Game();

	SDL_Window* m_pWindow;
	SDL_Renderer* m_pRenderer;

	int m_currentFrame;

	bool m_bRunning;

	static Game* s_pInstance;

	// GameObjects
	Player* m_pPlayer;
	Enemy* enemy_ptr;
	Island* m_pIsland;
	Ocean* m_pOcean;

	// cloud game objects
	int m_cloudNum = 3;
	std::vector<Cloud*> m_pClouds;

	void createGameObjects();

	glm::vec2 m_mousePosition;
};

typedef Game TheGame;

#endif /* defined (__Game__) */

