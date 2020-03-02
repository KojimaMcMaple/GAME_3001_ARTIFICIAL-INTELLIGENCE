#include "PathFindingDisplayObject.h"

PathFindingDisplayObject::PathFindingDisplayObject(): m_currentTile(nullptr)
{
}

PathFindingDisplayObject::~PathFindingDisplayObject()
= default;

Tile* PathFindingDisplayObject::getTile() const
{
	return m_currentTile;
}

void PathFindingDisplayObject::setTile(Tile* new_tile)
{
	m_currentTile = new_tile;
}
