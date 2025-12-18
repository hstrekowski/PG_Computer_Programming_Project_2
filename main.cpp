#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int GAME_AREA_SIZE = 600;

class Game
{
private:
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    bool isRunning;

    Uint32 startTime;
    char lastAction[64]; // Zamiast std::string używamy tablicy char

public:
    Game() : window(NULL), renderer(NULL), font(NULL), isRunning(true), startTime(0)
    {
        // Inicjalizacja napisu na początku
        strncpy(lastAction, "Brak", 64);
    }

    bool init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
            return false;

        // Inicjalizacja SDL_ttf
        if (TTF_Init() == -1)
        {
            printf("TTF Init Error: %s\n", TTF_GetError());
            return false;
        }

        window = SDL_CreateWindow("Projekt SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                  SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        // Wczytanie czcionki (podaj poprawną nazwę pliku, który masz w folderze!)
        font = TTF_OpenFont("arial.ttf", 20);
        if (!font)
        {
            printf("Błąd wczytywania czcionki: %s\n", TTF_GetError());
        }

        startTime = SDL_GetTicks();
        return true;
    }

    // Pomocnicza funkcja do rysowania tekstu (używa tylko char*)
    void drawText(const char *text, int x, int y)
    {
        if (!font || !text || text[0] == '\0')
            return;

        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface *surface = TTF_RenderText_Solid(font, text, white);
        if (!surface)
            return;

        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dest = {x, y, surface->w, surface->h};

        SDL_RenderCopy(renderer, texture, NULL, &dest);

        // Ważne: SDL w C wymaga ręcznego zwalniania pamięci!
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
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
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    isRunning = false;
                    break;
                case SDLK_n:
                    startTime = SDL_GetTicks();
                    strncpy(lastAction, "Nowa Gra (n)", 64);
                    break;
                case SDLK_UP:
                    strncpy(lastAction, "Strzalka w gore", 64);
                    break;
                case SDLK_DOWN:
                    strncpy(lastAction, "Strzalka w dol", 64);
                    break;
                }
            }
        }
    }

    void render()
    {
        // 1. Tło panelu (szare)
        SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
        SDL_RenderClear(renderer);

        // 2. Pole gry (czarne)
        SDL_Rect gameRect = {0, 0, GAME_AREA_SIZE, GAME_AREA_SIZE};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &gameRect);

        // 3. Wyświetlanie informacji w prawym panelu
        char buffer[128]; // Bufor na sformatowany tekst
        int panelX = GAME_AREA_SIZE + 10;

        // Czas gry
        Uint32 seconds = (SDL_GetTicks() - startTime) / 1000;
        sprintf(buffer, "Czas: %u s", seconds); // Formatowanie int do char[]
        drawText(buffer, panelX, 20);

        // Ostatnia akcja
        drawText("Ostatnia akcja:", panelX, 60);
        drawText(lastAction, panelX, 85);

        SDL_RenderPresent(renderer);
    }

    void cleanup()
    {
        if (font)
            TTF_CloseFont(font);
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