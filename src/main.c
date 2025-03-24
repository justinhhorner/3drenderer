#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* color_buffer_texture = NULL;

bool is_running = false;
int window_width = 800;
int window_height = 600;

uint32_t* color_buffer = NULL;

bool initialize_window(void) {
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return false;
    }

    SDL_DisplayID* displays = SDL_GetDisplays(NULL);
    SDL_DisplayMode* display_mode = SDL_GetCurrentDisplayMode(displays[0]);
    if (!display_mode)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }
    window_width = display_mode->w;
    window_height = display_mode->h;

    SDL_PropertiesID window_properties = SDL_CreateProperties();
    if (!window_properties)
    {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    SDL_SetNumberProperty(window_properties, SDL_PROP_WINDOW_CREATE_X_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(window_properties, SDL_PROP_WINDOW_CREATE_Y_NUMBER, SDL_WINDOWPOS_CENTERED);
    SDL_SetNumberProperty(window_properties, SDL_PROP_WINDOW_CREATE_WIDTH_NUMBER, window_width);
    SDL_SetNumberProperty(window_properties, SDL_PROP_WINDOW_CREATE_HEIGHT_NUMBER, window_height);
    window = SDL_CreateWindowWithProperties(window_properties);
    if (!window) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    SDL_SetWindowFullscreen(window, true);

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
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    SDL_Quit();
}

void process_input(void) {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_EVENT_QUIT:
            is_running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (event.key.key == SDLK_ESCAPE)
                is_running = false;
            break;

    }
}

void update(void) {

}

void clear_color_buffer(uint32_t color) {
    for (int i = 0; i < (window_width * window_height); i++) {
        bool is_row = i / window_width % 10 == 0;
        bool is_column = i % 10 == 0;
        if (is_row || is_column) {
            color_buffer[i] = 0xFF000000;
        } else {
            color_buffer[i] = color;
        }
    }
}

void render_color_buffer() {
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        (int)(window_width * sizeof(uint32_t))
    );

    SDL_RenderTexture(
        renderer,
        color_buffer_texture,
        NULL,
        NULL
    );
}

void render(void) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    clear_color_buffer(0xFFFFFF00);
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
