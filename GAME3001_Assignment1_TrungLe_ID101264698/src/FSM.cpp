#include "FSM.h"
#include "Game.h"

GameState::GameState()
{
}

void GameState::Render()
{
	SDL_RenderPresent(TheGame::Instance()->getRenderer());
}

void GameState::Resume()
{
}

// PAUSE 
PauseState::PauseState()
{
}

void PauseState::Enter()
{
	std::cout << "Entering Pause..." << std::endl;
}

void PauseState::Update()
{
	if (TheGame::Instance()->isKeyDown(SDL_SCANCODE_R)) {
		TheGame::Instance()->GetFSM().PopState();
	}
}

void PauseState::Render()
{
	std::cout << "Rendering Pause..." << std::endl;
	TheGame::Instance()->GetFSM().GetStates().front()->Render();
	SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), 255, 255, 255, 50);
	SDL_Rect temp_overlay = { 0,0,Globals::sWindowWidth,Globals::sWindowHeight };
	SDL_RenderFillRect(TheGame::Instance()->getRenderer(), &temp_overlay);
	TheGame::Instance()->GetPauseScreen().draw();
	GameState::Render();
}

void PauseState::Exit()
{
	std::cout << "Exiting Pause..." << std::endl;
}

// SEEK
SeekState::SeekState()
{
}

void SeekState::Enter()
{
	std::cout << "Entering Seek..." << std::endl;
	TheSoundManager::Instance()->playMusic("encounter", -1);
	TheGame::Instance()->GetTarget().RandomizeOffscreenPos();
	TheGame::Instance()->GetShip().RandomizeInscreenPos();
	TheGame::Instance()->GetShip().setTargetPosition(TheGame::Instance()->GetTarget().getPosition());
	TheGame::Instance()->GetShip().setSteeringState(SEEK);
}

void SeekState::Update()
{
	TheGame::Instance()->UpdateGameObjects();

	if (TheGame::Instance()->isKeyDown(SDL_SCANCODE_X)) {
		Mix_HaltMusic();
		TheSoundManager::Instance()->playSound("menu", 0);
		TheGame::Instance()->GetFSM().ChangeState(new TitleState());
	}
}

void SeekState::Render()
{
	//std::cout << "Rendering Game..." << std::endl;

	SDL_RenderClear(TheGame::Instance()->getRenderer()); // clear the renderer to the draw colour
	TheGame::Instance()->RenderGameObjects();

	if (dynamic_cast<SeekState*>(TheGame::Instance()->GetFSM().GetStates().back())) {
		GameState::Render();
	}
}

void SeekState::Exit()
{
	std::cout << "Exiting Seek..." << std::endl;
}

void SeekState::Resume()
{
}

// ARRIVE
ArriveState::ArriveState()
{
}

void ArriveState::Enter()
{
	std::cout << "Entering Seek..." << std::endl;
	TheSoundManager::Instance()->playMusic("encounter", -1);
	TheGame::Instance()->GetTarget().RandomizeOffscreenPos();
	TheGame::Instance()->GetShip().RandomizeInscreenPos();
	TheGame::Instance()->GetShip().setTargetPosition(TheGame::Instance()->GetTarget().getPosition());
	TheGame::Instance()->GetShip().setSteeringState(ARRIVE);
}

void ArriveState::Update()
{
	TheGame::Instance()->UpdateGameObjects();

	if (TheGame::Instance()->isKeyDown(SDL_SCANCODE_X)) {
		Mix_HaltMusic();
		TheSoundManager::Instance()->playSound("menu", 0);
		TheGame::Instance()->GetFSM().ChangeState(new TitleState());
	}
}

void ArriveState::Render()
{
	//std::cout << "Rendering Game..." << std::endl;

	SDL_RenderClear(TheGame::Instance()->getRenderer()); // clear the renderer to the draw colour
	TheGame::Instance()->RenderGameObjects();

	if (dynamic_cast<ArriveState*>(TheGame::Instance()->GetFSM().GetStates().back())) {
		GameState::Render();
	}
}

void ArriveState::Exit()
{
	std::cout << "Exiting Arrive..." << std::endl;
}

void ArriveState::Resume()
{
}

// FLEE
FleeState::FleeState()
{
}

