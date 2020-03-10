#include "PlayScene.h"
#include <algorithm>
#include <ctime>
#include <iomanip>
#include "Game.h"
#include "glm/gtx/string_cast.hpp"
#include "SceneState.h"
#include "Util.h"
#include "IMGUI_SDL/imgui_sdl.h"


// Pathfinding & Steering functions ***********************************************

void PlayScene::m_resetGrid()
{
		for (auto tile : m_openList)
		{
			tile->setTileState(UNDEFINED);
			m_openList.pop_back();
		}
		m_openList.clear();
		m_openList.resize(0);
		m_openList.shrink_to_fit();
		for (auto tile : m_closedList)
		{
			tile->setTileState(UNDEFINED);
			m_closedList.pop_back();
		}
		m_closedList.clear();
		m_closedList.resize(0);
		m_closedList.shrink_to_fit();
		for (auto tile : shortest_path_)
		{
			tile->setTileState(UNDEFINED);
			shortest_path_.pop_back();
		}
		shortest_path_.clear();
		shortest_path_.resize(0);
		shortest_path_.shrink_to_fit();
}

void PlayScene::m_buildGrid()
{
	// CONSTRUCTOR FOR m_pGrid
	const auto size = Config::TILE_SIZE;
	const auto offset = size * 0.5f;
	
	m_pGrid = std::vector<Tile*>(); // instantiates a structure of type vector<Tile*>

	for (auto row = 0; row < Config::ROW_NUM; ++row)
	{
		for (auto col = 0; col < Config::COL_NUM; ++col)
		{
			auto tile = new Tile(glm::vec2(offset + size * col, offset + size * row), 
				glm::vec2(col, row));
			addChild(tile);
			//tile->SetId(row*(800/40)+col);
			tile->setTileState(UNDEFINED);
			m_pGrid.push_back(tile);
		}
	}
}

void PlayScene::m_mapTiles()
{
	// CONSTRUCTOR FOR NEIGHBORS
	for (auto tile : m_pGrid)
	{
		const auto x = tile->getGridPosition().x;
		const auto y = tile->getGridPosition().y;

		if(y != 0)                   { tile->setUp   (m_pGrid[x + ((y - 1) * Config::COL_NUM)]); }
		if(x != Config::COL_NUM - 1) { tile->setRight(m_pGrid[(x + 1) + (y * Config::COL_NUM)]); }
		if(y != Config::ROW_NUM - 1) { tile->setDown (m_pGrid[x + ((y + 1) * Config::COL_NUM)]); }
		if(x != 0)					 { tile->setLeft (m_pGrid[(x - 1) + (y * Config::COL_NUM)]); }
	}
}

void PlayScene::LoadTileTextures()
{
	for (int i = 0; i < NUM_OF_TILE_TYPES; i++) {
		TheTextureManager::Instance()->load(kTileTypeTexture[i], kTileTypeTextureId[i], TheGame::Instance()->getRenderer());
	}
}

int PlayScene::m_spawnObject(PathFindingDisplayObject* object)
{
	m_resetGrid();
	
	Tile* randomTile = nullptr;
	auto randomTileIndex = 0;
	do
	{
		randomTileIndex = int(Util::RandomRange(0, m_pGrid.size() - 1));
		randomTile = m_pGrid[randomTileIndex];
	} while (randomTile->getTileState() != UNDEFINED); // search for empty tile, w/o textures


	if (object->getTile() != nullptr)
	{
		object->getTile()->setTileState(UNDEFINED);
	}

	object->setPosition(randomTile->getPosition());
	object->setTile(randomTile);

	return randomTileIndex;
}

int PlayScene::SpawnObjectAtGridIdx(PathFindingDisplayObject* object, int grid_idx)
{
	m_resetGrid();
	if (!impassable_list_.empty()) {
		for (auto tile : impassable_list_)
		{
			tile->setTileState(UNDEFINED);
			ComputeDistanceForOneTile(tile);
			impassable_list_.pop_back();
		}
	}

	Tile* tile = nullptr;
	tile = m_pGrid[grid_idx];


	if (object->getTile() != nullptr)
	{
		object->getTile()->setTileState(UNDEFINED);
	}

	object->setPosition(tile->getPosition());
	object->setTile(tile);
	object->getTile()->SetTileType(DEFAULT_TILE);
	//ComputeDistanceForOneTile(object->getTile());

	return grid_idx;
}

