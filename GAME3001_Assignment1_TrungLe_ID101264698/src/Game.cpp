#include "Game.h"
#include "Util.h"

Game* Game::s_pInstance = 0;

glm::vec2 Game::getTargetPosition()
{
	return m_pTarget->getPosition();
}

SDL_Renderer * Game::getRenderer()
{
	return m_pRenderer;
}

glm::vec2 Game::getMousePosition()
{
	return m_mousePosition;
}

FSM& Game::GetFSM()
{
	// TODO: insert return statement here
	return *fsm_;
}

UI& Game::GetTitleScreen()
{
	// TODO: insert return statement here
	return *title_screen_ptr_;
}

UI& Game::GetPauseScreen()
{
	// TODO: insert return statement here
	return *pause_screen_ptr_;
}

Ship& Game::GetShip()
{
	// TODO: insert return statement here
	return *m_pShip;
}

Target& Game::GetTarget()
{
	// TODO: insert return statement here
	return *m_pTarget;
}

Game::Game()
{
}

Game::~Game()
{
}

void Game::createGameObjects()
{
	title_screen_ptr_ = new UI("title", "../Assets/textures/Title_Screen.png", 0, 0, Globals::sWindowWidth, Globals::sWindowHeight, 0, 0, Globals::sWindowWidth, Globals::sWindowHeight);
	
	m_pShip = new Ship();
	m_pTarget = new Target();
}

void Game::UpdateGameObjects()
{
	m_pShip->setTargetPosition(m_pTarget->getPosition());
	m_pShip->update();
	m_pTarget->update();
}

void Game::RenderGameObjects()
{
	m_pTarget->draw();
	m_pShip->draw();
}

bool Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	srand(time(NULL));
	
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
		m_pWindow = SDL_CreateWindow(title, xpos, ypos, width, height, flags);

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
			createGameObjects();
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
	TheSoundManager::Instance()->load("../Assets/audio/MGS_Encounter.mp3", "encounter", SOUND_MUSIC);
	TheSoundManager::Instance()->load("../Assets/audio/Konami Intro.wav", "intro", SOUND_SFX);
	TheSoundManager::Instance()->load("../Assets/audio/equip.wav", "equip", SOUND_SFX);
	TheSoundManager::Instance()->load("../Assets/audio/Menu.wav", "menu", SOUND_SFX);
	TheSoundManager::Instance()->load("../Assets/audio/New item.wav", "item", SOUND_SFX);
	key_states_ = SDL_GetKeyboardState(nullptr);
	fsm_ = new FSM();
	fsm_->ChangeState(new TitleState());
	std::cout << "init success" << std::endl;
	TheSoundManager::Instance()->playSound("intro", 0);

	m_bRunning = true; // everything initialized successfully - start the main loop

	return true;
}

bool Game::isKeyDown(SDL_Scancode keyboard_code)
{
	if (key_states_ != nullptr) {
		if (key_states_[keyboard_code]) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

void Game::render()
{
	GetFSM().Render();
}

void Game::update()
{
	GetFSM().Update();
}

void Game::clean()
{
	std::cout << "cleaning game" << std::endl;

	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	SDL_Quit();
}


void Game::handleEvents()
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
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
					m_bRunning = false;
					break;
				case SDLK_w:
					//m_pShip->setTargetPosition(m_pTarget->getPosition());
					m_pTarget->setVelocity(glm::vec2(m_pTarget->getVelocity().x, -1.0f));
					break;
				case SDLK_s:
					//m_pShip->setTargetPosition(m_pTarget->getPosition());
					m_pTarget->setVelocity(glm::vec2(m_pTarget->getVelocity().x, 1.0f));
					break;
				case SDLK_a:
					//m_pShip->setTargetPosition(m_pTarget->getPosition());
					m_pTarget->setVelocity(glm::vec2(-1.0f, m_pTarget->getVelocity().y));
					break;
				case SDLK_d:
					//m_pShip->setTargetPosition(m_pTarget->getPosition());
					m_pTarget->setVelocity(glm::vec2(1.0f, m_pTarget->getVelocity().y));
					break;
				case SDLK_0:
					m_pShip->setSteeringState(SteeringState::IDLE);
					break;
				//case SDLK_1:
				//	m_pShip->setTargetPosition(m_pTarget->getPosition());
				//	m_pShip->setSteeringState(SteeringState::SEEK);
				//	break;
				//case SDLK_2:
				//	m_pShip->setSteeringState(SteeringState::ARRIVE);
				//	break;
				//case SDLK_3:
				//	m_pShip->setSteeringState(SteeringState::AVOID);
				//	break;
				//case SDLK_4:
				//	m_pShip->setSteeringState(SteeringState::FLEE);
				//	break;
				case SDLK_RIGHT:
					m_pShip->turnRight();
					break;
				case SDLK_LEFT:
					m_pShip->turnLeft();
					break;
				case SDLK_UP:
					m_pShip->setVelocity(m_pShip->getCurrentDirection());
					m_pShip->move();
					break;

			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym) {
				case SDLK_w:
					if (m_pTarget->getVelocity().y < 0.0f) {
						m_pTarget->setVelocity(glm::vec2(m_pTarget->getVelocity().x, 0.0f));
					}
					break;
				
				case SDLK_s:
					if (m_pTarget->getVelocity().y > 0.0f) {
						m_pTarget->setVelocity(glm::vec2(m_pTarget->getVelocity().x, 0.0f));
					}
					break;
				
				case SDLK_a:
					if (m_pTarget->getVelocity().x < 0.0f) {
						m_pTarget->setVelocity(glm::vec2(0.0f, m_pTarget->getVelocity().y));
					}
					break;
				case SDLK_d:
					if (m_pTarget->getVelocity().x > 0.0f) {
						m_pTarget->setVelocity(glm::vec2(0.0f, m_pTarget->getVelocity().y));
					}
					break;
				case SDLK_RIGHT:
					
					break;
				case SDLK_LEFT:
					
					break;
				case SDLK_UP:
					
					break;
				
			}
			
		default:
			break;
		}
	}
}