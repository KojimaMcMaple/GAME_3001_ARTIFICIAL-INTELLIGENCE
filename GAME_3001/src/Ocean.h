#pragma once
#ifndef __OCEAN__
#define __OCEAN__

#include "GameObject.h"
#include "TextureManager.h"

class Ocean : public GameObject {
public:
	// Constructor / Destructor
	Ocean();
	~Ocean();

	// Inherited via GameObject
	void Draw() override;
	void Update() override;
	void Clean() override;
private:
	void _move();
	void _checkBounds();
	void _reset();
};

#endif // defined (__OCEAN__)
