#include "Player.h"
#include "Game.h"

Player::Player()
{
	TheTextureManager::Instance()->load("../Assets/textures/plane.png", "player", TheGame::Instance()->GetRenderer());
	SetPosition(glm::vec2(0, 430.0f));

	glm::vec2 size = TheTextureManager::Instance()->getTextureSize("player");
	SetWidth(size.x);
	SetHeight(size.y);
	SetIsColliding(false);
	SetType(GameObjectType::PLAYER);

	TheSoundManager::Instance()->load("../Assets/audio/engine.ogg",
		"engine", sound_type::SOUND_MUSIC);

	TheSoundManager::Instance()->playMusic("engine", -1);
}

Player::~Player()
{
}

void Player::Draw()
{
	TheTextureManager::Instance()->draw("player", GetPosition().x, GetPosition().y, TheGame::Instance()->GetRenderer(), true);
}

void Player::Update()
{
	glm::vec2 mouseVector = TheGame::Instance()->GetMousePosition();

	SetPosition(glm::vec2(mouseVector.x, GetPosition().y));
}

void Player::Clean()
{
}
