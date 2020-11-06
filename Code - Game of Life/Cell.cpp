#include "Cell.h"
#include <cstdlib>

// This the constructor and it is called when we create an object
Cell::Cell() {	
}

void Cell::setValues(int row, int column, int pixelScale) {
	position.x = static_cast<float>(row * pixelScale);
	position.y = static_cast<float>(column * pixelScale);

	cellShape.setSize(sf::Vector2f(static_cast<float>(pixelScale), static_cast<float>(pixelScale)));
	cellShape.setPosition(position);

	state = 0;
}


FloatRect Cell::getPosition() {
	return cellShape.getGlobalBounds();
}

RectangleShape Cell::getShape() {
	return cellShape;
}

// return 0 if dead, 1 if alive
int Cell::getState() {
	return state;
}

// switch int state to 0 if currently 1 , and 1 if currently 0
void Cell::setState(int new_state) {
	state = new_state;
}