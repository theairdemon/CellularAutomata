#include "Block.h"
#include <string>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <SFML/Graphics.hpp>

// Avoid having to put sf in front of all the SFML classes and functions
using namespace sf;

// define the constant values of rows/columns of cells
const int ROWS = 20;
const int COLUMNS = 10;

// declare functions fleshed out below
RectangleShape makeShape(int row, int column, int pixelScale);
int surroundingCells(int r, int c, int grid[ROWS][COLUMNS]);
int rollDie(int last_num);

// This is where our game starts from
int main()
{
	int windowWidth = 300;
	int windowHeight = 2 * windowWidth + 100;
	RenderWindow window(VideoMode(windowWidth, windowHeight), "TETRIS");
	// measure framerate
	Clock clock;
	float lastTime = 0;
	window.setFramerateLimit(60);

	// gravity = 10 => it takes 10 frames to move one cell
	// more like inverse gravity; the bigger it is, the slower it moves
	int start_gravity = 30;
	int gravity = start_gravity;

	int pixelScale = std::min(windowWidth / COLUMNS, windowHeight / ROWS);
	bool set_outline = true;

	Color dead = Color(0, 0, 0, 255);
	Color alive = Color(255, 255, 255, 255);
	Color lines = Color(50, 50, 50, 255);
	Color alive_still = Color(50, 50, 220, 255);

	// 0 == dead and moving, 1 == alive and moving, 2 == alive and still
	int grid[ROWS][COLUMNS] = { 0 };
	int temp_grid[ROWS][COLUMNS] = { 0 };
	Text hud;
	Font font;
	font.loadFromFile("DS-DIGIT.ttf");
	hud.setFont(font);
	hud.setCharacterSize(50);
	hud.setFillColor(Color::White);
	hud.setPosition(static_cast<float>(window.getSize().x*0.02), static_cast<float>(window.getSize().y - 85));

	// initialize first block type
	int block_type = rollDie(7);
	Block block1 = Block(block_type, 0, 4);
	Block held_block = Block(block_type, 0, 4);
	
	bool game_started = false;
	bool game_over = false;
	int generation = 0;
	int score = 0;
	bool hard_drop = false;
	bool holding_block = false;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			bool can_input = true;
			if (event.type == Event::Closed) {
				window.close();
			}

			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					//std::cout << "mouse: (" << event.mouseButton.x << ", " << event.mouseButton.y << ")" << std::endl;
					float click_x = static_cast<float>(event.mouseButton.x);
					float click_y = static_cast<float>(event.mouseButton.y);

					if (click_y > windowHeight - 100) {
						game_started = true;
					}
					//std::cout << "========================" << std::endl;
				}
			}

			/* GAME CONTROLS
			escape == quit
			enter == begin
			left arrow == shiftLeft
			right arrow == shiftRight
			Z == rotateLeft
			X == rotateRight
			C == holdBlock
			down arrow == softDrop
			up arrow == hardDrop
			*/
			else if (event.type == Event::KeyPressed && can_input) {
				can_input = false;
				if (event.key.code == Keyboard::Escape) {
					window.close();
				}
				else if (event.key.code == Keyboard::Enter) {
					game_started = true;
				}
				else if (event.key.code == Keyboard::Left) {
					block1.shiftLeft();
				}
				else if (event.key.code == Keyboard::Right) {
					block1.shiftRight(COLUMNS);
				}
				else if (event.key.code == Keyboard::Z) {
					block1.rotateLeft();
				}
				else if (event.key.code == Keyboard::X) {
					block1.rotateRight();
				}
				else if (event.key.code == Keyboard::C) {
					if (!holding_block) {
						holding_block = true;
						// store current block
						int current_block_type = block1.getShape();
						held_block = Block(current_block_type, 0, 4);
						// get new block
						block_type = rollDie(block_type);
						block1 = Block(block_type, 0, 4);
					}
					else {
						int held_block_type = held_block.getShape();
						int current_block_type = block1.getShape();
						held_block = Block(current_block_type, 0, 4);
						block1 = Block(held_block_type, 0, 4);
					}
				}
				else if (event.key.code == Keyboard::R) {
					block_type = rollDie(block_type);
					block1 = Block(block_type, 0, 4);
					gravity = start_gravity;
					score = 0;

					for (int r = 0; r < ROWS; r++) {
						for (int c = 0; c < COLUMNS; c++) {
							grid[r][c] = 0;
						}
					}

					game_started = true;
				}
				else if (event.key.code == Keyboard::Up) {
					hard_drop = true;
				}
			}

			else if (event.type == Event::KeyReleased) {
				can_input = true;
			}
		}

		// create the stringstream for the hud
		std::stringstream ss;

		// check if the game has started yet
		if (game_started) {
			ss << "Lines: " << score;

			bool hit_something = false;

			do {
				// essentially, all "alive" blocks will be moved down by one
				for (int r = 0; r < ROWS; r++) {
					for (int c = 0; c < COLUMNS; c++) {
						temp_grid[r][c] = 0;
					}
				}
				int current_row = block1.getRow();
				int current_column = block1.getColumn();

				// bottom-up; this allows for detection of blocks hitting the bottom first
				for (int r = current_row; r < current_row + 4; r++) {
					for (int c = current_column; c < current_column + 4; c++) {
						int state = block1.getState(r - current_row, c - current_column);
						if (r <= ROWS - 1 && c <= COLUMNS - 1) {
							temp_grid[r][c] = state;
						}
					}
				}

				// gravity using generations
				generation++;

				/*
				Moves the block down by one cell if we're on the correct "generation"
				*/
				if (generation >= gravity) {
					generation = 0;

					// check if current block1 has hit either the bottom of the screen or a 2 cell
					for (int r = current_row; r < current_row + 4; r++) {
						for (int c = 0; c < COLUMNS; c++) {
							if (temp_grid[ROWS - 1][c] == 1) {
								hit_something = true;
								hard_drop = false;
							}
							else if (r < ROWS - 1 && grid[r + 1][c] == 2 && temp_grid[r][c] == 1) {
								hit_something = true;
								hard_drop = false;
							}
						}
					}

					if (hit_something) {
						for (int r = current_row; r < current_row + 4; r++) {
							for (int c = current_column; c < current_column + 4; c++) {
								int state = block1.getState(r - current_row, c - current_column);
								if (r <= ROWS - 1 && c <= COLUMNS - 1 && state == 1) {
									temp_grid[r][c] = 2;
								}
							}
						}
					}
					else {
						block1.update();
					}

				}

				for (int r = 0; r < ROWS; r++) {
					for (int c = 0; c < COLUMNS; c++) {
						if (temp_grid[r][c] > 0) {
							grid[r][c] = std::max(temp_grid[r][c], grid[r][c]);
						}
						else if (temp_grid[r][c] == 0 && grid[r][c] < 2) {
							grid[r][c] = 0;
						}
					}
				}

				if (hit_something) {
					block_type = rollDie(block_type);
					block1 = Block(block_type, 0, 4);
					generation = -1 * gravity;
					bool decreased_gravity = false;

					//check for complete lines
					for (int r = 0; r < ROWS; r++) {
						int num_blocks = 0;
						for (int c = 0; c < COLUMNS; c++) {
							if (grid[r][c] == 2) {
								num_blocks++;
							}
						}
						if (num_blocks == COLUMNS) {
							score++;
							if (!decreased_gravity && gravity > 5) {
								gravity--;
								decreased_gravity = true;
							}
							for (int new_r = r; new_r > 0; new_r--) {
								for (int c = 0; c < COLUMNS; c++) {
									grid[new_r][c] = grid[new_r - 1][c];
								}
							}
						}
					}

					for (int c = 0; c < COLUMNS; c++) {
						if (grid[0][c] == 2) {
							game_started = false;
							game_over = true;
						}
					}
				}
			} while (hard_drop);
		}
		else if (game_over) {
			ss << "GAME OVER";
		}
		else {
			ss << "Start";
		}

		// Clear everything from the last frame
		window.clear(Color(0, 0, 0, 255));

		for (int r = 0; r < ROWS; r++) {
			for (int c = 0; c < COLUMNS; c++) {
				int state = grid[r][c];
				RectangleShape cellShape = makeShape(r, c, pixelScale);
				if (pixelScale > 10 && set_outline == true) {
					cellShape.setOutlineThickness(-1);
					cellShape.setOutlineColor(lines);
				}

				if (state == 1) {
					cellShape.setFillColor(alive);
				}
				else if (state == 2) {
					cellShape.setFillColor(alive_still);
				}
				else {
					cellShape.setFillColor(dead);
				}
				window.draw(cellShape);
			}
		}

		// show held block if it is there
		if (holding_block) {
			for (int r = 0; r < 4; r++) {
				for (int c = 0; c < 4; c++) {
					int state = held_block.getState(r, c);
					if (state == 1) {
						RectangleShape cellShape = makeShape(r + 24, c + 8, pixelScale - 4);
						if (pixelScale > 10 && set_outline == true) {
							cellShape.setOutlineThickness(-1);
							cellShape.setOutlineColor(lines);
						}
						cellShape.setFillColor(alive);
						window.draw(cellShape);
					}					
				}
			}
		}

		hud.setString(ss.str());
		window.draw(hud);

		// Show everything we just drew
		window.display();

	}

	return 0;
}

