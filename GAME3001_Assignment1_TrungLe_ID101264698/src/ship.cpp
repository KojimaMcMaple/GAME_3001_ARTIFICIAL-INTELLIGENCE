#include "ship.h"
#include "Game.h"
#include "Util.h"
#include "GLM/gtx/rotate_vector.hpp"


Ship::Ship()
{
	TheTextureManager::Instance()->load("../Assets/textures/ship3.png", "ship", TheGame::Instance()->getRenderer());

	glm::vec2 size = TheTextureManager::Instance()->getTextureSize("ship");
	setWidth(size.x);
	setHeight(size.y);
	setPosition(glm::vec2(0.0f, 0.0f));
	setVelocity(glm::vec2(0.0f, 0.0f));
	setAcceleration(glm::vec2(0.0f, 0.0f));
	setIsColliding(false);
	setType(GameObjectType::SHIP);
	setSteeringState(SteeringState::IDLE);
	m_maxSpeed = 5.0f;
	m_currentHeading = 0.0; // current facing angle
	m_turnRate = 5.0f; // 5 degrees per frame
	m_currentDirection = glm::vec2(1.0f, 0.0f); // facing right
	SetAngleToTarget(0.0f);
}


Ship::~Ship()
{
}

void Ship::draw()
{
	int xComponent = getPosition().x;
	int yComponent = getPosition().y;

	TheTextureManager::Instance()->draw("ship", xComponent, yComponent,
		TheGame::Instance()->getRenderer(), m_currentHeading, 255, true);
}

void Ship::m_checkSteeringState()
{
	switch (getState())
	{
	case SteeringState::IDLE:
		//move();
		break;
	case SteeringState::SEEK:
		m_seek();
		move();
		m_checkBounds();
		//m_checkArrival();
		break;
	case SteeringState::ARRIVE:
		m_seek();
		move();
		m_checkBounds();
		m_checkArrival();
		break;
	case SteeringState::AVOID:
		break;
	case SteeringState::FLEE:
		m_flee();
		move();
		m_checkBounds();
		break;
	}
}

void Ship::update()
{
	m_checkSteeringState();
}

void Ship::clean()
{
}

void Ship::turnRight()
{
	m_currentHeading += (m_turnRate < GetAngleOffset()) ? m_turnRate : GetAngleOffset();

	if (m_currentHeading > 360)
	{
		m_currentHeading -= 360.0f;
	}

	//std::cout << "currentHeading: " << m_currentHeading << std::endl;
	m_changeDirection();
	m_computeTargetDirection();
}

void Ship::turnLeft()
{
	m_currentHeading -= (m_turnRate < GetAngleOffset()) ? m_turnRate : GetAngleOffset();

	if (m_currentHeading < 0)
	{
		m_currentHeading += 360.0f;
	}

	//std::cout << "currentHeading: " << m_currentHeading << std::endl;
	m_changeDirection();
	m_computeTargetDirection();
}

void Ship::move()
{
	SetAngleToTarget(Util::signedAngle(getCurrentDirection(), m_targetDirection));
	//std::cout << "target Direction: " << angleToTarget << std::endl;

	//TODO: if angleToTarget > 0.0f then turn right
	if (GetAngleToTarget() > 0) {
		turnRight();
	}

	//TODO: if angleToTarget < 0.0f then turn left
	if (GetAngleToTarget() < 0) {
		turnLeft();
	}

	//TODO: set velocity to the current Direction
	setVelocity(getCurrentDirection());

	glm::vec2 newPosition = getPosition() + getVelocity() * m_maxSpeed;
	setPosition(newPosition);
}

glm::vec2 Ship::getTargetPosition()
{
	return m_targetPosition;
}

glm::vec2 Ship::getCurrentDirection()
{
	return m_currentDirection;
}

float Ship::getMaxSpeed()
{
	return m_maxSpeed;
}

float Ship::GetAngleToTarget()
{
	return angle_to_target_;
}

float Ship::GetAngleOffset()
{
	return abs(abs(GetAngleToTarget() - m_turnRate));;
}

void Ship::setTargetPosition(glm::vec2 newPosition)
{
	m_targetPosition = newPosition;
}

void Ship::setCurrentDirection(glm::vec2 newDirection)
{
	m_currentDirection = newDirection;
}

void Ship::setMaxSpeed(float newSpeed)
{
	m_maxSpeed = newSpeed;
}

void Ship::SetAngleToTarget(float angle)
{
	angle_to_target_ = angle;
}

void Ship::m_checkBounds()
{

	if (getPosition().x > 800)
	{
		setPosition(glm::vec2(0.0f, getPosition().y));
	}

	if (getPosition().x < 0)
	{
		setPosition(glm::vec2(800.0f, getPosition().y));
	}

	if (getPosition().y > 600)
	{
		setPosition(glm::vec2(getPosition().x, 0.0f));
	}

	if (getPosition().y < 0)
	{
		setPosition(glm::vec2(getPosition().x, 600.0f));
	}

}

void Ship::m_reset()
{

}

void Ship::m_changeDirection()
{
	float x = cos(m_currentHeading * Util::Deg2Rad);
	float y = sin(m_currentHeading * Util::Deg2Rad);
	m_currentDirection = glm::vec2(x, y);
}

void Ship::m_computeTargetDirection()
{
	glm::vec2 steeringVelocity = getTargetPosition() - getPosition();
	std::cout << steeringVelocity.x << ", " << steeringVelocity.y << std::endl;
	m_targetDirection = Util::normalize(steeringVelocity);
	std::cout << m_targetDirection.x << " ," << m_targetDirection.y << std::endl;
	std::cout << std::endl;
}

void Ship::m_computeOppositeTargetDirection()
{
	glm::vec2 steeringVelocity = getTargetPosition() - getPosition();
	std::cout << steeringVelocity.x << ", " << steeringVelocity.y << std::endl;
	m_targetDirection = -Util::normalize(steeringVelocity);
	std::cout << m_targetDirection.x << " ," << m_targetDirection.y << std::endl;
	std::cout << std::endl;
}

void Ship::m_seek()
{
	m_computeTargetDirection();
	/**/ // TODO: remove after move is updated 
}

void Ship::m_flee()
{
	m_computeOppositeTargetDirection();
	/**/ // TODO: remove after move is updated 
}

void Ship::m_checkArrival()
{
	if (Util::distance(getPosition(),getTargetPosition()) <= 3.0f)
	{
		setSteeringState(IDLE);
		TheSoundManager::Instance()->playSound("item", 0);
	}
}
