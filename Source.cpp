#include <iostream>
#include <random>
#include <vector>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include "constants.h"
#include "bot.h"

int width = GetSystemMetrics(SM_CXSCREEN) / 2;
int height = width;
double aspectRatio = width / height;
int cellWidth = width / 100;
int cellHeight = cellWidth * aspectRatio;
int cellHcount = width / cellWidth;
int cellVcount = cellHcount * aspectRatio;
int cellCount = cellHcount * cellVcount;

int scanSize = cellHcount / 20; // in cells
int scanRegionWidth = cellWidth * scanSize; // pixels per scan region
int scanRegionHeight = scanRegionWidth; // pixels per scan region
int scanRegionHcount = width / scanRegionWidth;
int scanRegionVcount = height / scanRegionHeight;

static Bot bots[maxBots];
static int *cells = new int[cellVcount * cellHcount];
sf::RenderWindow window(sf::VideoMode(width, height), "Simulation");
sf::RectangleShape cell(sf::Vector2f(cellWidth, cellHeight));

enum class Behaviour {
	CLUSTER_ANY = 0,
	CLUSTER_COLOUR,
	RANDOM
};
Behaviour behaviour = Behaviour::CLUSTER_COLOUR;

struct Coord {
	int x;
	int y;
	int value;
};

int sign(int val) {

	if (val < 0) {
		return -1;
	}
	else {
		return 1;
	}
}

bool checkCell(int x, int y) {

	if (cells[(y * cellHcount) + x] == 1) {
		return true;
	}
	else {
		return false;
	}
}

void shuffleBots() { 
	std::random_shuffle(std::begin(bots), std::end(bots));
}

void validatePosition(int &x, int &y) {

	if (x >= cellHcount) {
		x = cellHcount - 1;
	}
	else if (x < 0) {
		x = 0;
	}

	if (y >= cellVcount) {
		y = cellVcount - 1;
	}
	else if (y < 0) {
		y = 0;
	}
}

void findClusters(std::vector<Coord> &coords, double density) {
	Coord c;
	int count, dy, dx;
	int scanCellCount = scanSize * scanSize;
	int threshold = scanCellCount * density; // if number of occupied cells is over threshold then mark the point
	bool flag = false; // means ignore this cluster

	for (int y = 0; y < cellVcount - scanSize / 2; y += scanSize / 2) {

		for (int x = 0; x < cellHcount - scanSize / 2; x += scanSize / 2) {
			flag = true;

			if (x < cellHcount - scanSize && y < cellVcount - scanSize) {
				count = 0;

				for (int j = y; j < y + scanSize; j++) {

					for (int i = x; i < x + scanSize; i++) {

						if (checkCell(i, j)) {
							count += 1; // count neighbouring cells
						}
					}
				}

				if (count > threshold) { // if cell count is over threshold then remove the ignore flag
					flag = false;

				}

				if (!flag) {
					bool sameCluster = false;
					int checkSize = scanSize * 2.5;

					// check around this point for other prviously marked points
					for (int i = 0; i < coords.size(); i++) {

						dy = abs(y - coords[i].y);
						dx = abs(x - coords[i].x);

						if (dy + dx < checkSize) {
							sameCluster = true;
							continue;
						}

						for (int j = y - checkSize; j < y + checkSize; j++) { 

							for (int k = x - checkSize; k < x + checkSize; k++) {

								if (k == coords[i].x && j == coords[i].y) { 
									sameCluster = true;
								}
							}
						}
					}

					if (!sameCluster) {
						c.x = x;
						c.y = y;
						//c.value = count;
						coords.push_back(c);
						flag = true;
					}
				}
			}
		}
	}
}

void findSpace(int &x, int &y, int radius) {
	// Tries to find a random empty space within a certain radius around the x, y coords
	bool spaceAvailable = false;
	std::vector<Coord> spaces;
	Coord space;

	for (int cy = y - radius; cy <= y + radius; cy++) {

		if (cy < 0 || cy > cellVcount - 1) {
			continue;
		}

		for (int cx = x - radius; cx <= x + radius; cx++) {

			if (cx < 0 || cx > cellHcount - 1) {
				continue;
			}

			if (!checkCell(cx, cy)){
				spaceAvailable = true;
				space.x = cx;
				space.y = cy;
				spaces.push_back(space);
			}
		}
	}

	if (spaceAvailable) {
		random_shuffle(begin(spaces), end(spaces));
		space = spaces[rand() % spaces.size()];
		x = space.x;
		y = space.y;
	}
}

void spawn(int x, int y, sf::Color colour) { 
	// Attempts to place a new bot in an empty space within a certain radius of the x, y coords given
	int tx = x;
	int ty = y;
	findSpace(x, y, spawnRadius);

	if (x == tx && y == ty) {
		return;
	}

	for (int i = 0; i < maxBots; i++) {
		if (!bots[i].isActive()) {
			bots[i].setPos(x, y);
			bots[i].setColour(colour);
			bots[i].activate();
			break;
		}
	}
}

sf::Color randomColour() { 
	sf::Color colour;

	if (behaviour == Behaviour::CLUSTER_ANY) {
		int r = rand() % 255;
		int g = rand() % 255;
		int b = rand() % 255;
		colour = sf::Color(r, g, b);
	}
	else {
		int i = rand() % 3;

		switch (i) {
		case 0:
			colour = sf::Color::Red;
			break;
		case 1:
			colour = sf::Color::Green;
			break;
		case 2:
			colour = sf::Color::Blue;
			break;
		}
	}

	return colour;
}

