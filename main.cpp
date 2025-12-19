#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

// --- USTAWIENIA OKNA I ŚWIATA ---
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int VIEWPORT_SIZE = 600;

const int WORLD_WIDTH = 2400;
const int WORLD_HEIGHT = 600;
const int FLOOR_TOP = 350;

struct Player
{
    float x, y;
    int w, h;
    float speed;
};

class Game
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    TTF_Font *fontSmall; // Dodatkowa mniejsza czcionka do listy wymagan
    bool isRunning;

    Player player;
    SDL_Rect camera;
    Uint32 totalStartTime; // Czas od uruchomienia programu
    Uint32 stageStartTime; // Czas od rozpoczecia etapu (resetowany 'n')
    char lastAction[64];

public:
    Game() : window(NULL), renderer(NULL), font(NULL), fontSmall(NULL), isRunning(true)
    {
        player.w = 50;
        player.h = 90;
    }

    bool init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            return false;
        if (TTF_Init() == -1)
            return false;

        window = SDL_CreateWindow("Beat 'em Up - Projekt Podstawy Programowania",
                                  SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (!window)
            return false;

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        if (!renderer)
            return false;

        font = TTF_OpenFont("arial.ttf", 18);
        fontSmall = TTF_OpenFont("arial.ttf", 14); // Mniejsza czcionka do listy

        totalStartTime = SDL_GetTicks(); // Ten czas mierzymy tylko raz
        resetGame();
        return true;
    }

    void resetGame()
    {
        player.x = 100;
        player.y = FLOOR_TOP + 50;
        player.speed = 4.0f;

        camera.x = 0;
        camera.y = 0;
        camera.w = VIEWPORT_SIZE;
        camera.h = VIEWPORT_SIZE;

        stageStartTime = SDL_GetTicks(); // Resetujemy czas etapu
        strncpy(lastAction, "Nowa Gra Rozpoczeta", 64);
    }

    void handleEvents()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                isRunning = false;
            if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                    isRunning = false;
                if (event.key.keysym.sym == SDLK_n)
                    resetGame();
            }
        }

        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        bool moved = false;

        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
        {
            player.x -= player.speed;
            strncpy(lastAction, "Idzie w lewo", 64);
            moved = true;
        }
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
        {
            player.x += player.speed;
            strncpy(lastAction, "Idzie w prawo", 64);
            moved = true;
        }
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
        {
            player.y -= player.speed * 0.6f;
            strncpy(lastAction, "Idzie w gore", 64);
            moved = true;
        }
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
        {
            player.y += player.speed * 0.6f;
            strncpy(lastAction, "Idzie w dol", 64);
            moved = true;
        }

        const int ROAD_MARGIN = 20;
        if (player.y + player.h < (float)(FLOOR_TOP + ROAD_MARGIN))
            player.y = (float)(FLOOR_TOP + ROAD_MARGIN) - player.h;
        if (player.y + player.h > (float)WORLD_HEIGHT)
            player.y = (float)WORLD_HEIGHT - player.h;

        if (!moved && strcmp(lastAction, "Nowa Gra Rozpoczeta") != 0)
            strncpy(lastAction, "Oczekiwanie", 64);

        if (player.x < 0)
            player.x = 0;
        if (player.x > WORLD_WIDTH - player.w)
            player.x = WORLD_WIDTH - player.w;

        camera.x = (int)(player.x + player.w / 2) - VIEWPORT_SIZE / 2;
        if (camera.x < 0)
            camera.x = 0;
        if (camera.x > WORLD_WIDTH - VIEWPORT_SIZE)
            camera.x = WORLD_WIDTH - VIEWPORT_SIZE;
    }

    // Dodalem parametr fontu do drawText
    void drawText(const char *text, int x, int y, TTF_Font *f = NULL)
    {
        TTF_Font *usedFont = (f == NULL) ? font : f;
        if (!usedFont || !text)
            return;
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface *surf = TTF_RenderText_Solid(usedFont, text, white);
        if (!surf)
            return;
        SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_Rect dst = {x, y, surf->w, surf->h};
        SDL_RenderCopy(renderer, tex, NULL, &dst);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex);
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);

        SDL_Rect viewport = {0, 0, VIEWPORT_SIZE, VIEWPORT_SIZE};
        SDL_RenderSetViewport(renderer, &viewport);

        // --- ŚWIAT GRY ---
        SDL_SetRenderDrawColor(renderer, 20, 20, 50, 255);
        SDL_Rect sky = {0, 0, VIEWPORT_SIZE, FLOOR_TOP};
        SDL_RenderFillRect(renderer, &sky);

        SDL_SetRenderDrawColor(renderer, 70, 70, 75, 255);
        SDL_Rect ground = {-camera.x, FLOOR_TOP, WORLD_WIDTH, WORLD_HEIGHT - FLOOR_TOP};
        SDL_RenderFillRect(renderer, &ground);

        SDL_SetRenderDrawColor(renderer, 120, 120, 0, 255);
        for (int i = 0; i < WORLD_WIDTH; i += 300)
        {
            SDL_Rect line = {i - camera.x, FLOOR_TOP + 100, 80, 10};
            SDL_RenderFillRect(renderer, &line);
        }

        SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        SDL_Rect playerRect = {(int)player.x - camera.x, (int)player.y, player.w, player.h};
        SDL_RenderFillRect(renderer, &playerRect);

        // --- PANEL BOCZNY ---
        SDL_RenderSetViewport(renderer, NULL);
        int px = VIEWPORT_SIZE + 15;
        char buf[128];

        drawText("--- CZAS ---", px, 15);
        sprintf(buf, "Od startu: %u s", (SDL_GetTicks() - totalStartTime) / 1000);
        drawText(buf, px, 40);
        sprintf(buf, "Etap: %u s", (SDL_GetTicks() - stageStartTime) / 1000);
        drawText(buf, px, 65);

        drawText("--- STATUS ---", px, 105);
        drawText(lastAction, px, 130);
        sprintf(buf, "X: %.0f | Y: %.0f", player.x, player.y);
        drawText(buf, px, 155);

        // --- LISTA WYMAGAN ---
        drawText("--- WYMAGANIA ---", px, 200);
        drawText("- Podzial okna (viewport)", px, 225, fontSmall);
        drawText("- Kamera (scrolling)", px, 245, fontSmall);
        drawText("- Brak STL (string/vector)", px, 265, fontSmall);
        drawText("- Ruch WSAD + Strzalki", px, 285, fontSmall);
        drawText("- Czas rzeczywisty", px, 305, fontSmall);
        drawText("- Obsluga Esc i 'n'", px, 325, fontSmall);
        drawText("- Brak ograniczenia blokami", px, 345, fontSmall);

        drawText("--- STEROWANIE ---", px, 480);
        drawText("WSAD / Strzalki - Ruch", px, 505, fontSmall);
        drawText("N - Nowa Gra", px, 525, fontSmall);
        drawText("Esc - Wyjscie", px, 545, fontSmall);

        SDL_RenderPresent(renderer);
    }

    void cleanup()
    {
        if (font)
            TTF_CloseFont(font);
        if (fontSmall)
            TTF_CloseFont(fontSmall);
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    bool running() { return isRunning; }
};

int main(int argc, char *argv[])
{
    Game game;
    if (!game.init())
        return 1;
    while (game.running())
    {
        game.handleEvents();
        game.render();
        SDL_Delay(16);
    }
    game.cleanup();
    return 0;
}