void FleeState::Enter()
{
	std::cout << "Entering Seek..." << std::endl;
	TheSoundManager::Instance()->playMusic("encounter", -1);
	TheGame::Instance()->GetTarget().setPosition(glm::vec2(Globals::sWindowWidth / 2 + TheGame::Instance()->GetTarget().getWidth(), Globals::sWindowHeight / 2));
	TheGame::Instance()->GetShip().setPosition(glm::vec2(Globals::sWindowWidth / 2 - TheGame::Instance()->GetShip().getWidth(), Globals::sWindowHeight / 2));
	TheGame::Instance()->GetShip().setTargetPosition(TheGame::Instance()->GetTarget().getPosition());
	TheGame::Instance()->GetShip().setSteeringState(FLEE);
}

void FleeState::Update()
{
	TheGame::Instance()->UpdateGameObjects();

	if (TheGame::Instance()->isKeyDown(SDL_SCANCODE_X)) {
		Mix_HaltMusic();
		TheSoundManager::Instance()->playSound("menu", 0);
		TheGame::Instance()->GetFSM().ChangeState(new TitleState());
	}
}

void FleeState::Render()
{
	//std::cout << "Rendering Game..." << std::endl;

	SDL_RenderClear(TheGame::Instance()->getRenderer()); // clear the renderer to the draw colour
	TheGame::Instance()->RenderGameObjects();

	if (dynamic_cast<FleeState*>(TheGame::Instance()->GetFSM().GetStates().back())) {
		GameState::Render();
	}
}

void FleeState::Exit()
{
	std::cout << "Exiting Flee..." << std::endl;
}

void FleeState::Resume()
{
}

// TITLE
TitleState::TitleState()
{
}

void TitleState::Enter()
{
	std::cout << "Entering Title..." << std::endl;
}

void TitleState::Update()
{
	if (TheGame::Instance()->isKeyDown(SDL_SCANCODE_1)) {
		TheSoundManager::Instance()->playSound("equip", 0);
		TheGame::Instance()->GetFSM().ChangeState(new SeekState());
	}
	else if (TheGame::Instance()->isKeyDown(SDL_SCANCODE_2)) {
		TheSoundManager::Instance()->playSound("equip", 0);
		TheGame::Instance()->GetFSM().ChangeState(new ArriveState());
	}
	else if (TheGame::Instance()->isKeyDown(SDL_SCANCODE_3)) {
		TheSoundManager::Instance()->playSound("equip", 0);
		TheGame::Instance()->GetFSM().ChangeState(new SeekState());
	}
	else if (TheGame::Instance()->isKeyDown(SDL_SCANCODE_4)) {
		TheSoundManager::Instance()->playSound("equip", 0);
		TheGame::Instance()->GetFSM().ChangeState(new FleeState());
	}
}

void TitleState::Render()
{
	//std::cout << "Rendering Title..." << std::endl;
	SDL_RenderClear(TheGame::Instance()->getRenderer()); // clear the renderer to the draw colour
	TheGame::Instance()->GetTitleScreen().draw();
	GameState::Render();
}

void TitleState::Exit()
{
	std::cout << "Exiting Title..." << std::endl;
}

FSM::FSM()
{
}

FSM::~FSM()
{
}

void FSM::Update()
{
	if (!states_.empty()) {
		states_.back()->Update();
	}
}

void FSM::Render()
{
	if (!states_.empty()) {
		states_.back()->Render();
	}
}

void FSM::ChangeState(GameState* state_ptr)
{
	if (!states_.empty()) {
		states_.back()->Exit(); //invoke Exit() of curr state
		delete states_.back(); //deallocate curr state
		states_.back() = nullptr;
		states_.pop_back();
	}
	PushState(state_ptr);

	//state_ptr->Enter();
	//states_.push_back(state_ptr);

	//states_.push_back(state_ptr);
	//states_.back()->Enter();
}

void FSM::PushState(GameState* state_ptr)
{
	state_ptr->Enter();
	states_.push_back(state_ptr);
}

void FSM::PopState()
{
	if (!states_.empty()) {
		states_.back()->Exit(); //invoke Exit() of curr state
		delete states_.back(); //deallocate curr state
		states_.back() = nullptr;
		states_.pop_back();
	}
	states_.back()->Resume();
}

void FSM::Clean()
{
	while (!states_.empty()) {
		states_.back()->Exit(); //invoke Exit() of curr state
		delete states_.back(); //deallocate curr state
		states_.back() = nullptr;
		states_.pop_back();
	}
}

std::vector<GameState*>& FSM::GetStates()
{
	// TODO: insert return statement here
	return states_;
}
