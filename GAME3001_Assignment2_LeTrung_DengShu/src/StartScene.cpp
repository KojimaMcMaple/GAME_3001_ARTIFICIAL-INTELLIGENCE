#include "StartScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"

StartScene::StartScene()
{
	StartScene::start();
}

StartScene::~StartScene()
= default;

void StartScene::draw()
{
	m_pStartLabel->draw();
	m_pInstructionsLabel->draw();
	m_pInstructionsLabel2->draw();

	m_pShip->draw();
}

void StartScene::update()
{
}

void StartScene::clean()
{
	std::cout << "Clean called on StartScene" << std::endl;
	
	delete m_pStartLabel;
	m_pStartLabel = nullptr;
	
	delete m_pInstructionsLabel;
	m_pInstructionsLabel = nullptr;
	
	delete m_pInstructionsLabel2;
	m_pInstructionsLabel2 = nullptr;

	delete m_pShip;
	m_pShip = nullptr;

	removeAllChildren();
}

void StartScene::handleEvents()
{
	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TheGame::Instance()->quit();
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				TheGame::Instance()->quit();
				break;
			case SDLK_1:
				TheGame::Instance()->changeSceneState(SceneState::PLAY_SCENE);
				break;
			case SDLK_2:
				TheGame::Instance()->changeSceneState(SceneState::END_SCENE);
				break;
			}
			break;

		default:
			break;
		}
	}
}

void StartScene::start()
{
	SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), 20, 20, 20, 255);
	const SDL_Color sdl_color = { 255, 255, 255, 255 };
	m_pStartLabel = new Label("Assignment 02", "Consolas", 80, sdl_color, glm::vec2(400.0f, 40.0f));
	m_pStartLabel->setParent(this);
	addChild(m_pStartLabel);

	m_pInstructionsLabel = new Label("Press 1 to Play", "Consolas", 40, sdl_color, glm::vec2(400.0f, 120.0f));
	m_pInstructionsLabel->setParent(this);
	addChild(m_pInstructionsLabel);
	
	m_pInstructionsLabel2 = new Label("Press 2 to END", "Consolas", 40, sdl_color, glm::vec2(400.0f, 240.0f));
	m_pInstructionsLabel2->setParent(this);
	addChild(m_pInstructionsLabel2);

	m_pShip = new Ship();
	m_pShip->setPosition(glm::vec2(400.0f, 300.0f));
	addChild(m_pShip);
}
