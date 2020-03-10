#include "Tile.h"
#include <iomanip>
#include <sstream>
#include "Game.h"
#include "Util.h"


Tile::Tile(glm::vec2 world_position, glm::vec2 grid_position):
	m_gridPosition(grid_position)
{
	TheTextureManager::Instance()->load("../Assets/textures/tile.png",
		"tile", TheGame::Instance()->getRenderer());

	auto size = TheTextureManager::Instance()->getTextureSize("tile");
	setWidth(size.x);
	setHeight(size.y);
	setPosition(world_position);

	std::ostringstream tempLabel;
	tempLabel << std::fixed << std::setprecision(1) <<  m_tileValue;
	auto labelstring = tempLabel.str();

	SDL_Color label_color_ = { 0, 50, 0, 255 };
	auto closedOpenLabelPosition = glm::vec2(getPosition().x, getPosition().y - 5);
	m_pClosedOpenLabel = new Label("-", "Consolas", 12, label_color_, closedOpenLabelPosition);

	auto valueLabelPosition = glm::vec2(getPosition().x, getPosition().y + 10);
	m_pValueLabel = new Label(labelstring, "Consolas", 14, label_color_, valueLabelPosition, true);

	m_pNeighbours = { nullptr, nullptr, nullptr, nullptr };
	m_heuristic = MANHATTAN;
}

Tile::~Tile()
{
	delete m_pValueLabel;
	delete m_pClosedOpenLabel;
	m_pNeighbours.clear();
	m_pNeighbours.resize(0);
	m_pNeighbours.shrink_to_fit();
}

int Tile::GetId()
{
	return id_;
}

void Tile::SetId(int value)
{
	id_ = value;
}

void Tile::draw()
{
	const int xComponent = getPosition().x;
	const int yComponent = getPosition().y;

	TheTextureManager::Instance()->draw(kTileTypeTextureId[GetTileType()], xComponent, yComponent,
		TheGame::Instance()->getRenderer(), 0, 199, true);
	TheTextureManager::Instance()->draw("tile", xComponent, yComponent,
		TheGame::Instance()->getRenderer(), 0, 255, true);

	m_pClosedOpenLabel->draw();
	m_pValueLabel->draw();
}

void Tile::update()
{
}

void Tile::clean()
{
	
}

TileType Tile::GetTileType() const
{
	return tile_type_;
}

void Tile::SetTileType(const TileType type_idx)
{
	tile_type_ = type_idx;
}

Tile * Tile::getUp()
{
	return m_pNeighbours[UP];
}

Tile * Tile::getDown()
{
	return m_pNeighbours[DOWN];
}

Tile * Tile::getRight()
{
	return m_pNeighbours[RIGHT];
}

Tile * Tile::getLeft()
{
	return m_pNeighbours[LEFT];
}

void Tile::setUp(Tile * tile)
{
	m_pNeighbours[UP] = tile;
}

void Tile::setDown(Tile * tile)
{
	m_pNeighbours[DOWN] = tile;
}

void Tile::setRight(Tile * tile)
{
	m_pNeighbours[RIGHT] = tile;
}

void Tile::setLeft(Tile * tile)
{
	m_pNeighbours[LEFT] = tile;
}

void Tile::setTileState(const TileState state)
{
	m_tileState = state;

	switch(state)
	{
	case IMPASSABLE:
		setTileStateLabel("X");
		break;
	case OPEN:
		setTileStateLabel("O");
		break;
	case CLOSED:
		setTileStateLabel("C");
		break;
	case START:
		setTileStateLabel("S");
		break;
	case GOAL:
		setTileStateLabel("G");
		m_tileValue = 0;
		break;
	case UNDEFINED:
		setTileStateLabel("-");
		break;
	default:
		std::cout << "a state that has not been defined" << std::endl;
		break;
	}

	if (state == GOAL)
	{
		m_tileValue = 0;
	}
}

TileState Tile::getTileState() const
{
	return m_tileState;
}

void Tile::setTargetDistance(const glm::vec2 goal_location)
{
	m_goalLocation = goal_location;

	// declare heuristic;
	auto h = 0.0f;

	switch(m_heuristic)
	{
		case EUCLIDEAN:
			//euclidean distance heuristic
			h = Util::distance(getGridPosition(), goal_location);
			break;
		case MANHATTAN:
			//manhattan distance heuristic
			h = abs(getGridPosition().x - goal_location.x) +
				abs(getGridPosition().y - goal_location.y);
			break;
		default:
			break;
	}

	const float g = Config::TILE_COST;
	
	m_tileValue = g + h;

	std::ostringstream tempLabel;
	tempLabel << std::fixed << std::setprecision(1) << m_tileValue;
	const auto labelstring = tempLabel.str();
	m_pValueLabel->setText(labelstring);
}

void Tile::SetTargetDistance(float in_value)
{
	m_tileValue = in_value;
	if (in_value == INFINITY) {
		GetValueLabel()->setText("INF");
	}
	else {
		std::ostringstream tempLabel;
		tempLabel << std::fixed << std::setprecision(1) << m_tileValue;
		const auto labelstring = tempLabel.str();
		m_pValueLabel->setText(labelstring);
	}
	
}

glm::vec2 Tile::getGridPosition() const
{
	return m_gridPosition;
}

float Tile::getTileValue() const
{
	return m_tileValue;
}

void Tile::setTileStateLabel(const std::string& closed_open) const
{
	m_pClosedOpenLabel->setText(closed_open);

	const SDL_Color blue = { 0, 0, 255, 255 };
	m_pClosedOpenLabel->setColour(blue);
}

std::vector<Tile*> Tile::getNeighbours()
{
	return m_pNeighbours;
}

void Tile::setHeuristic(const Heuristic heuristic)
{
	m_heuristic = heuristic;
}

Label* Tile::GetValueLabel()
{
	return m_pValueLabel;
}

Tile* Tile::GetParentNode()
{
	return parent_node_;
}

void Tile::SetParentNode(Tile* in_tile)
{
	parent_node_ = in_tile;
}
