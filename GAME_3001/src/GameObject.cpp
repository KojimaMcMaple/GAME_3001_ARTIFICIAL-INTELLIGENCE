#include "GameObject.h"

GameObject::GameObject()
{
	m_position = glm::vec2(0, 0);
	m_rotation = glm::vec2(0, 0);
	m_scale = glm::vec2(1.0f, 1.0f);
	m_velocity = glm::vec2(0, 0);
	m_acceleration = glm::vec2(0, 0);
}

GameObject::~GameObject()
{
}


glm::vec2 GameObject::GetPosition()
{
	return m_position;
}

glm::vec2 GameObject::GetRotation()
{
	return m_rotation;
}

glm::vec2 GameObject::GetScale()
{
	return m_scale;
}

glm::vec2 GameObject::GetVelocity()
{
	return m_velocity;
}

glm::vec2 GameObject::GetAcceleration()
{
	return m_acceleration;
}

int GameObject::GetWidth()
{
	return m_width;
}

int GameObject::GetHeight()
{
	return m_height;
}

bool GameObject::GetIsColliding()
{
	return m_isColliding;
}

GameObjectType GameObject::GetType()
{
	return m_type;
}

void GameObject::SetPosition(glm::vec2 newPosition)
{
	m_position = newPosition;
}

void GameObject::SetWidth(int newWidth)
{
	m_width = newWidth;
}

void GameObject::SetHeight(int newHeight)
{
	m_height = newHeight;
}

void GameObject::SetVelocity(glm::vec2 newVelocity)
{
	m_velocity = newVelocity;
}

void GameObject::SetIsColliding(bool collision)
{
	m_isColliding = collision;
}

void GameObject::SetType(GameObjectType newType)
{
	m_type = newType;
}
