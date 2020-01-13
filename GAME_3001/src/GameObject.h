#pragma once
#ifndef __GameObject__
#define __GameObject__

#include <glm\vec2.hpp>
#include <string>

enum GameObjectType {
	ISLAND,
	PLAYER,
	CLOUD,
	BULLET,
	ENEMY,
	OCEAN,
	NUM_OF_TYPES
};

class GameObject {
public:
	GameObject();
	~GameObject();

	// Draw the object
	virtual void Draw() = 0;

	// Update the object
	virtual void Update() = 0;

	// remove anything that needs to be deleted
	virtual void Clean() = 0;

	// getters for common variables
	glm::vec2 GetPosition();
	glm::vec2 GetRotation();
	glm::vec2 GetScale();
	glm::vec2 GetVelocity();
	glm::vec2 GetAcceleration();
	int GetWidth();
	int GetHeight();
	bool GetIsColliding();
	GameObjectType GetType();

	// setters for common variables
	void SetPosition(glm::vec2 newPosition);
	void SetWidth(int newWidth);
	void SetHeight(int newHeight);
	void SetVelocity(glm::vec2 newVelocity);
	void SetIsColliding(bool collision);
	void SetType(GameObjectType newType);


private:
	// transform variables
	glm::vec2 m_position;
	glm::vec2 m_rotation;
	glm::vec2 m_scale;

	// movement variables
	glm::vec2 m_velocity;
	glm::vec2 m_acceleration;

	// size variables
	int m_width;
	int m_height;

	// animation variables
	int m_currentRow;
	int m_currentFrame;
	int m_numFrames;
	std::string m_textureID;

	// collision variables
	bool m_isColliding;
	GameObjectType m_type;
};

#endif