#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Cell
{
private:

	Vector2f position;

	// A RectangleShape object
	RectangleShape cellShape;

	// 0 == dead, 1 == alive
	int state;

public:
	// rows increase going down, column increase going to the right
	Cell();

	void setValues(int row, int column, int pixelScale);

	FloatRect getPosition();
	
	RectangleShape getShape();

	// return 0 if dead, 1 if alive
	int getState();

	// manually set the state of the cell
	void setState(int new_state);

};