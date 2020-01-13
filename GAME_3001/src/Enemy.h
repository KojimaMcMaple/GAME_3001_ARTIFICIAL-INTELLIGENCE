#pragma once
#ifndef __ENEMY__
#define __ENEMY__

#include "GameObject.h"
#include "TextureManager.h"

class Enemy :public GameObject {
public:
	Enemy();
	~Enemy();

	void Update();
	void Draw();
	void Clean();

private:
	static const int kSpeedX = 5;
	static const int kSpeedY = 5;

	void _Move();
	void _CheckBounds();
};

#endif /* defined (__Enemy__) */