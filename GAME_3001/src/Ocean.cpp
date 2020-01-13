#include "Ocean.h"
#include "Game.h"

Ocean::Ocean()
{
	TheTextureManager::Instance()->load("../Assets/textures/ocean.gif",
		"ocean", TheGame::Instance()->GetRenderer());

	glm::vec2 size = TheTextureManager::Instance()->getTextureSize("ocean");
	SetWidth(size.x);
	SetHeight(size.y);
	SetVelocity(glm::vec2(0, 5));
	_reset();
	SetIsColliding(false);
	SetType(GameObjectType::OCEAN);
}

Ocean::~Ocean()
{
}

void Ocean::Draw()
{
	int xComponent = GetPosition().x;
	int yComponent = GetPosition().y;
	TheTextureManager::Instance()->draw("ocean", xComponent, yComponent,
		TheGame::Instance()->GetRenderer());
}

void Ocean::Update()
{
	_move();
	_checkBounds();
}

void Ocean::Clean()
{
}

void Ocean::_move()
{
	glm::vec2 newPosition = GetPosition() + GetVelocity();
	SetPosition(newPosition);
}

void Ocean::_checkBounds()
{
	if (GetPosition().y >= 0) 
	{
		_reset();
	}
}

void Ocean::_reset()
{
	SetPosition(glm::vec2(0, -960));
}
