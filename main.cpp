#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char* args[]) {
    // 1. Inicjalizacja SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Błąd SDL: " << SDL_GetError() << std::endl;
        return 1;
    }

    // 2. Tworzenie okna
    SDL_Window* window = SDL_CreateWindow("Moja Gra w SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Pozycja naszego "gracza" (kwadratu)
    SDL_Rect player = {375, 275, 50, 50}; 
    int speed = 5;
    bool quit = false;
    SDL_Event e;

    // 3. Główna pętla gry
    while (!quit) {
        // Obsługa zdarzeń (klawiatura, myszka, zamykanie okna)
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) quit = true;
        }

        // Obsługa klawiszy (płynny ruch)
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_UP])    player.y -= speed;
        if (currentKeyStates[SDL_SCANCODE_DOWN])  player.y += speed;
        if (currentKeyStates[SDL_SCANCODE_LEFT])  player.x -= speed;
        if (currentKeyStates[SDL_SCANCODE_RIGHT]) player.x += speed;

        // 4. Renderowanie (Rysowanie)
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // Kolor tła (ciemnoszary)
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Kolor gracza (zielony)
        SDL_RenderFillRect(renderer, &player);

        SDL_RenderPresent(renderer);

        SDL_Delay(16); // Około 60 FPS
    }

    // 5. Czyszczenie pamięci
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}