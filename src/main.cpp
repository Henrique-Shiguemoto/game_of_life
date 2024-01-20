#include <iostream>

#define SDL_MAIN_HANDLED
#include "SDL.h"

#include "main.h"

bool g_GameIsRunning = false;
bool g_Change_To_Playing_State = false;
SDL_Window* g_Window = nullptr;
SDL_Renderer* g_Renderer = nullptr;
SDL_Surface* g_Surface = nullptr;
SDL_Texture* g_Texture = nullptr;
state g_State = PREP;

unsigned int g_Framebuffer[WINDOW_DEFAULT_WIDTH * WINDOW_DEFAULT_HEIGHT] = {0};
#define pixel_at(x, y) g_Framebuffer[(WINDOW_DEFAULT_WIDTH * (y)) + (x)]

#define BLACK 0x00000000
#define WHITE 0xFFFFFFFF

int main(){
	if(!init()) return 1;
	create_window();
	
	while(g_GameIsRunning){
		handle_input();
		simulate_world();
		render_graphics();
	}
	quit();
	return 0;
}

bool init(){
	if(SDL_Init(SDL_INIT_VIDEO)){
		std::cout << SDL_GetError() << std::endl;
		return false;
	}
	g_GameIsRunning = true;
	
	return true;
}

void create_window(){
	if((g_Window = SDL_CreateWindow(GAME_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT, SDL_WINDOW_SHOWN))) 
		std::cout << SDL_GetError() << std::endl;
	if((g_Renderer = SDL_CreateRenderer(g_Window, -1, SDL_RENDERER_ACCELERATED)))
		std::cout << SDL_GetError() << std::endl;
	g_Surface = SDL_CreateRGBSurfaceFrom(g_Framebuffer,
                                      WINDOW_DEFAULT_WIDTH,
                                      WINDOW_DEFAULT_HEIGHT,
                                      32,
                                      WINDOW_DEFAULT_WIDTH * 4,
                                      0x000000ff,
                                      0x0000ff00,
                                      0x00ff0000,
                                      0xff000000);
	if(!g_Surface){
		std::cout << SDL_GetError() << std::endl;
	}
	g_Texture = SDL_CreateTexture(g_Renderer, SDL_PIXELFORMAT_ABGR32, SDL_TEXTUREACCESS_STREAMING, WINDOW_DEFAULT_WIDTH, WINDOW_DEFAULT_HEIGHT);
	if(!g_Texture){
		std::cout << SDL_GetError() << std::endl;
	}
}

void handle_input(){
	SDL_Event e;
	while(SDL_PollEvent(&e)){
		const unsigned char* keyboardState = SDL_GetKeyboardState(NULL);

		static bool mouseClicked = false;

		// quitting
		g_GameIsRunning = !keyboardState[SDL_SCANCODE_ESCAPE];

		if(!g_Change_To_Playing_State && keyboardState[SDL_SCANCODE_A]){
			g_Change_To_Playing_State = true;
			g_State = PLAYING;
		}
		
		if(e.type == SDL_QUIT) g_GameIsRunning = false;

		if(g_State == PREP){
			if(e.type == SDL_MOUSEBUTTONDOWN) 	mouseClicked = true;
			if(e.type == SDL_MOUSEBUTTONUP) 	mouseClicked = false;
			if(e.type == SDL_MOUSEMOTION && mouseClicked){
				int x, y;
				SDL_GetMouseState(&x, &y);
				if(pixel_at(x, y) == BLACK) 		pixel_at(x, y) = WHITE;
				else if(pixel_at(x, y) == WHITE) 	pixel_at(x, y) = BLACK;
			}
		}else if(g_State == PLAYING){
			// no mouse input on playing mode (the game plays itself)
		}
	}
}

void simulate_world(){
	unsigned int internal_framebuffer[WINDOW_DEFAULT_WIDTH * WINDOW_DEFAULT_HEIGHT] = {0};
	#define pixel_at_internal(x, y) internal_framebuffer[(WINDOW_DEFAULT_WIDTH * (y)) + (x)]

	if(g_State == PREP){

	}else if(g_State == PLAYING){
		for (int y = 0; y < WINDOW_DEFAULT_HEIGHT; ++y) {
			for (int x = 0; x < WINDOW_DEFAULT_WIDTH; ++x) {
				SDL_Point neighbors[8];
				neighbors[0] = {x - 1, y - 1};
				neighbors[1] = {x, y - 1};
				neighbors[2] = {x + 1, y - 1};
				neighbors[3] = {x - 1, y};
				neighbors[4] = {x + 1, y};
				neighbors[5] = {x - 1, y + 1};
				neighbors[6] = {x, y + 1};
				neighbors[7] = {x + 1, y + 1};

				int live_neighbors_count = 0;

				for (int i = 0; i < 8; ++i) {
					if(check_if_point_is_in_window(neighbors[i].x, neighbors[i].y)){
						if(pixel_at(neighbors[i].x, neighbors[i].y) == WHITE){
							live_neighbors_count++;
						}
					}
				}
				
				if(pixel_at(x, y) == WHITE){
					// alive
					if(live_neighbors_count < 2){
						pixel_at_internal(x, y) = BLACK;
					}else if(live_neighbors_count == 2 || live_neighbors_count == 3){
						pixel_at_internal(x, y) = WHITE;
					}else if(live_neighbors_count > 3){
						pixel_at_internal(x, y) = BLACK;
					}
				}else if(pixel_at(x, y) == BLACK){
					// dead
					if(live_neighbors_count == 3){
						pixel_at_internal(x, y) = WHITE;
					}
				}
			}
		}

		//copy internal buffer to framebuffer
		for (int y = 0; y < WINDOW_DEFAULT_HEIGHT; ++y){
			for (int x = 0; x < WINDOW_DEFAULT_WIDTH; ++x){
				pixel_at(x, y) = pixel_at_internal(x, y);
			}
		}
	}	
}

void render_graphics(){
	SDL_SetRenderDrawColor(g_Renderer, 255, 0, 0, 255);
	SDL_RenderClear(g_Renderer);

	SDL_UpdateTexture(g_Texture, NULL, g_Framebuffer, (int)(sizeof(unsigned int) * WINDOW_DEFAULT_WIDTH));
	SDL_RenderCopy(g_Renderer, g_Texture, NULL, NULL);

	SDL_RenderPresent(g_Renderer);
}

void quit(){
	//Destroy resources here and quit subsystems like audio, fonts and so on...
	SDL_DestroyWindow(g_Window);
	SDL_DestroyRenderer(g_Renderer);
	SDL_Quit();
}

bool check_if_point_is_in_window(int x, int y){
	return !(x < 0 || x > WINDOW_DEFAULT_WIDTH || y < 0 || y > WINDOW_DEFAULT_HEIGHT);
}