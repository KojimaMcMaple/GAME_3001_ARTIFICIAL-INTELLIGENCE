#include "Enemy.h"
#include "Game.h"

Enemy::Enemy() {
	TheTextureManager::Instance()->load("../Assets/textures/enemy.png", "enemy", TheGame::Instance()->GetRenderer());
	SetPosition(glm::vec2(0, 0));

	glm::vec2 size = TheTextureManager::Instance()->getTextureSize("enemy");
	SetWidth(size.x);
	SetHeight(size.y);
	SetVelocity(glm::vec2( 2, 0));
	SetPosition(glm::vec2(GetWidth() * 0.5f, GetHeight()));
	SetIsColliding(false);
	SetType(GameObjectType::ENEMY);
}

Enemy::~Enemy() {

}

void Enemy::Update() {
	_Move();
	_CheckBounds();
}

void Enemy::Draw() {
	int xComponent = GetPosition().x;
	int yComponent = GetPosition().y;
	TheTextureManager::Instance()->draw("enemy", xComponent, yComponent,
		TheGame::Instance()->GetRenderer(), true);
}

void Enemy::Clean() {

}

void Enemy::_Move() {
	glm::vec2 newPosition = GetPosition() + GetVelocity();
	SetPosition(newPosition);
}

void Enemy::_CheckBounds() {
	if (GetPosition().x >= 640 - GetWidth() * 0.5f) {
		SetVelocity(glm::vec2(-2.0f, 0.0f));
	}
	if (GetPosition().x <= GetWidth() * 0.5f) {
		SetVelocity(glm::vec2(2.0f, 0.0f));
	}
}