#pragma once
#ifndef __TILE_STATE__
#define __TILE_STATE__

// TileType = tile visual, TileState = tile logic

enum TileState {
	IMPASSABLE = -1,
	OPEN,
	CLOSED,
	START,
	GOAL,
	UNDEFINED,
	NUM_OF_TILE_STATES
};
#endif /* defined (__TILE_STATE__) */