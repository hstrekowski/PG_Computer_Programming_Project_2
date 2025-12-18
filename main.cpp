#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>

// --- KONFIGURACJA ---
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int LEVEL_WIDTH = 2000;
const int LEVEL_HEIGHT = 2000;

// --- FUNKCJA POMOCNICZA DO TEKSTU ---
void drawText(SDL_Renderer *renderer, TTF_Font *font, std::string text, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surface)
        return;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect destRect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &destRect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

int main(int argc, char *args[])
{
    // 1. Inicjalizacja systemów
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0)
    {
        std::cerr << "Błąd inicjalizacji!" << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Projekt Podstawy Programowania - SDL2",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Załaduj czcionkę (upewnij się, że plik arial.ttf jest w folderze!)
    TTF_Font *font = TTF_OpenFont("arial.ttf", 18);
    if (!font)
    {
        std::cerr << "Nie znaleziono pliku arial.ttf! Skopiuj go do folderu z programem." << std::endl;
        return 1;
    }

    // --- STAN GRY ---
    SDL_Rect player = {LEVEL_WIDTH / 2, LEVEL_HEIGHT / 2, 40, 40}; // Pozycja w świecie
    SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};

    Uint32 startTime = SDL_GetTicks();
    bool quit = false;
    SDL_Event e;

    // --- GŁÓWNA PĘTLA ---
    while (!quit)
    {
        // 2. Obsługa zdarzeń (Punkt 1a i 1b)
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_QUIT)
                quit = true;
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                    quit = true; // Esc: Wyjście
                if (e.key.keysym.sym == SDLK_n)
                { // 'n': Nowa gra
                    player.x = LEVEL_WIDTH / 2;
                    player.y = LEVEL_HEIGHT / 2;
                    startTime = SDL_GetTicks();
                }
            }
        }

        // 3. Obsługa ruchu (Punkt 3 - WSAD i Strzałki)
        const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
        int speed = 5;
        if (currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_W])
            player.y -= speed;
        if (currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_S])
            player.y += speed;
        if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A])
            player.x -= speed;
        if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D])
            player.x += speed;

        // Granice świata dla gracza
        if (player.x < 0)
            player.x = 0;
        if (player.y < 0)
            player.y = 0;
        if (player.x > LEVEL_WIDTH - player.w)
            player.x = LEVEL_WIDTH - player.w;
        if (player.y > LEVEL_HEIGHT - player.h)
            player.y = LEVEL_HEIGHT - player.h;

        // 4. Aktualizacja Kamery (Punkt 2)
        camera.x = (player.x + player.w / 2) - SCREEN_WIDTH / 2;
        camera.y = (player.y + player.h / 2) - SCREEN_HEIGHT / 2;

        // Blokada kamery na krawędziach mapy
        if (camera.x < 0)
            camera.x = 0;
        if (camera.y < 0)
            camera.y = 0;
        if (camera.x > LEVEL_WIDTH - camera.w)
            camera.x = LEVEL_WIDTH - camera.w;
        if (camera.y > LEVEL_HEIGHT - camera.h)
            camera.y = LEVEL_HEIGHT - camera.h;

        // 5. RENDEROWANIE
        // Tło (Punkt 2)
        SDL_SetRenderDrawColor(renderer, 20, 20, 25, 255);
        SDL_RenderClear(renderer);

        // Podłoga (siatka co 100px, aby widzieć ruch kamery)
        SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
        for (int x = 0; x < LEVEL_WIDTH; x += 100)
        {
            SDL_Rect line = {x - camera.x, 0 - camera.y, 1, LEVEL_HEIGHT};
            SDL_RenderFillRect(renderer, &line);
        }
        for (int y = 0; y < LEVEL_HEIGHT; y += 100)
        {
            SDL_Rect line = {0 - camera.x, y - camera.y, LEVEL_WIDTH, 1};
            SDL_RenderFillRect(renderer, &line);
        }

        // Gracz (Punkt 3)
        SDL_Rect playerScreenPos = {player.x - camera.x, player.y - camera.y, player.w, player.h};
        SDL_SetRenderDrawColor(renderer, 0, 200, 255, 255);
        SDL_RenderFillRect(renderer, &playerScreenPos);

        // 6. UI - Informacje (Punkt 1 i 4)
        SDL_Color white = {255, 255, 255, 255};
        SDL_Color green = {0, 255, 100, 255};

        // Czas
        std::string timeText = "Czas: " + std::to_string((SDL_GetTicks() - startTime) / 1000) + "s";
        drawText(renderer, font, timeText, 20, 20, white);

        // Wymagania
        drawText(renderer, font, "Zrealizowano: Esc(Wyjscie), N(Reset), WSAD/Strzalki(Ruch)", 20, 50, green);
        drawText(renderer, font, "Mechanika: Kamera podąża za graczem, Granice mapy", 20, 75, green);
        drawText(renderer, font, "Pozycja: " + std::to_string(player.x) + " , " + std::to_string(player.y), 20, 100, white);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // Stabilne ~60 FPS
    }

    // Czyszczenie
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}