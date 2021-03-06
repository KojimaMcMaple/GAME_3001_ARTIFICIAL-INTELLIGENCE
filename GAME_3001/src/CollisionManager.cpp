#include "CollisionManager.h"



int CollisionManager::squaredDistance(glm::vec2 P1, glm::vec2 P2)
{
	int diffOfXs = P2.x - P1.x;
	int diffOfYs = P2.y - P1.y;
	int result = diffOfXs * diffOfXs + diffOfYs * diffOfYs;

	return result;
}

bool CollisionManager::squaredRadiusCheck(GameObject * object1, GameObject * object2)
{
	glm::vec2 P1 = object1->GetPosition();
	glm::vec2 P2 = object2->GetPosition();
	int halfHeights = (object1->GetHeight() + object2->GetHeight()) * 0.5;

	//if (glm::distance(P1, P2) < halfHeights) {

	if (CollisionManager::squaredDistance(P1, P2) < (halfHeights * halfHeights)) {
		if (!object2->GetIsColliding()) {
			
			object2->SetIsColliding(true);

			switch (object2->GetType()) {
			case ISLAND:
				std::cout << "Collision with Island!" << std::endl;
				TheSoundManager::Instance()->playSound("yay", 0);
				break;
			case CLOUD:
				std::cout << "Collision with Cloud!" << std::endl;
				TheSoundManager::Instance()->playSound("thunder", 0);
				break;
			case BULLET:
				break;
			case ENEMY:
				break;
			default:
				std::cout << "Collision with unknown type!" << std::endl;
					break;
			}

			return true;
		}
		return false;
	}

	
}

CollisionManager::CollisionManager()
{
}


CollisionManager::~CollisionManager()
{
}
