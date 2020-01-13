#include "Island.h"
#include "Game.h"

Island::Island()
{
	TheTextureManager::Instance()->load("../Assets/textures/island.png",
		"island", TheGame::Instance()->GetRenderer());

	glm::vec2 size = TheTextureManager::Instance()->getTextureSize("island");
	SetWidth(size.x);
	SetHeight(size.y);
	SetVelocity(glm::vec2(0, 5));
	_reset();
	SetIsColliding(false);
	SetType(GameObjectType::ISLAND);

	TheSoundManager::Instance()->load("../Assets/audio/yay.ogg", 
		"yay", sound_type::SOUND_SFX);
}

Island::~Island()
{
}

void Island::Draw()
{
	int xComponent = GetPosition().x;
	int yComponent = GetPosition().y;
	TheTextureManager::Instance()->draw("island", xComponent, yComponent,
		TheGame::Instance()->GetRenderer(), true);
}

void Island::Update()
{
	_move();
	_checkBounds();
}

void Island::Clean()
{
}

void Island::_move()
{
	glm::vec2 newPosition = GetPosition() + GetVelocity();
	SetPosition(newPosition);
}

void Island::_checkBounds()
{
	if (GetPosition().y > 480 + GetHeight()) {
		_reset();
	}
}

void Island::_reset()
{
	SetIsColliding(false);
	int halfWidth = GetWidth() * 0.5;
	int xComponent = rand() % (640 - GetWidth()) + halfWidth + 1;
	int yComponent = -GetHeight();
	SetPosition(glm::vec2(xComponent, yComponent));
}
