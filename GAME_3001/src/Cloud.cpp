#include "Cloud.h"
#include "Game.h"

Cloud::Cloud()
{
	TheTextureManager::Instance()->load("../Assets/textures/cloud.png",
		"cloud", TheGame::Instance()->GetRenderer());

	glm::vec2 size = TheTextureManager::Instance()->getTextureSize("cloud");
	SetWidth(size.x);
	SetHeight(size.y);
	_reset();
	SetIsColliding(false);
	SetType(GameObjectType::CLOUD);

	TheTextureManager::Instance()->setAlpha("cloud", 128);

	TheSoundManager::Instance()->load("../Assets/audio/thunder.ogg",
		"thunder", sound_type::SOUND_SFX);
}

Cloud::~Cloud()
{
}

void Cloud::Draw()
{
	int xComponent = GetPosition().x;
	int yComponent = GetPosition().y;
	TheTextureManager::Instance()->draw("cloud", xComponent, yComponent,
		TheGame::Instance()->GetRenderer(), true);
}

void Cloud::Update()
{
	_move();
	_checkBounds();
}

void Cloud::Clean()
{
}

void Cloud::_move()
{
	glm::vec2 newPosition = GetPosition() + GetVelocity();
	SetPosition(newPosition);
}

void Cloud::_checkBounds()
{
	if (GetPosition().y > 480 + GetHeight()) {
		_reset();
	}
}

void Cloud::_reset()
{
	SetIsColliding(false);
	int randomVelocityX = (rand() % 5) - 2;
	int randomVelocityY = (rand() % 6) + 5;
	SetVelocity(glm::vec2(randomVelocityX, randomVelocityY));
	int halfWidth = GetWidth() * 0.5;
	int xComponent = rand() % (640 - GetWidth()) + halfWidth + 1;
	int yComponent = -GetHeight();
	SetPosition(glm::vec2(xComponent, yComponent));
}