int findTarget(int currentIndex) { 
	// Returns the index of the nearest bot
	int x = bots[currentIndex].getXCoord();
	int y = bots[currentIndex].getYCoord();
	int dx = 0;
	int dy = 0;
	int pdx = width;
	int pdy = height;
	bool targetFound = false;
	int targetIndex = -1;

	for (int i = 0; i < maxBots; i++) {

		if (bots[i].isActive() && i != currentIndex) {

			dy = abs(y - bots[i].getYCoord());
			dx = abs(x - bots[i].getXCoord());

			if (behaviour == Behaviour::CLUSTER_COLOUR && bots[currentIndex].getColour() != bots[i].getColour()) {
				continue;
			}

			if (dy < pdy || dx < pdx) {

				pdy = dy;
				pdx = dx;
				targetIndex = i;
				targetFound = true;
			}
		}
	}

	return targetIndex;
}

void initCells() {

	for (int y = 0; y < cellVcount; y++) {

		for (int x = 0; x < cellHcount; x++) {
			cells[(y * cellHcount) + x] = 0;
		}
	}
}

void setCell(int x, int y) {
	cells[(y * cellHcount) + x] = 1;
}

int main() {
	sf::RectangleShape cluster(sf::Vector2f(scanRegionWidth, scanRegionHeight));
	sf::Font font;
	sf::Text text;
	sf::Clock clock;
	std::vector<Coord> clusters;
	window.setFramerateLimit(FPS);
	cluster.setOutlineColor(sf::Color::Black);
	cluster.setFillColor(sf::Color::Transparent);
	cluster.setOutlineThickness(5);
	font.loadFromFile("font.ttf");
	text.setFont(font);
	text.setFillColor(sf::Color::Black);
	text.setOutlineColor(sf::Color::White);
	text.setOutlineThickness(1);
	text.setCharacterSize(20);
	text.setPosition(0, 0);
	srand(time(NULL));
	sf::Time prevTime = clock.getElapsedTime();
	int spawnedBots = 0;
	bool stopSpawning = false;
	initCells();

	while (window.isOpen()) {
		sf::Event event;

		while (window.pollEvent(event)) {

			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		shuffleBots();
		window.clear(sf::Color::White);
		initCells(); 
		sf::Time newTime = clock.getElapsedTime();

		if (newTime.asSeconds() - prevTime.asSeconds() > spawnDelay && !stopSpawning) {
			prevTime = clock.getElapsedTime();
			int x = rand() % cellHcount;
			int y = rand() % cellVcount;
			spawn(x, y, randomColour());
			spawnedBots += 1;
		}

		if (spawnedBots >= maxBots) {
			stopSpawning = true;
		}

		int activeBots = 0;

		for (int i = 0; i < maxBots; i++) {

			if (bots[i].isActive()) {

				activeBots += 1;
				int currentIndex = i;
				int x = bots[i].getXCoord();
				int y = bots[i].getYCoord();
				int px = x;
				int py = y;
				int targetBot = findTarget(currentIndex);
				bool targetFound;

				if (targetBot == -1) {
					targetFound = false;
				}
				else {
					targetFound = true;
				}

				if (targetFound == true && behaviour != Behaviour::RANDOM) {

					if (behaviour == Behaviour::CLUSTER_ANY) {
						x = bots[targetBot].getXCoord();
						y = bots[targetBot].getYCoord();
					}

					if (behaviour == Behaviour::CLUSTER_COLOUR) {

						if (bots[currentIndex].getColour() == bots[targetBot].getColour()) {
							x = bots[targetBot].getXCoord();
							y = bots[targetBot].getYCoord();
						}
					}
				}
				else {
					x += -1 + (rand() % static_cast<int>(1 - -1 + 1));
					y += -1 + (rand() % static_cast<int>(1 - -1 + 1));
				}

				validatePosition(x, y);

				if (checkCell(x, y)) {
					int tx = x;
					int ty = y;
					findSpace(x, y, moveRadius);

					if (x == tx && y == ty) {
						x = px;
						y = py;
					}
				}

				bots[currentIndex].setPos(x, y);

				if (!checkCell(x, y)) {
					cell.setPosition(sf::Vector2f(x * cellWidth, y * cellHeight));
					cell.setFillColor(bots[i].getColour());
					window.draw(cell);
					setCell(x, y);
				}
			}
		}

		if (enableNoise) {

			for (int y = 0; y < cellVcount; y++) { // add random noise

				for (int x = 0; x < cellHcount; x++) {

					if (rand() % 1000 < noiseDensity) {
						setCell(x, y);
						cell.setPosition(sf::Vector2f(x * cellWidth, y * cellHeight));
						cell.setFillColor(sf::Color::Yellow);
						window.draw(cell);
					}
				}
			}
		}

		clusters.clear();
		findClusters(clusters, 0.9);

		for (int i = 0; i < clusters.size(); i++) {
			int x = clusters[i].x;
			int y = clusters[i].y;
			cluster.setPosition(sf::Vector2f(x * cellWidth, y * cellHeight));
			window.draw(cluster);
		}

		text.setString("Cluster Count: " + std::to_string(clusters.size()));
		text.setPosition(sf::Vector2f(0, 0));
		text.setCharacterSize(20);
		window.draw(text);
		window.display();
	}

	return 0;
}