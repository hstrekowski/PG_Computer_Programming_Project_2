#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "player.h"

class Game
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    TTF_Font *fontSmall;
    bool isRunning;

    Player player;
    SDL_Rect camera;
    Uint32 totalStartTime;
    Uint32 stageStartTime;
    char lastAction[64];

    void updatePlayer();
    void updateCamera();
    void drawWorld();
    void drawSidebar();
    // Nowa metoda do centrowania:
    void drawCenteredText(const char *text, int y, TTF_Font *f = NULL);

public:
    Game();
    bool init();
    void resetGame();
    void handleEvents();
    void render();
    void cleanup();
    bool running();
};

#endif