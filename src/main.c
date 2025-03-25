#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;

bool is_running = false;
int window_width = 800;
int window_height = 600;

uint32_t* color_buffer = NULL;

bool initialize_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    window_width = display_mode.w;
    window_height = display_mode.h;

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        0
    );
    if (!window) {
        fprintf(stderr, "Error creating window");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error creating renderer");
        return false;
    }

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    return true;
}

void setup(void) {
    color_buffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
    if (color_buffer == NULL) {
        fprintf(stderr, "Unable to allocate color buffer");
        is_running = false;
    }

    color_buffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );
    if (!color_buffer_texture) {
        fprintf(stderr, "Error creating SDL texture");
        is_running = false;
    }
}

void destroy_window(void) {
    free(color_buffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(color_buffer_texture);
    SDL_Quit();
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
                is_running = false;
            break;

    }
}

void update(void) {

}

void clear_color_buffer(uint32_t color) {
    for (int i = 0; i < (window_width * window_height); i++) {
        color_buffer[i] = color;
    }
}

void render_color_buffer() {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t))
    );

    SDL_RenderCopy(
        renderer,
        color_buffer_texture,
        NULL,
        NULL
    );
}

void draw_grid(int gutter, int color) {
    for (int y = gutter; y < window_height; y += gutter) {
        for (int x = gutter; x < window_width; x += gutter) {
            color_buffer[window_width * y + x] = color;
        }
    }
}

void draw_rect(int x, int y, int w, int h, int color) {
    for (int i = y; i < y + h; i++) {
        for (int j = x; j < x + w; j++) {
            color_buffer[window_width * i + j] = color;
        }
    }
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    clear_color_buffer(0xFF000000);
    draw_grid(10, 0xFF333333);
    draw_rect(300, 200, 300, 200, 0xFFFF00FF);
    render_color_buffer();

    SDL_RenderPresent(renderer);
}

int main(void) {
    is_running = initialize_window();

    setup();

    while (is_running) {
        process_input();
        update();
        render();
    }
    
    return 0;
}
