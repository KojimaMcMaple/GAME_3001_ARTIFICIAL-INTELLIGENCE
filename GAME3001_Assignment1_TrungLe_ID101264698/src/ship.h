#pragma once
#ifndef __Ship__
#define __Ship__

#include "GameObject.h"
#include "TextureManager.h"
#include "SoundManager.h"

class Ship :
	public GameObject
{
public:
	Ship();
	~Ship();

	// Inherited via GameObject
	void draw() override;

	void update() override;

	void clean() override;

	void turnRight();
	void turnLeft();
	void move();

	// getters
	glm::vec2 getTargetPosition();
	glm::vec2 getCurrentDirection();
	float getMaxSpeed();
	float GetAngleToTarget();
	float GetAngleOffset();

	// setters
	void setTargetPosition(glm::vec2 newPosition);
	void setCurrentDirection(glm::vec2 newDirection);
	void setMaxSpeed(float newSpeed);
	void SetAngleToTarget(float angle);

private:	
	// steering behaviour functions
	void m_checkSteeringState();
	void m_seek();
	void m_flee();
	void m_checkArrival();

	// Ship movement
	void m_checkBounds();
	void m_reset();
	float m_maxSpeed;
	float m_turnRate;
	float angle_to_target_;

	void m_changeDirection();
	float m_currentHeading;
	glm::vec2 m_currentDirection;
	
	// target info
	glm::vec2 m_targetPosition;
	glm::vec2 m_targetDirection;
	void m_computeTargetDirection();
	void m_computeOppositeTargetDirection();
};


#endif /* defined (__Ship__) */

