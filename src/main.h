#ifndef MAIN_H
#define MAIN_H

#define GAME_NAME 				"Game of Life with SDL2"
#define WINDOW_DEFAULT_WIDTH  	800
#define WINDOW_DEFAULT_HEIGHT 	600

typedef enum state{
	PREP = 0,
	PLAYING = 1
} state;

bool init();
void create_window();
void handle_input();
void simulate_world();
void render_graphics();
void quit();
bool check_if_point_is_in_window(int x, int y);

#endif
