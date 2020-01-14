#include "Game.h"

Game* Game::s_pInstance = 0;

SDL_Renderer * Game::GetRenderer()
{
	return m_pRenderer;
}

glm::vec2 Game::GetMousePosition()
{
	return m_mousePosition;
}

Game::Game()
{
}

Game::~Game()
{
}

void Game::CreateGameObjects()
{
	m_pPlayer = new Player();
	enemy_ptr = new Enemy();
	m_pIsland = new Island();
	m_pOcean = new Ocean();

	for (size_t i = 0; i < m_cloudNum; i++)
	{
		m_pClouds.push_back(new Cloud());
	}
}

bool Game::Init(const char* title, int xpos, int ypos, int height, int width, bool fullscreen)
{
	int flags = 0;

	if (fullscreen)
	{
		flags = SDL_WINDOW_FULLSCREEN;
	}

	// initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) >= 0)
	{
		std::cout << "SDL Init success" << std::endl;

		// if succeeded create our window
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, height, width, flags);

		// if window creation successful create our renderer
		if (m_pWindow != 0)
		{
			std::cout << "window creation success" << std::endl;
			m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);

			if (m_pRenderer != 0) // render init success
			{
				std::cout << "renderer creation success" << std::endl;
				SDL_SetRenderDrawColor(m_pRenderer, 255, 255, 255, 255);
			}
			else
			{
				std::cout << "renderer init failure" << std::endl;
				return false; // render int fail
			}

			//TheTextureManager::Instance()->load("../../Assets/textures/animate-alpha.png", "animate", m_pRenderer);
			CreateGameObjects();
		}
		else 
		{
			std::cout << "window init failure" << std::endl;
			return false; // window init fail
		}
	}
	else
	{
		std::cout << "SDL init failure" << std::endl;
		return false; //SDL could not intialize
	}

	std::cout << "init success" << std::endl;
	m_bRunning = true; // everything initialized successfully - start the main loop

	return true;
}

void Game::Render()
{
	SDL_RenderClear(m_pRenderer); // clear the renderer to the draw colour

	m_pOcean->Draw();
	m_pIsland->Draw();
	enemy_ptr->Draw();
	m_pPlayer->Draw();

	for (Cloud* cloud : m_pClouds) {
		cloud->Draw();
	}

	SDL_RenderPresent(m_pRenderer); // draw to the screen
}

void Game::Update()
{
	m_pPlayer->Update();
	enemy_ptr->Update();
	m_pIsland->Update();
	m_pOcean->Update();
	
	Collision::squaredRadiusCheck(m_pPlayer, m_pIsland);

	for (Cloud* cloud : m_pClouds) {
		cloud->Update();
		Collision::squaredRadiusCheck(m_pPlayer, cloud);
	}

}

void Game::Clean()
{
	std::cout << "cleaning game" << std::endl;

	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
}

void Game::HandleEvents()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			m_bRunning = false;
			break;
		case SDL_MOUSEMOTION:
			m_mousePosition.x = event.motion.x;
			m_mousePosition.y = event.motion.y;
			break;
		default:
			break;
		}
	}
}