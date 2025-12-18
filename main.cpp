#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char *argv[])
{
    // 1. Inicjalizacja
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Hubert Strękowski 208381", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool isRunning = true;
    SDL_Event event;

    // 2. Pętla główna
    while (isRunning)
    {
        // Obsługa zdarzeń
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                isRunning = false;
        }

        // Renderowanie
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Czarny kolor tła
        SDL_RenderClear(renderer);

        // Tutaj rysujesz swoje obiekty...

        SDL_RenderPresent(renderer);
    }

    // 3. Sprzątanie
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}