#include "ship.h"
#include "Game.h"
#include "Util.h"
#include "PlayScene.h"
#include "glm/gtx/string_cast.hpp"


Ship::Ship() :
	m_maxSpeed(4.0f), m_currentDirection(glm::vec2(1.0f, 0.0f)), m_turnRate(1.0f), angle_to_target_(0.0f)
{
	TheTextureManager::Instance()->load("../Assets/textures/ship3.png",
		"ship", TheGame::Instance()->getRenderer());

	auto size = TheTextureManager::Instance()->getTextureSize("ship");
	setWidth(size.x);
	setHeight(size.y);
	setPosition(glm::vec2(400.0f, 300.0f));
	setVelocity(glm::vec2(0.0f, 0.0f));
	setAcceleration(glm::vec2(0.0f, 0.0f));
	setIsColliding(false);
	setType(SHIP);
	setState(IDLE);
}

Ship::~Ship()
= default;

void Ship::draw()
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;

	TheTextureManager::Instance()->draw("ship", xComponent, yComponent,
		TheGame::Instance()->getRenderer(), m_currentHeading, 255, true);
}

void Ship::m_checkSteeringState()
{
	switch (getState())
	{
	case IDLE:
		//move();
		break;
	case SEEK:
		m_seek();
		move();
		m_checkBounds();
		break;
	case ARRIVE:
		m_seek();
		move();
		m_checkBounds();
		m_arrive();
		break;
	case TRAVERSE:
		CheckNextNode();
		m_seek();
		TurnThenMove();
		m_checkBounds();
		break;
	case AVOID:
		break;
	case FLEE:
		break;
	default:
		std::cout << "unknown or unused case" << std::endl;
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

//void Ship::move()
//{
//	if (Util::distance(getPosition(), m_targetPosition) > 1.0f) {
//		const glm::vec2 desired = Util::normalize(m_targetPosition - getPosition()) * m_maxSpeed;
//		setVelocity(desired);
//
//
//		const glm::vec2 newPosition = getPosition() + getVelocity();
//		setPosition(newPosition);
//	}
//}

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

void Ship::TurnThenMove()
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

	if (abs(GetAngleToTarget()) < 5) {
		setVelocity(getCurrentDirection());
	}
	else {
		setVelocity(glm::vec2(0, 0));
	}

	glm::vec2 newPosition = getPosition() + getVelocity() * m_maxSpeed;
	setPosition(newPosition);
}

std::vector<glm::vec2> Ship::GetTargetPath()
{
	return target_path_;
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

void Ship::SetTargetPath(std::vector<glm::vec2> path)
{
	target_path_.clear();
	target_path_.resize(0);
	target_path_.shrink_to_fit();

	for (auto node : path) {
		target_path_.push_back(node);
	}
}

void Ship::setTargetPosition(const glm::vec2 position)
{
	m_targetPosition = position;
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

	if (getPosition().x > Config::SCREEN_WIDTH)
	{
		setPosition(glm::vec2(0.0f, getPosition().y));
	}

	if (getPosition().x < 0)
	{
		setPosition(glm::vec2(800.0f, getPosition().y));
	}

	if (getPosition().y > Config::SCREEN_HEIGHT)
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
	setIsColliding(false);
	const int halfWidth = getWidth() * 0.5f;
	const auto xComponent = rand() % (640 - getWidth()) + halfWidth + 1;
	const auto yComponent = -getHeight();
	setPosition(glm::vec2(xComponent, yComponent));
}

//void Ship::m_seek()
//{
//	const auto desired = Util::normalize(m_targetPosition - getPosition()) * m_maxSpeed;
//	auto steer = (desired - getVelocity());
//
//	steer = Util::limitMagnitude(steer, angle_to_target_);
//	setAcceleration(steer);
//}

void Ship::m_changeDirection()
{
	float x = cos(m_currentHeading * Util::Deg2Rad);
	float y = sin(m_currentHeading * Util::Deg2Rad);
	m_currentDirection = glm::vec2(x, y);
}

void Ship::m_computeTargetDirection()
{
	glm::vec2 steeringVelocity = getTargetPosition() - getPosition();
	//std::cout << steeringVelocity.x << ", " << steeringVelocity.y << std::endl;
	m_targetDirection = Util::normalize(steeringVelocity);
	//std::cout << m_targetDirection.x << " ," << m_targetDirection.y << std::endl;
	//std::cout << std::endl;
}

void Ship::m_computeOppositeTargetDirection()
{
	glm::vec2 steeringVelocity = getTargetPosition() - getPosition();
	std::cout << steeringVelocity.x << ", " << steeringVelocity.y << std::endl;
	m_targetDirection = -Util::normalize(steeringVelocity);
	//std::cout << m_targetDirection.x << " ," << m_targetDirection.y << std::endl;
	//std::cout << std::endl;
}

void Ship::m_seek()
{
	m_computeTargetDirection();
	/**/ // TODO: remove after move is updated 
}

bool Ship::HasArrived()
{
	if (Util::distance(getPosition(), getTargetPosition()) <= 3.0f)
	{
		return true;
	}
	return false;
}

void Ship::m_arrive()
{
	if (HasArrived())
	{
		setState(IDLE);
		TheSoundManager::Instance()->playSound("item", 0);
	}
}

void Ship::CheckNextNode()
{
	if (curr_target_path_idx_ == -1) {
		curr_target_path_idx_ = 0;
		setTargetPosition(target_path_[curr_target_path_idx_]);
	}

	if (HasArrived()) {
		curr_target_path_idx_++;
		if (curr_target_path_idx_ < target_path_.size()) {
			setTargetPosition(target_path_[curr_target_path_idx_]);
		}
		else {
			curr_target_path_idx_ = -1;
			m_arrive();
		}
	}
}