void PlayScene::ResetShipPosition()
{
	if (m_pShip->getTile() != nullptr)
	{
		m_pShip->setPosition(m_pShip->getTile()->getPosition());
	}
}

void PlayScene::m_spawnShip()
{
	ResetShipPosition();
	if (m_pShip->getTile() != nullptr)
	{
		m_pShip->getTile()->setTileState(UNDEFINED);
	}

	const auto randomTileIndex = m_spawnObject(m_pShip);
	m_pGrid[randomTileIndex]->setTileState(START);
	m_pShip->setState(IDLE);
}

void PlayScene::m_spawnPlanet()
{
	ResetShipPosition();
	if (m_pPlanet->getTile() != nullptr)
	{
		m_pPlanet->getTile()->setTileState(UNDEFINED);
	}

	const auto randomTileIndex = m_spawnObject(m_pPlanet);
	auto tile = m_pGrid[randomTileIndex];
	m_computeTileValues();
	tile->setTileState(GOAL);
}

void PlayScene::SpawnShipAtGridIdx(int grid_idx)
{
	ResetShipPosition();
	if (m_pShip->getTile() != nullptr)
	{
		m_pShip->getTile()->setTileState(UNDEFINED);
	}

	const auto randomTileIndex = SpawnObjectAtGridIdx(m_pShip, grid_idx);
	m_pGrid[randomTileIndex]->setTileState(START);
	m_pShip->setState(IDLE);
}

void PlayScene::SpawnPlanetAtGridIdx(int grid_idx)
{
	ResetShipPosition();
	if (m_pPlanet->getTile() != nullptr)
	{
		m_pPlanet->getTile()->setTileState(UNDEFINED);
	}

	const auto randomTileIndex = SpawnObjectAtGridIdx(m_pPlanet, grid_idx);
	auto tile = m_pGrid[randomTileIndex];
	m_computeTileValues();
	tile->setTileState(GOAL);
}

void PlayScene::PopulateGrid()
{
	m_resetGrid();
	for (auto tile : impassable_list_)
	{
		tile->setTileState(UNDEFINED);
		ComputeDistanceForOneTile(tile);
		impassable_list_.pop_back();
	}
	m_findShortestPath();
	std::cout << "generating asteroids..." << std::endl;
	// RANDOMIZE TILE TEXTURE, SET TILE STATE IF TEXTURE IS OBSTACLE
	for (auto tile : m_pGrid) {
		if (tile->getTileState() != START && tile->getTileState() != GOAL) {
			auto tile_type = static_cast<TileType>(rand() % NUM_OF_TILE_TYPES);
			tile->SetTileType(tile_type);
			if (tile_type > DEFAULT_TILE) {
				if (tile->getTileState() == OPEN) {
					//TileState original_state = tile->getTileState();
					tile->setTileState(IMPASSABLE);
					impassable_list_.push_back(tile);
					ComputeDistanceForOneTile(tile);
					m_findShortestPath();
					if (path_cost_ == -1) {
						tile->SetTileType(DEFAULT_TILE);
						//tile->setTileState(original_state);
						tile->setTileState(UNDEFINED);
						impassable_list_.pop_back();
						ComputeDistanceForOneTile(tile);
						m_findShortestPath();
					}
				}
				else {
					tile->setTileState(IMPASSABLE);
					impassable_list_.push_back(tile);
					ComputeDistanceForOneTile(tile);
				}
			}
		}
	}
}

void PlayScene::ComputeDistanceForOneTile(Tile* in_tile)
{
	if (in_tile->getTileState() == IMPASSABLE) {
		in_tile->SetTargetDistance(INFINITY);
	}
	else {
		in_tile->setHeuristic(m_heuristic);
		in_tile->setTargetDistance(m_pPlanet->getTile()->getGridPosition());
	}
}

void PlayScene::m_computeTileValues()
{
	for (auto tile : m_pGrid)
	{
		ComputeDistanceForOneTile(tile);
	}
}

Tile* PlayScene::m_findLowestCostTile(const std::vector<Tile*>& neighbours)
{
	Tile* minTile = nullptr;
	auto min = INFINITY;
	
	// for every tile in the neighbours vector
	for (auto tile : neighbours)
	{
		// ensure the tile you are inspecting is not a nullptr
		if (tile != nullptr)
		{
			if (tile->getTileState() == UNDEFINED || tile->getTileState() == GOAL) {
				// find the minimum value
				if (min > tile->getTileValue())
				{
					min = tile->getTileValue();
					minTile = tile;
				}
			}
			else {
				//std::cout << "Tile state of " << tile->getGridPosition().x << " " << tile->getGridPosition().y << " is " << tile->getTileState() << std::endl;
			}
		}
	}
	return minTile;
}

