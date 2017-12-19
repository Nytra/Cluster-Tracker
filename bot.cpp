#include "bot.h"

Bot::Bot() {
	_active = false;
}

void Bot::kill() {
	_active = false;
}

void Bot::activate() {
	_active = true;
}

bool Bot::isActive() {
	return _active;
}

int Bot::getXCoord() {
	return _x;
}

int Bot::getYCoord() {
	return _y;
}

sf::Color Bot::getColour() {
	return _colour;
}

void Bot::setPos(int x, int y) {
	_x = x;
	_y = y;
}

void Bot::setColour(sf::Color colour) {
	_colour = colour;
}