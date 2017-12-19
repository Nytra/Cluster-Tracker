#ifndef BOT_H_
#define BOT_H_

#include <SFML/Graphics.hpp>

class Bot {
private:
	int _x;
	int _y;
	sf::Color _colour;
	bool _active;
public:
	Bot();
	void kill();
	void activate();
	bool isActive();
	int getXCoord();
	int getYCoord();
	sf::Color getColour();
	void setPos(int x, int y);
	void setColour(sf::Color colour);
};

#endif