void PlayScene::m_findShortestPath()
{
	m_resetGrid();
	//m_computeTileValues();
	std::cout << "finding shortest. . ." << std::endl;
	auto tile = m_pShip->getTile();
	//std::cout << "Ship pos " << tile->getGridPosition().x << " " << tile->getGridPosition().y << std::endl;
	path_cost_ = 0;
	while(tile->getTileState() != GOAL)
	{
		const auto neighbours = tile->getNeighbours();

		tile = m_findLowestCostTile(neighbours);
		//std::cout << tile->getGridPosition().x << " " << tile->getGridPosition().y << std::endl;
		if (tile == nullptr) {
			std::cout << "NO PATH" << std::endl;
			path_cost_ = -1;
			m_resetGrid();
			break;
		}
		else if (tile->getTileState() == GOAL) {
			path_cost_ += tile->getTileValue();
		}
		else {
			shortest_path_.push_back(tile);
			path_cost_ += tile->getTileValue();
		}

		// now for every tile in the neighbours vector
		for (auto neighbor_tile : neighbours)
		{
			// check if the tile is not a nullptr and make sure it is only undefined
			if (neighbor_tile != nullptr) 
			{
				if (neighbor_tile->getTileState() == UNDEFINED) {
					neighbor_tile->setTileState(CLOSED);
					m_closedList.push_back(neighbor_tile);
				}
			}
		}
	}

	for (auto tile : m_closedList) {
		tile->setTileState(UNDEFINED);
	}

	if (!shortest_path_.empty()) {
		for (auto tile : shortest_path_) {
			tile->setTileState(OPEN);
		}
	}
}

std::pair<Tile*, int> PlayScene::FindLowestCostTileFromList(std::vector<Tile*> in_list)
{
	std::pair<Tile*, int> result;
	result.first = nullptr;
	result.second = -1;
	auto min = INFINITY;
	int count = 0;
	// for every tile in the neighbours vector
	for (auto tile : in_list)
	{
		// ensure the tile you are inspecting is not a nullptr
		if (tile != nullptr)
		{
			// find the minimum value
			if (min > tile->getTileValue())
			{
				min = tile->getTileValue();
				result.first = tile;
				result.second = count;
			}
		}
		count++;
	}

	return result;
}

bool PlayScene::IsIdInList(std::vector<Tile*> list, int id)
{
	for(auto tile : list){
		if (id == tile->GetId()) {
			return true;
		}
	}
	return false;
}

void PlayScene::FindShortestPathAStar()
{
	std::vector<Tile*> open_list;
	std::vector<Tile*> closed_list;
	shortest_path_.clear();
	shortest_path_.resize(0);
	shortest_path_.shrink_to_fit();
	open_list.push_back(m_pShip->getTile());
	while (!open_list.empty()) {
		std::pair<Tile*, int> curr_node = FindLowestCostTileFromList(open_list);
		if (curr_node.first->getTileState() == GOAL) {
			std::cout << "LAST LINE" << std::endl;
			auto curr = curr_node.first;
			while (curr->getTileState() != START) {
				std::cout << "ONE " << curr->getGridPosition().x << " " << curr->getGridPosition().y << std::endl;
				shortest_path_.push_back(curr);
				curr = curr->GetParentNode();
				std::cout << "TWO " << curr->getGridPosition().x << " " << curr->getGridPosition().y << std::endl;
			}
			break;
		}
		closed_list.push_back(curr_node.first);
		open_list.erase(open_list.begin() + curr_node.second);
		for (auto neighbor : curr_node.first->getNeighbours()) {
			if (neighbor != nullptr) {
				if (!IsIdInList(open_list, neighbor->GetId())) {
					neighbor->SetParentNode(curr_node.first);
					open_list.push_back(neighbor);
					//std::cout << "ONE " << curr_node.first->getGridPosition().x << " " << curr_node.first->getGridPosition().y << std::endl;
				}
				else if (curr_node.first->getTileValue() < neighbor->getTileValue()) {
					neighbor->SetParentNode(curr_node.first);
					//std::cout << "TWO " << curr_node.first->getGridPosition().x << " " << curr_node.first->getGridPosition().y << std::endl;
				}
			}
		}
	}
	for (auto tile : shortest_path_) {
		tile->setTileState(OPEN);
	}
}

