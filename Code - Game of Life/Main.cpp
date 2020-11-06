#include <string>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <SFML/Graphics.hpp>

// Avoid having to put sf in front of all the SFML classes and functions
using namespace sf;

// define the constant values of rows/columns of cells
const int ROWS = 50;
const int COLUMNS = ROWS;

// declare functions fleshed out below
RectangleShape makeShape(int row, int column, int pixelScale);
int surroundingCells(int r, int c, int grid[ROWS][COLUMNS]);

// This is where our game starts from
int main()
{
	int windowWidth = 750;
	int windowHeight = windowWidth + 100;
	RenderWindow window(VideoMode(windowWidth, windowHeight), "Cellular Automata");
	window.setFramerateLimit(20);

	int pixelScale = windowWidth / ROWS;
	bool set_outline = true;

	Color dead = Color(0, 0, 0, 255);
	Color alive = Color(255, 255, 255, 255);
	
	// 0 == dead, 1 == alive
	int grid[ROWS][COLUMNS];

	for (int r = 0; r < ROWS; r++) {
		for (int c = 0; c < COLUMNS; c++) {
			grid[r][c] = 0;
		}
	}

	Text hud;
	Font font;
	font.loadFromFile("DS-DIGIT.ttf");
	hud.setFont(font);
	hud.setCharacterSize(50);
	hud.setFillColor(Color::White);
	hud.setPosition(static_cast<float>(window.getSize().x*0.02), static_cast<float>(window.getSize().y - 85));

	//Glider

	grid[1][1] = 1;
	grid[2][2] = 1;
	grid[2][3] = 1;
	grid[3][1] = 1;
	grid[3][2] = 1;

	bool game_started = false;
	int generation = 0;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed) {
				// Someone closed the window- bye
				window.close();
			}

			else if (Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
				// Someone closed the window- bye
				window.close();
			}

			else if (Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
				game_started = true;
			}

			else if (event.type == sf::Event::MouseButtonPressed) {
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					//std::cout << "mouse: (" << event.mouseButton.x << ", " << event.mouseButton.y << ")" << std::endl;
					float click_x = static_cast<float>(event.mouseButton.x);
					float click_y = static_cast<float>(event.mouseButton.y);

					if (click_y > 500.0) {
						game_started = true;
					}
					else {
						float approx_row = click_y / static_cast<float>(pixelScale);
						float approx_column = click_x / static_cast<float>(pixelScale);
						//std::cout << "approx: [" << approx_row << "][" << approx_column << "]" << std::endl;

						int better_row = static_cast<int>(approx_row);
						int better_column = static_cast<int>(approx_column);
						//std::cout << "better: [" << better_row << "][" << better_column << "]" << std::endl;

						grid[better_row][better_column] = std::abs(1 - grid[better_row][better_column]);
					}
					//std::cout << "========================" << std::endl;
				}
			}
		}	

		std::stringstream ss;

		// Clear everything from the last frame
		window.clear(Color(0, 0, 0, 255));

		for (int r = 0; r < ROWS; r++) {
			for (int c = 0; c < COLUMNS; c++) {
				int state = grid[r][c];
				RectangleShape cellShape = makeShape(r, c, pixelScale);
				if (pixelScale > 10 && set_outline == true) {
					cellShape.setOutlineThickness(-1);
					cellShape.setOutlineColor(Color(100, 100, 100, 255));
				}

				if (state == 1) {
					cellShape.setFillColor(alive);
				}
				else {
					cellShape.setFillColor(dead);
				}
				window.draw(cellShape);
			}
		}

		// check if the game has started yet
		if (game_started) {
			ss << "Generation: " << generation;
			generation++;

			/* RULES
			1. Any live cell with two or three live neighbors survives.
			2. Any dead cell with three live neighbors becomes a live cell.
			3. All other live cells die in the next generation. Similarly, all other dead cells stay dead.
			*/
			int temp_grid[ROWS][COLUMNS];
			for (int r = 0; r < ROWS; r++) {
				for (int c = 0; c < COLUMNS; c++) {
					int state = grid[r][c];
					temp_grid[r][c] = state;
					int live_neighbors = surroundingCells(r, c, grid);
					if (state == 0 && live_neighbors == 3) {
						temp_grid[r][c] = 1;
					}
					else if (state == 1 && (live_neighbors < 2 || live_neighbors > 3)) {
						temp_grid[r][c] = 0;
					}
				}
			}

			for (int r = 0; r < ROWS; r++) {
				for (int c = 0; c < COLUMNS; c++) {
					grid[r][c] = temp_grid[r][c];
				}
			}
		}
		else {
			ss << "Start";
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