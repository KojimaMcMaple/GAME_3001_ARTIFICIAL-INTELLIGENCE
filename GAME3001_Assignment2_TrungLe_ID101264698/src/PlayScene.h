#pragma once
#ifndef __PLAY_SCENE__
#define __PLAY_SCENE__

#include "Scene.h"
#include "imgui.h"

// Game Objects
#include "Mine.h"
#include "Planet.h"
#include "ship.h"
#include "Tile.h"
#include "Heuristic.h"


class PlayScene final : public Scene
{
public:
	PlayScene();
	~PlayScene();

	// Inherited via Scene
	virtual void draw() override;
	virtual void update() override;
	virtual void clean() override;
	virtual void handleEvents() override;
	virtual void start() override;

private:
	// GameObjects
	Ship* m_pShip;
	Planet* m_pPlanet;

	// Tile & Grid members
	std::vector<Tile*> m_pGrid;

	// mines
	std::vector<Mine*> m_mines;

	void m_resetGrid();
	void m_buildGrid();
	void m_mapTiles();
	void LoadTileTextures();

	int m_spawnObject(PathFindingDisplayObject* object);
	void m_spawnShip();
	void m_spawnPlanet();

	void PopulateGrid();

	void m_computeTileValues();

	// pathfinding functions & variables
	Tile* m_findLowestCostTile(const std::vector<Tile*>& neighbours);
	void m_findShortestPath();
	Tile* GetNeighborWithWantedState(const Tile* in_tile, const TileState in_state);
	bool HasNeighborWithWantedState(const Tile* in_tile, const TileState in_state);
	bool HasViablePath();
	std::vector<Tile*> shortest_path_;
	std::vector<Tile*> m_openList;
	std::vector<Tile*> m_closedList;
	Heuristic m_heuristic;
	void m_selectHeuristic(Heuristic heuristic);

	glm::vec2 m_mousePosition;

	// ImGui utility functions
	void m_ImGuiKeyMap();
	void m_ImGuiSetStyle();
	void m_updateUI();

	// ImGui menu variables
	bool m_exitApp = false;
	bool m_displayAbout = false;
	bool m_displayUI = true;

	// ImGui button variables
	ImVec4 m_manhattanButtonColour;
	ImVec4 m_euclideanButtonColour;
	
	void m_resetAll();

};

#endif /* defined (__PLAY_SCENE__) */