Tile* PlayScene::GetTileFromPosition(glm::vec2 position)
{
	//tile size: 40, fomula: int oneDindex = (row * length_of_row) + column; // Indexes
	int column = int(position.x / 40);
	int row = int(position.y / 40);
	int row_length = int(800 / 40);
	return m_pGrid[(row * row_length) + column];
}

void PlayScene::SetTileFromPosition(PathFindingDisplayObject* in_ptr)
{
	in_ptr->setTile(GetTileFromPosition(in_ptr->getPosition()));
}

void PlayScene::MoveStartToGoal()
{
	if (!shortest_path_.empty()) {
		std::vector<glm::vec2> open_position_list;
		for (auto node : shortest_path_) {
			open_position_list.push_back(node->getPosition());
		}
		open_position_list.push_back(m_pPlanet->getPosition());
		m_pShip->SetTargetPath(open_position_list);
		for (auto node : m_pShip->GetTargetPath()) {
			std::cout << node.x << " " << node.y << std::endl;
		}
		m_pShip->setState(TRAVERSE);
	}
}

Tile* PlayScene::GetNeighborWithWantedState(Tile* in_tile, const TileState in_state)
{
	if (in_tile != nullptr) {
		for (auto neighbour_tile : in_tile->getNeighbours()) {
			if (neighbour_tile != nullptr) {
				if (neighbour_tile->getTileState() == in_state) {
					return neighbour_tile;
				}
			}
		}
	}
	return nullptr;
}

bool PlayScene::HasViablePath()
{
	// RUN m_findShortestPath, ADD TILES TO shortest_path_, CLEAN shortest_path_ IF FALSE
	m_findShortestPath();
	
	if (path_cost_ == -1) {
		return false;
	}
	else {
		return true;
	}

	//std::vector<Tile*> checked_tiles;
	//// START WITH SHIP TILE, THEN WORK OUT IF ADJACENT TILES HAVE AN OPEN TILE
	//auto test_tile = GetNeighborWithWantedState(m_pShip->getTile(), OPEN);
	//if (test_tile == nullptr) {
	//	if (GetNeighborWithWantedState(test_tile, GOAL) != nullptr) {
	//		return true;
	//	}
	//	else {
	//		std::cout << "EARLY TERMINATION" << std::endl;
	//		return false;
	//	}
	//}
	//checked_tiles.push_back(test_tile);
	//test_tile->setTileState(CLOSED);
	//std::cout << "LINE 1" << std::endl;
	//while (GetNeighborWithWantedState(test_tile, GOAL) == nullptr) {
	//	std::cout << "LINE 2" << std::endl;
	//	Tile* next_tile = nullptr;
	//	for (const auto neighbour_tile : test_tile->getNeighbours()) { //copied from GetNeighborWithWantedState()
	//		std::cout << "LINE 3" << std::endl;
	//		if (neighbour_tile != nullptr) {
	//			if (neighbour_tile->getTileState() == OPEN) {
	//				std::cout << "LINE 4" << std::endl;
	//				checked_tiles.push_back(neighbour_tile);
	//				next_tile = neighbour_tile;
	//				test_tile = neighbour_tile;
	//				//neighbour_tile->setTileState(CLOSED);
	//				break;
	//			}
	//		}
	//	}
	//	checked_tiles.back()->setTileState(CLOSED);
	//	if (next_tile == nullptr) {
	//		std::cout << "LINE 5" << std::endl;
	//		SetStateForTileList(checked_tiles, OPEN);
	//		return false;
	//	}
	//}
	//SetStateForTileList(checked_tiles, OPEN);
	//return true;
}

void PlayScene::SetStateForTileList(std::vector<Tile*> in_list, const TileState in_state)
{
	if (!in_list.empty()) {
		for (auto tile : in_list) {
			tile->setTileState(in_state);
		}
	}
}

void PlayScene::m_selectHeuristic(Heuristic heuristic)
{
	// recalculate grid
	m_heuristic = heuristic;
	auto start = m_pShip->getTile();
	auto goal = m_pPlanet->getTile();
	m_resetGrid();
	m_computeTileValues();
	start->setTileState(START);
	goal->setTileState(GOAL);
	m_findShortestPath();

	// change button colour depending on heuristic chosen
	switch(heuristic)
	{
	case MANHATTAN:
		m_manhattanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 1.0f);
		m_euclideanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		break;
	case EUCLIDEAN:
		m_manhattanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		m_euclideanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 1.0f);
		break;
	}
	
}

