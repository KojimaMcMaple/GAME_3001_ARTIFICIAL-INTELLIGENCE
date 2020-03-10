#pragma once
#ifndef __SHIP__
#define __SHIP__

#include "PathFindingDisplayObject.h"
#include "TextureManager.h"

class Ship final : public PathFindingDisplayObject
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
	void TurnThenMove();
	bool HasArrived();

	// getters
	std::vector<glm::vec2> GetTargetPath();
	glm::vec2 getTargetPosition();
	glm::vec2 getCurrentDirection();
	float getMaxSpeed();
	float GetAngleToTarget();
	float GetAngleOffset();

	// setters
	void SetTargetPath(std::vector<glm::vec2> path);
	void setTargetPosition(glm::vec2 position);
	void setCurrentDirection(glm::vec2 newDirection);
	void setMaxSpeed(float newSpeed);
	void SetAngleToTarget(float angle);

private:
	// steering behaviour functions
	void m_checkSteeringState();
	void m_seek();
	void m_arrive();
	bool IsTraversing = false;
	void CheckNextNode();

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
	std::vector<glm::vec2> target_path_;
	int curr_target_path_idx_ = -1;
	glm::vec2 m_targetPosition;
	glm::vec2 m_targetDirection;
	void m_computeTargetDirection();
	void m_computeOppositeTargetDirection();
};


#endif /* defined (__SHIP__) */

