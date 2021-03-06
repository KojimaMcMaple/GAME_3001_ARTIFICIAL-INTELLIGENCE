#pragma once
#ifndef __GameObject__
#define __GameObject__

#include <glm\vec2.hpp>
#include <string>
#include <SDL.h>

#include "Globals.h"
#include "GameObjectType.h"
#include "SteeringState.h"

class GameObject {
private:
	// RENDERING
	SDL_Rect src_, dst_;
	std::string texture_id_;

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
	SteeringState m_state;

public:
	GameObject();
	~GameObject();

	// Draw the object
	virtual void draw() = 0;

	// Update the object
	virtual void update() = 0;

	// remove anything that needs to be deleted
	virtual void clean() = 0;

	// getters for common variables
	glm::vec2 getPosition();
	glm::vec2 getRotation();
	glm::vec2 getScale();
	glm::vec2 getVelocity();
	glm::vec2 getAcceleration();
	int getWidth();
	int getHeight();
	bool getIsColliding();
	GameObjectType getType();
	SteeringState getState();
	SDL_Rect* getSrc();
	SDL_Rect* getDst();
	std::string getTextureId();

	// setters for common variables
	void setPosition(glm::vec2 newPosition);
	void setWidth(int newWidth);
	void setHeight(int newHeight);
	void setVelocity(glm::vec2 newVelocity);
	void setIsColliding(bool collision);
	void setType(GameObjectType newType);
	void setSteeringState(SteeringState newState);
	void setAcceleration(glm::vec2 newAcceleration);
	void setSrc(int x, int y, int w, int h);
	void setDst(int x, int y, int w, int h);
	void setTextureId(std::string id); //cleanup is done by manager
	void RandomizeOffscreenPos();
	void RandomizeInscreenPos();
};

#endif