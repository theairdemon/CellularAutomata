#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Block
{
private:
	int shape_type;
	int shape[4][4];

	int row; int column;

public:
	Block(int type, int r, int c);
	int getRow();
	int getColumn();
	int getState(int r, int c);
	int getShape();
	void update();

	void shiftLeft();
	void shiftRight(int max_column);
	void rotateLeft();
	void rotateRight();
};