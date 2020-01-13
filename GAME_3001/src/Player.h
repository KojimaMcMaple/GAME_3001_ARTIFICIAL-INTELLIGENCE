#pragma once
#ifndef __Player__
#define __Player__

#include "GameObject.h"
#include "TextureManager.h"
#include "SoundManager.h"

class Player : public GameObject {
public:
	Player();
	~Player();

	// Draw the object
	void Draw();

	// Update the object
	void Update();

	// remove anything that needs to be deleted
	void Clean();
private:

};


#endif /* defined (__Player__) */