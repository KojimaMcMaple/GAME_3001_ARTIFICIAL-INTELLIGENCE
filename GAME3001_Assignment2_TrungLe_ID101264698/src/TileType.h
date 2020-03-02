#pragma once
#ifndef __TILE_TYPE__
#define __TILE_TYPE__
#include <string>

// TileType = tile visual, TileState = tile logic
//duplicating DEFAULT_TILE to create a random bias/ weighted random for white tiles
enum TileType {
	DEFAULT_TILE_01,
	DEFAULT_TILE_02,
	DEFAULT_TILE_03,
	DEFAULT_TILE_04,
	DEFAULT_TILE_05,
	DEFAULT_TILE_06,
	DEFAULT_TILE_07,
	DEFAULT_TILE,
	ASTEROID_01,
	ASTEROID_02,
	ASTEROID_03,
	ASTEROID_04,
	ASTEROID_05,
	ASTEROID_06,
	ASTEROID_07,
	ASTEROID_08,
	ASTEROID_09,
	ASTEROID_10,
	ASTEROID_11,
	ASTEROID_12,
	NUM_OF_TILE_TYPES
};

static const std::string kTileTypeTextureId[NUM_OF_TILE_TYPES] = {
	"DEFAULT_TILE",
	"DEFAULT_TILE",
	"DEFAULT_TILE",
	"DEFAULT_TILE",
	"DEFAULT_TILE",
	"DEFAULT_TILE",
	"DEFAULT_TILE",
	"DEFAULT_TILE",
	"ASTEROID_01",
	"ASTEROID_02",
	"ASTEROID_03",
	"ASTEROID_04",
	"ASTEROID_05",
	"ASTEROID_06",
	"ASTEROID_07",
	"ASTEROID_08",
	"ASTEROID_09",
	"ASTEROID_10",
	"ASTEROID_11",
	"ASTEROID_12",
};

static const std::string kTileTypeTexture[NUM_OF_TILE_TYPES] = {
	"../Assets/textures/default.png",
	"../Assets/textures/default.png",
	"../Assets/textures/default.png",
	"../Assets/textures/default.png",
	"../Assets/textures/default.png",
	"../Assets/textures/default.png",
	"../Assets/textures/default.png",
	"../Assets/textures/default.png",
	"../Assets/textures/01.png",
	"../Assets/textures/02.png",
	"../Assets/textures/03.png",
	"../Assets/textures/04.png",
	"../Assets/textures/05.png",
	"../Assets/textures/06.png",
	"../Assets/textures/07.png",
	"../Assets/textures/08.png",
	"../Assets/textures/09.png",
	"../Assets/textures/10.png",
	"../Assets/textures/11.png",
	"../Assets/textures/12.png",
};
#endif