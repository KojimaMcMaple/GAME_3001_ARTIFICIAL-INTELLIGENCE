#include "EndScene.h"
#include <algorithm>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"

EndScene::EndScene()
{
	EndScene::start();
}

EndScene::~EndScene()
= default;

void EndScene::draw()
{
	m_pStartLabel->draw();
	m_pInstructionsLabel->draw();

	m_pPlanet->draw();
}

void EndScene::update()
{
}

void EndScene::clean()
{
	std::cout << "Clean called on EndScene" << std::endl;

	delete m_pStartLabel;
	m_pStartLabel = nullptr;

	delete m_pInstructionsLabel;
	m_pInstructionsLabel = nullptr;

	delete m_pPlanet;
	m_pPlanet = nullptr;

	removeAllChildren();
}

void EndScene::handleEvents()
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
				TheGame::Instance()->changeSceneState(SceneState::START_SCENE);
				break;
			}
			break;
		default:
			break;
		}
	}
}

void EndScene::start()
{
	SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), 20, 20, 20, 255);
	const SDL_Color sdl_color = { 255, 255, 255, 255 };
	m_pStartLabel = new Label("Assignment 02 finally ends...", "Consolas", 50, sdl_color, glm::vec2(400.0f, 40.0f));
	m_pStartLabel->setParent(this);
	addChild(m_pStartLabel);

	m_pInstructionsLabel = new Label("Press 2 to go back", "Consolas", 40, sdl_color, glm::vec2(400.0f, 120.0f));
	m_pInstructionsLabel->setParent(this);
	addChild(m_pInstructionsLabel);

	m_pPlanet = new Planet();
	m_pPlanet->setPosition(glm::vec2(400.0f, 300.0f));
	addChild(m_pPlanet);
}
