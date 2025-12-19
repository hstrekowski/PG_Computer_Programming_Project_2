#include "game.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>

Game::Game() : window(NULL), renderer(NULL), font(NULL), fontSmall(NULL), isRunning(true)
{
    player.w = PLAYER_WIDTH;
    player.h = PLAYER_HEIGHT;
}

bool Game::init()
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

    font = TTF_OpenFont("arial.ttf", FONT_SIZE_NORMAL);
    fontSmall = TTF_OpenFont("arial.ttf", FONT_SIZE_SMALL);

    if (!font || !fontSmall)
    {
        printf("Blad: Nie mozna wczytac czcionki arial.ttf!\n");
    }

    totalStartTime = SDL_GetTicks();
    resetGame();
    return true;
}

void Game::resetGame()
{
    player.x = PLAYER_START_X;
    player.y = FLOOR_TOP + PLAYER_START_Y_OFFSET;
    player.speed = PLAYER_BASE_SPEED;

    camera.x = 0;
    camera.y = 0;
    camera.w = VIEWPORT_SIZE;
    camera.h = VIEWPORT_SIZE;

    stageStartTime = SDL_GetTicks();
    strncpy(lastAction, "Nowa Gra Rozpoczeta", 64);
}

void Game::handleEvents()
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
    updatePlayer();
    updateCamera();
}

void Game::updatePlayer()
{
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    bool moved = false;

    // Ruch lewo / prawo (Strzalki + AD)
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

    // Ruch gora / dol (Strzalki + WS)
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
    {
        player.y -= player.speed * PLAYER_Y_SPEED_MULT;
        strncpy(lastAction, "Idzie w gore", 64);
        moved = true;
    }
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
    {
        player.y += player.speed * PLAYER_Y_SPEED_MULT;
        strncpy(lastAction, "Idzie w dol", 64);
        moved = true;
    }

    // Ograniczenie pozycji (Clamping do krawedzi drogi z marginesem)
    if (player.y + player.h < (float)(FLOOR_TOP + ROAD_MARGIN))
        player.y = (float)(FLOOR_TOP + ROAD_MARGIN) - player.h;
    if (player.y + player.h > (float)WORLD_HEIGHT)
        player.y = (float)WORLD_HEIGHT - player.h;

    // Granice poziome swiata
    if (player.x < 0)
        player.x = 0;
    if (player.x > WORLD_WIDTH - player.w)
        player.x = WORLD_WIDTH - player.w;

    if (!moved && strcmp(lastAction, "Nowa Gra Rozpoczeta") != 0)
        strncpy(lastAction, "Oczekiwanie", 64);
}

void Game::updateCamera()
{
    camera.x = (int)(player.x + player.w / 2) - VIEWPORT_SIZE / 2;
    if (camera.x < 0)
        camera.x = 0;
    if (camera.x > WORLD_WIDTH - VIEWPORT_SIZE)
        camera.x = WORLD_WIDTH - VIEWPORT_SIZE;
}

void Game::drawCenteredText(const char *text, int y, TTF_Font *f)
{
    TTF_Font *usedFont = (f == NULL) ? font : f;
    if (!usedFont || !text || text[0] == '\0')
        return;

    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *surf = TTF_RenderText_Solid(usedFont, text, white);
    if (!surf)
        return;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);

    // Obliczanie wycentrowanej pozycji X w panelu bocznym
    int x = VIEWPORT_SIZE + (SIDEBAR_WIDTH / 2) - (surf->w / 2);

    SDL_Rect dst = {x, y, surf->w, surf->h};
    SDL_RenderCopy(renderer, tex, NULL, &dst);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}

void Game::render()
{
    // Czyszczenie ekranu (kolor tła panelu bocznego)
    SDL_SetRenderDrawColor(renderer, COLOR_PANEL_BG, 255);
    SDL_RenderClear(renderer);

    drawWorld();
    drawSidebar();

    SDL_RenderPresent(renderer);
}

void Game::drawWorld()
{
    SDL_Rect viewport = {0, 0, VIEWPORT_SIZE, VIEWPORT_SIZE};
    SDL_RenderSetViewport(renderer, &viewport);

    // Rysowanie tła (Niebo)
    SDL_SetRenderDrawColor(renderer, COLOR_SKY, 255);
    SDL_Rect sky = {0, 0, VIEWPORT_SIZE, FLOOR_TOP};
    SDL_RenderFillRect(renderer, &sky);

    // Rysowanie drogi
    SDL_SetRenderDrawColor(renderer, COLOR_GROUND, 255);
    SDL_Rect ground = {-camera.x, FLOOR_TOP, WORLD_WIDTH, WORLD_HEIGHT - FLOOR_TOP};
    SDL_RenderFillRect(renderer, &ground);

    // Rysowanie pasów na drodze
    SDL_SetRenderDrawColor(renderer, COLOR_ROAD_LINES, 255);
    for (int i = 0; i < WORLD_WIDTH; i += 300)
    {
        SDL_Rect line = {i - camera.x, FLOOR_TOP + 100, 80, 10};
        SDL_RenderFillRect(renderer, &line);
    }

    // Rysowanie gracza
    SDL_SetRenderDrawColor(renderer, COLOR_PLAYER, 255);
    SDL_Rect playerRect = {(int)player.x - camera.x, (int)player.y, player.w, player.h};
    SDL_RenderFillRect(renderer, &playerRect);
}

void Game::drawSidebar()
{
    // Reset viewportu do calego okna
    SDL_RenderSetViewport(renderer, NULL);
    char buf[128];

    // Sekcja CZAS
    drawCenteredText("--- CZAS ---", PY_TIME_HDR);
    sprintf(buf, "Od startu: %u s", (SDL_GetTicks() - totalStartTime) / 1000);
    drawCenteredText(buf, PY_TIME_TOTAL, fontSmall);
    sprintf(buf, "Etap: %u s", (SDL_GetTicks() - stageStartTime) / 1000);
    drawCenteredText(buf, PY_TIME_STAGE, fontSmall);

    // Sekcja STATUS
    drawCenteredText("--- STATUS ---", PY_STATUS_HDR);
    drawCenteredText(lastAction, PY_STATUS_ACT, fontSmall);
    sprintf(buf, "X: %.0f | Y: %.0f", player.x, player.y);
    drawCenteredText(buf, PY_STATUS_POS, fontSmall);

    // Sekcja WYMAGANIA
    drawCenteredText("--- WYMAGANIA ---", PY_REQ_HDR);
    const char *reqs[] = {
        "Podzial okna",
        "Kamera (scroll)",
        "Brak STL",
        "Ruch WSAD/Strzalki",
        "Czas rzecz.",
        "Obsluga Esc/n",
        "Brak blokow"};
    for (int i = 0; i < 7; i++)
    {
        drawCenteredText(reqs[i], PY_REQ_START + (i * PY_REQ_STEP), fontSmall);
    }

    // Sekcja STEROWANIE
    drawCenteredText("--- STEROWANIE ---", PY_CTRL_HDR);
    drawCenteredText("Ruch: WSAD / Strzalki", PY_CTRL_START, fontSmall);
    drawCenteredText("N: Nowa Gra", PY_CTRL_START + 20, fontSmall);
    drawCenteredText("Esc: Wyjscie", PY_CTRL_START + 40, fontSmall);
}

void Game::cleanup()
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

bool Game::running() { return isRunning; }