// ImGui functions ***********************************************



void PlayScene::m_ImGuiKeyMap()
{
	auto& io = ImGui::GetIO();

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;

	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
}

void PlayScene::m_ImGuiSetStyle()
{
	auto& style = ImGui::GetStyle();

	style.Alpha = 0.8f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

void PlayScene::UpdateSettingsUI()
{
	// Prepare Window Frame
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow(); // use for debug purposes

	std::string windowString = "Settings ";

	ImGui::Begin(&windowString[0], NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar);

	// set window to top getLeft corner
	ImGui::SetWindowPos(ImVec2(0, 0), true);

	/*************************************************************************************************/
	/* MENU                                                                                          */
	/*************************************************************************************************/

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::Separator();
			ImGui::MenuItem("Exit", NULL, &m_exitApp);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::Separator();
			ImGui::MenuItem("About", NULL, &m_displayAbout);
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	if (m_exitApp)
	{
		TheGame::Instance()->quit();
	}

	if (m_displayAbout)
	{
		ImGui::Begin("About Pathfinding Simulator", &m_displayAbout, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Separator();
		ImGui::Text("Template by:");
		ImGui::Text("Tom Tsiliopoulos ");
		ImGui::Text("Modified by:");
		ImGui::Text("Trung Le (Kyle) & Shu Deng");
		ImGui::End();
	}

	/*************************************************************************************************/
	// HUD
	std::string path_score_str = "Total Score: " + std::to_string(path_cost_ * 100);
	if (path_cost_ == -1) {
		path_score_str = "Total Score: NO PATH";
	}
	ImGui::Text(path_score_str.c_str());
	std::string path_cost_str = "Total path cost: " + std::to_string(path_cost_);
	ImGui::Text(path_cost_str.c_str());

	// COMMANDS
    //fomula: int oneDindex = (row * length_of_row) + column;
	if (ImGui::Button("Load Preset 1")) {
		SpawnShipAtGridIdx(19);
		SpawnPlanetAtGridIdx(14 * 20 + 0);
		PopulateGrid();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Preset 2")) {
		SpawnShipAtGridIdx(7 * 20 + 9);
		SpawnPlanetAtGridIdx(14 * 20 + 19);
		PopulateGrid();
	}
	ImGui::SameLine();
	if (ImGui::Button("Load Preset 3")) {
		SpawnShipAtGridIdx(7 * 20 + 0);
		SpawnPlanetAtGridIdx(7 * 20 + 19);
		PopulateGrid();
	}

	ImGui::NewLine();
	if (ImGui::Button("Respawn Ship (Random)")) {
		m_spawnShip();
	}
	if (ImGui::Button("Respawn Planet")){
		m_spawnPlanet();
	}
	ImGui::SameLine();
	if (ImGui::Button("Respawn Obstacles")){
		PopulateGrid();
	}

	ImGui::NewLine();
	if (ImGui::Button("Find Shortest Path")){
		//FindShortestPathAStar();
		m_findShortestPath();
	}
	ImGui::SameLine();
	if (ImGui::Button("Move Ship")){
		MoveStartToGoal();
	}

	if (ImGui::CollapsingHeader("Heuristic Options"))
	{
		ImGui::PushStyleColor(ImGuiCol_Button, m_manhattanButtonColour);
		if (ImGui::Button("Manhattan Distance"))
		{
			m_selectHeuristic(MANHATTAN);
		}
		ImGui::PopStyleColor();

		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, m_euclideanButtonColour);
		if (ImGui::Button("Euclidean Distance"))
		{
			m_selectHeuristic(EUCLIDEAN);
		}
		ImGui::PopStyleColor();
	}

	// Main Window End
	ImGui::End();
}

void PlayScene::UpdateHUD()
{
	
}

void PlayScene::m_updateUI()
{
	UpdateSettingsUI();
	//UpdateHUD();
}

/*** SCENE FUNCTIONS ***/
void PlayScene::m_resetAll()
{
	
}

void PlayScene::start()
{
	TheSoundManager::Instance()->playMusic("encounter", -1);
	
	// setup default heuristic
	m_heuristic = MANHATTAN;
	m_manhattanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 0.40f);
	m_euclideanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	
	m_buildGrid();
	m_mapTiles();

	// LOAD TEXTURES FOR TILES
	LoadTileTextures();

	// instantiate ship and add it to the DisplayList
	m_pShip = new Ship();
	addChild(m_pShip);

	m_spawnShip();

	// instantiate planet and add it to the DisplayList
	m_pPlanet = new Planet();
	addChild(m_pPlanet);

	m_spawnPlanet();

	PopulateGrid();
}

