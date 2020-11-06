#include "Block.h"

// This the constructor and it is called when we create an object
Block::Block(int type, int r, int c)
{
	shape_type = type;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			shape[i][j] = 0;
		}
	}
	
	if (shape_type == 0) {
		shape[0][1] = 1;
		shape[1][1] = 1;
		shape[2][1] = 1;
		shape[3][1] = 1;
	}
	// J-block
	else if (shape_type == 1) {
		shape[0][0] = 1;
		shape[1][0] = 1;
		shape[1][1] = 1;
		shape[1][2] = 1;
	}
	// L-block
	else if (shape_type == 2) {
		shape[0][2] = 1;
		shape[1][0] = 1;
		shape[1][1] = 1;
		shape[1][2] = 1;
	}
	// O-block
	else if (shape_type == 3) {
		shape[1][1] = 1;
		shape[1][2] = 1;
		shape[2][1] = 1;
		shape[2][2] = 1;
	}
	// S-block
	else if (shape_type == 4) {
		shape[0][1] = 1;
		shape[0][2] = 1;
		shape[1][0] = 1;
		shape[1][1] = 1;
	}
	// T-block
	else if (shape_type == 5) {
		shape[0][1] = 1;
		shape[1][0] = 1;
		shape[1][1] = 1;
		shape[1][2] = 1;
	}
	// Z-block
	else if (shape_type == 6) {
		shape[0][0] = 1;
		shape[0][1] = 1;
		shape[1][1] = 1;
		shape[1][2] = 1;
	}

	row = r; column = c;
}

int Block::getRow() {
	return row;
}
int Block::getColumn() {
	return column;
}
int Block::getState(int r, int c) {
	return shape[r][c];
}
int Block::getShape() {
	return shape_type;
}
void Block::update() {
	row++;
}

void Block::shiftLeft() {
	int farthest_left = 3;
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			if (shape[r][c] == 1) {
				farthest_left = std::min(c, farthest_left);
			}
		}
	}
	
	if (column + farthest_left > 0) {
		column--;
	}
}
void Block::shiftRight(int max_column) {
	int farthest_right = 0;
	for (int r = 0; r < 4; r++) {
		for (int c = 0; c < 4; c++) {
			if (shape[r][c] == 1) {
				farthest_right = std::max(c, farthest_right);
			}
		}
	}
	if (column + farthest_right < max_column - 1) {
		column++;
	}
}
void Block::rotateLeft() {

	if (shape_type == 0 || shape_type == 3) {
		int temp_shape[4][4] = { 0 };

		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				int state = shape[r][c];
				int new_r = std::abs(c - 3);
				int new_c = r;
				temp_shape[new_r][new_c] = state;
			}
		}

		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				shape[r][c] = temp_shape[r][c];
			}
		}
	}
	else {
		int temp_shape[4][4] = { 0 };

		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++) {
				int state = shape[r][c];
				int new_r = std::abs(c - 2);
				int new_c = r;
				temp_shape[new_r][new_c] = state;
			}
		}

		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				shape[r][c] = temp_shape[r][c];
			}
		}
	}
	
}
void Block::rotateRight() {
	if (shape_type == 0 || shape_type == 3) {
		int temp_shape[4][4] = { 0 };

		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				int state = shape[r][c];
				int new_r = c;
				int new_c = std::abs(r - 3);
				temp_shape[new_r][new_c] = state;
			}
		}

		for (int r = 0; r < 4; r++) {
			for (int c = 0; c < 4; c++) {
				shape[r][c] = temp_shape[r][c];
			}
		}
	}
	else {
		int temp_shape[4][4] = { 0 };

		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++) {
				int state = shape[r][c];
				int new_r = c;
				int new_c = std::abs(r - 2);
				temp_shape[new_r][new_c] = state;
			}
		}

		for (int r = 0; r < 3; r++) {
			for (int c = 0; c < 3; c++) {
				shape[r][c] = temp_shape[r][c];
			}
		}
	}
}