RectangleShape makeShape(int row, int column, int pixelScale) {
	Vector2f position;
	RectangleShape cellShape;

	position.x = static_cast<float>(column * pixelScale);
	position.y = static_cast<float>(row * pixelScale);

	cellShape.setSize(sf::Vector2f(static_cast<float>(pixelScale), static_cast<float>(pixelScale)));
	cellShape.setPosition(position);

	return cellShape;
}

int surroundingCells(int r, int c, int grid[ROWS][COLUMNS]) {
	int up_one = r - 1;
	int down_one = r + 1;
	int left_one = c - 1;
	int right_one = c + 1;

	// border checks; wrap-around to other side
	if (r == 0) {
		up_one = ROWS - 1;
	}
	else if (r == ROWS - 1) {
		down_one = 0;
	}
	if (c == 0) {
		left_one = COLUMNS - 1;
	}
	else if (c == COLUMNS - 1) {
		right_one = 0;
	}

	int live_neighbors = 0;
	live_neighbors += grid[up_one][left_one];     // top left
	live_neighbors += grid[up_one][c];            // top middle
	live_neighbors += grid[up_one][right_one];    // top right
	live_neighbors += grid[r][left_one];          // middle left
	live_neighbors += grid[r][right_one];         // middle right
	live_neighbors += grid[down_one][left_one];   // bottom left
	live_neighbors += grid[down_one][c];          // bottom middle
	live_neighbors += grid[down_one][right_one];  // bottom right

	return live_neighbors;
}

int rollDie(int last_num) {
	int random_num = rand() % 8;
	if (random_num == 7 || random_num == last_num) {
		random_num = rand() % 7;
	}
	return random_num;
}