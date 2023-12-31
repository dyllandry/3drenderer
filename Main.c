#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

bool is_running = false;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

int window_width = 800;
int window_height = 600;

bool initialize_window(void) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// Use SDL to query what is the fullscreen maximum width and height
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width = display_mode.w;
	window_height = display_mode.h;

	// Create a SDL Window
	window = SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		window_width,
		window_height,
		SDL_WINDOW_BORDERLESS
	);
	if (!window) {
		fprintf(stderr, "Error creating SDL window.\n");
		return false;
	}

	// Create a SDL renderer
	renderer = SDL_CreateRenderer(window, -1, 0);
	if (!renderer) {
		fprintf(stderr, "Error creating SDL renderer.\n");
		return false;
	}

	SDL_SetWindowFullscreen(window, SDL_SetWindowFullscreen);

	return true;
}

void setup(void) {
	// Allocate the required memory in bytes to hold the color buffer
	color_buffer = malloc(sizeof(uint32_t) * window_width * window_height);

	// Create a SDL texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		window_width,
		window_height
	);
}

void process_input(void) {
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type) {
	case SDL_QUIT:
		is_running = false;
		break;
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			is_running = false;
		}
		break;
	}
}

void update(void) {

} 

void draw_grid(void) {
	uint32_t grey = 0xFF888888;
	uint32_t black = 0xFF000000;
	for (int y = 0; y < window_height; y++) {
		for (int x = 0; x < window_width; x++) {
			uint32_t pixel_color = black;
			bool row_is_tenth = (y + 1) % 10 == 0;
			bool col_is_tenth = (x + 1) % 10 == 0;
			if (row_is_tenth && col_is_tenth) {
				pixel_color = grey;
			}
			color_buffer[(y * window_width) + x] = pixel_color;
		}
	}
}

void draw_rect(int rect_x, int rect_y, int rect_width, int rect_height, uint32_t color) {
	for (int row = 0; row < rect_height; row++) {
		for (int col = 0; col < rect_width; col++) {
			int pixel_y = row + rect_y;
			int pixel_x = col + rect_x;
			int max_window_y = window_height - 1;
			int max_window_x = window_width - 1;
			bool row_offscreen = pixel_y > max_window_y || pixel_y < 0;
			bool col_offscreen = pixel_x > max_window_x || pixel_y < 0;
			if (!row_offscreen && !col_offscreen) {
				color_buffer[((row + rect_y) * window_width) + col + rect_x] = color;
			}
		}
	}
}

void render_color_buffer(void) {
	SDL_UpdateTexture(
		color_buffer_texture,
		NULL,
		color_buffer,
		(int)(window_width * sizeof(uint32_t))
	);
	SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void clear_color_buffer(uint32_t color) {
	for (int y = 0; y < window_height; y++) {
		for (int x = 0; x < window_width; x++) {
			color_buffer[(window_width * y) + x] = color;
		}
	}
}

void render(void) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 1);
	SDL_RenderClear(renderer);

	draw_grid();

	// Draw a house
	int house_offset_x = 500;
	int house_offset_y = 500;
	int house_width = 500;
	int house_height = 300;
	// House body
	draw_rect(house_offset_x, house_offset_y, house_width, house_height, 0xFF9e774a);
	// House door
	draw_rect(house_offset_x + (house_width / 2) - 50, house_offset_y + (house_height / 3), 100, (house_height / 3) * 2, 0xFFd6984f);
	// House windows
	draw_rect(house_offset_x + 50, house_offset_y + (house_height / 3), 100, 100, 0xFF6fe3df);
	draw_rect(house_offset_x + house_width - 150, house_offset_y + (house_height / 3), 100, 100, 0xFF6fe3df);
	// House chimney
	draw_rect(house_offset_x + (house_width / 4), house_offset_y - (house_height / 3), 50, house_height / 3, 0xFF8a3838);


	
	render_color_buffer();
	clear_color_buffer(0xFFFFFF00);

	SDL_RenderPresent(renderer);
}

void destroy_window(void) {
	// Destroys resources in reverse order that they were allocated.
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char* args[]) {

	is_running = initialize_window();

	setup();

	while (is_running) {
		process_input();
		update();
		render();
	}

	destroy_window();

	return 0;
}