PlayScene::PlayScene()
{
	PlayScene::start();
}

PlayScene::~PlayScene()
= default;

void PlayScene::draw()
{
	if (m_displayUI)
	{
		for (auto tile : m_pGrid)
		{
			tile->draw();
		}
	}

	m_pPlanet->draw();
	
	m_pShip->draw();

	// ImGui Rendering section - DO NOT MOVE OR DELETE
	if (m_displayUI)
	{
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
		SDL_SetRenderDrawColor(TheGame::Instance()->getRenderer(), 255, 255, 255, 255);
	}
}

void PlayScene::update()
{
	//m_pTile->update();
	m_pShip->update();
	//if (m_pShip->getState() == TRAVERSE) {
	//	SetTileFromPosition(m_pShip);
	//}

	if (m_displayUI)
	{
		m_updateUI();
	}

}

void PlayScene::clean()
{
	std::cout << "PlayScene Clean Called" << std::endl;
	delete m_pShip;
	delete m_pPlanet;

	for (auto tile : m_pGrid)
	{
		delete tile;
		tile = nullptr;
	}
	m_pGrid.clear();
	m_pGrid.resize(0);
	m_pGrid.shrink_to_fit();

	for (auto mine : m_mines)
	{
		delete mine;
		mine = nullptr;
	}
	m_mines.clear();
	m_mines.resize(0);
	m_mines.shrink_to_fit();

	m_openList.clear();
	m_openList.resize(0);
	m_openList.shrink_to_fit();

	m_closedList.clear();
	m_closedList.resize(0);
	m_openList.shrink_to_fit();

	removeAllChildren();
}

void PlayScene::handleEvents()
{
	auto& io = ImGui::GetIO();
	auto wheel = 0;

	SDL_Event event;
	if (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_QUIT:
			TheGame::Instance()->quit();
			break;
		case SDL_MOUSEMOTION:
			m_mousePosition.x = event.motion.x;
			m_mousePosition.y = event.motion.y;
			break;
		case SDL_MOUSEWHEEL:
			wheel = event.wheel.y;
			break;
		case SDL_TEXTINPUT:
			io.AddInputCharactersUTF8(event.text.text);
			break;
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				TheGame::Instance()->quit();
				break;
			case SDLK_1:
				TheGame::Instance()->changeSceneState(SceneState::START_SCENE);
				break;
			case SDLK_2:
				TheGame::Instance()->changeSceneState(SceneState::END_SCENE);
				break;
			case SDLK_BACKQUOTE:
				m_displayUI = (m_displayUI) ? false : true;
				break;
			case SDLK_f:
				m_findShortestPath();
				break;
			case SDLK_g:
				
				break;
			case SDLK_o:
				m_spawnShip();
				break;
			case SDLK_p:
				m_spawnPlanet();
				break;
			case SDLK_r:
				m_resetAll();
				break;

				/************************************************************************/
			case SDLK_w:
				
				break;
			case SDLK_s:
				
				break;
			case SDLK_a:
				
				break;
			case SDLK_d:
				
				break;
			case SDLK_m:
				
				break;
			default:
				
				break;
			}
			{
				const int key = event.key.keysym.scancode;
				IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
				io.KeysDown[key] = (event.type == SDL_KEYDOWN);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			}
			break;
		case SDL_KEYUP:
			switch (event.key.keysym.sym)
			{
			case SDLK_w:
				
				break;

			case SDLK_s:
				
				break;

			case SDLK_a:
				
				break;
			case SDLK_d:
				
				break;
			default:
				
				break;
			}
			{
				const int key = event.key.keysym.scancode;
				IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
				io.KeysDown[key] = (event.type == SDL_KEYDOWN);
				io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
				io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
				io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
				io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
			}
			break;
		default:
			
			break;
		}
	}

	io.DeltaTime = 1.0f / 60.0f;
	int mouseX, mouseY;
	const int buttons = SDL_GetMouseState(&mouseX, &mouseY);
	io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io.MouseWheel = static_cast<float>(wheel);

	io.DisplaySize.x = 1280;
	io.DisplaySize.y = 720;

	m_ImGuiKeyMap();
	m_ImGuiSetStyle();
}
