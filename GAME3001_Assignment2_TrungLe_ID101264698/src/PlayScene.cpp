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

		for (auto tile : m_closedList)
		{
			tile->setTileState(UNDEFINED);
			m_closedList.pop_back();
		}
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

void PlayScene::m_spawnShip()
{
	if (m_pShip->getTile() != nullptr)
	{
		m_pShip->getTile()->setTileState(UNDEFINED);
	}
	
	const auto randomTileIndex = m_spawnObject(m_pShip);
	m_pGrid[randomTileIndex]->setTileState(START);
}

void PlayScene::m_spawnPlanet()
{
	if (m_pPlanet->getTile() != nullptr)
	{
		m_pPlanet->getTile()->setTileState(UNDEFINED);
	}
	
	const auto randomTileIndex = m_spawnObject(m_pPlanet);
	auto tile = m_pGrid[randomTileIndex];
	m_computeTileValues();
	tile->setTileState(GOAL);
}

void PlayScene::PopulateGrid()
{
	// RANDOMIZE TILE TEXTURE, SET TILE STATE IF TEXTURE IS OBSTACLE
	for (auto tile : m_pGrid) {
		if (tile->getTileState() != START && tile->getTileState() != GOAL) {
			auto tile_type = static_cast<TileType>(rand() % NUM_OF_TILE_TYPES);
			tile->SetTileType(tile_type);
			if (tile_type > DEFAULT_TILE) {
				tile->setTileState(IMPASSABLE);
				/*if (!HasViablePath()) {
					tile->SetTileType(DEFAULT_TILE);
					tile->setTileState(UNDEFINED);
				}*/
			}
		}
	}
}

void PlayScene::m_computeTileValues()
{
	for (auto tile : m_pGrid)
	{
		tile->setHeuristic(m_heuristic);
		tile->setTargetDistance(m_pPlanet->getTile()->getGridPosition());
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
			// find the minimum value
			if (min > tile->getTileValue())
			{
				min = tile->getTileValue();
				minTile = tile;
			}
		}
	}

	// now for every tile in the neighbours vector
	for (auto tile : neighbours)
	{
		// check if the tile is not a nullptr and make sure it is only undefined
		if ((tile != nullptr) && (tile->getTileState() == UNDEFINED))
		{
			// mark the minimum tile as open and add it to the open list
			if (tile == minTile)
			{
				tile->setTileState(OPEN);
				m_openList.push_back(tile);
			}
			// mark the other tiles as closed and add them to the closed list
			else
			{
				tile->setTileState(CLOSED);
				m_closedList.push_back(tile);
			}
		}
	}

	return minTile;
}

void PlayScene::m_findShortestPath()
{
	//m_computeTileValues();

	auto tile = m_pShip->getTile();

	while(tile->getTileState() != GOAL)
	{
		const auto neighbours = tile->getNeighbours();

		tile = m_findLowestCostTile(neighbours);
	}
}

Tile* PlayScene::GetNeighborWithWantedState(const Tile* in_tile, const TileState in_state)
{
	for (auto neighbour_tile : in_tile->getNeighbours()) {
		if (neighbour_tile != nullptr) {
			if (neighbour_tile->getTileState() == in_state) {
				return neighbour_tile;
			}
		}
	}
	return nullptr;
}

bool PlayScene::HasNeighborWithWantedState(const Tile* in_tile, const TileState in_state)
{
	for (auto neighbour_tile : in_tile->getNeighbours()) {
		if (neighbour_tile != nullptr) {
			if (neighbour_tile->getTileState() == in_state) {
				return true;
			}
		}
	}
	return false;
}

bool PlayScene::HasViablePath()
{
	// RUN m_findShortestPath, ADD TILES TO shortest_path_, CLEAN shortest_path_ IF FALSE

	m_findShortestPath();
	
	// START WITH SHIP TILES, THEN WORK OUT IF ADJACENT TILES HAVE AN OPEN TILE
	auto test_tile = m_pShip->getTile();
	while (!HasNeighborWithWantedState(test_tile, GOAL)) {
		if (!HasNeighborWithWantedState(test_tile, OPEN)) {
			return false;
		}
		test_tile = GetNeighborWithWantedState(test_tile, OPEN); // URGENT FIX: DO NOT ALLOW SWITCHING BETWEEN 2 OPEN TILES
		if (test_tile == nullptr) {
			return false;
		}
	}
	return true;
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

void PlayScene::m_updateUI()
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
		ImGui::Text("Authors:");
		ImGui::Text("Tom Tsiliopoulos ");
		ImGui::End();
	}

	/*************************************************************************************************/
	if (ImGui::Button("Respawn Ship"))
	{
		m_spawnShip();
	}

	ImGui::SameLine();

	if (ImGui::Button("Respawn Planet"))
	{
		m_spawnPlanet();
	}

	ImGui::SameLine();

	if (ImGui::Button("Find Shortest Path"))
	{
		m_findShortestPath();
	}

	if(ImGui::CollapsingHeader("Heuristic Options"))
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

/*** SCENE FUNCTIONS ***/
void PlayScene::m_resetAll()
{
	
}

void PlayScene::start()
{
	// setup default heuristic
	m_heuristic = MANHATTAN;
	m_manhattanButtonColour = ImVec4(0.26f, 1.0f, 0.98f, 0.40f);
	m_euclideanButtonColour = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	
	m_buildGrid();
	m_mapTiles();

	// LOAD TEXTURES FOR TILES
	LoadTileTextures();

	// instantiate planet and add it to the DisplayList
	m_pPlanet = new Planet();
	addChild(m_pPlanet);

	m_spawnPlanet();
	
	// instantiate ship and add it to the DisplayList
	m_pShip = new Ship();
	addChild(m_pShip);

	m_spawnShip();

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
	/*m_pTile->update();
	m_pShip->update();*/

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
