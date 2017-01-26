#ifndef GEJM_ENGINE_H
#define GEJM_ENGINE_H


#include <iostream>
#include <utility>
#include <vector>
#include <chrono>
#include <thread>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "Config.h"
#include "Tile.h"
#include "Structures.h"
#include "Timer.h"



//Cleanup method
template<typename T, typename... Args> void cleanup(T *t, Args&&... args) {
    cleanup(t);
    cleanup(std::forward<Args>(args)...);
}
template<> inline void cleanup<SDL_Window>(SDL_Window *win) {
    if (!win){
        return;
    }
    SDL_DestroyWindow(win);
}
template<> inline void cleanup<SDL_Renderer>(SDL_Renderer *ren) {
    if (!ren){
        return;
    }
    SDL_DestroyRenderer(ren);
}
template<> inline void cleanup<SDL_Texture>(SDL_Texture *tex) {
    if (!tex){
        return;
    }
    SDL_DestroyTexture(tex);
}
template<> inline void cleanup<SDL_Surface>(SDL_Surface *surf) {
    if (!surf){
        return;
    }
    SDL_FreeSurface(surf);
}
//


//Engine class declaration
class Engine
{
private:
  //Private fields
    const std::string _config_path = "../config.ini";
    bool _in_menu;
    bool _in_settings;
    bool _close;
    int _space_x;
    int _space_y;
    int _moves_count;
    LastClicked _last_clicked;
    Config _config;
    Timer _timer;
    Button _end_game_button;
    SDL_Event _event;
    SDL_Window* _window;
    SDL_Renderer* _renderer;
    SDL_Texture* _background_texture;
    SDL_Texture* _tile_reverse_texture;
    TTF_Font* _Font;
    SDL_Color _White;
    SDL_Surface* _score_surface;
    SDL_Texture* _score_texture;
    std::vector<SDL_Texture*> _tile_front_textures;
    std::vector<Tile> _tiles;
    std::vector<Button> _menu_buttons;
    Button _settings_button;
    SDL_Texture* _darker;

  //SDL-specific methods
    void logSDLError(std::ostream &os, const std::string &msg);
    SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren);
    void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h);
    void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y);

  //Private methods
    WindowCoords getTileCoords(Tile &tile);
    int checkIfTileClicked();
    int checkIfMenuButtonClicked();
    int checkIfSettingsButtonClicked();
    bool checkIfEndScreenButtonClicked();
    bool mainGame();
    bool mainMenu();
    bool mainSettings();
    bool mainEndScreen();
    bool checkIfDone();
    void createTiles();
    void createButtons();
    void renderClear();
    void renderBackground();
    void renderTile(Tile &tile);
    void renderButton(Button &button);
    void renderPresent();
    void renderGame();
    void renderMenu();
    void renderSettings();
    void renderEndScreen();
    void renderAll();
    void resetGame();
    void resizeTile(Tile &tile);
    void updateButtons();
    void updateTiles();
    void updateSize();
    void setDifficultyLevels();
    void setDifficulty();

public:
  //Public methods
    Engine();
    void game();

  //Cleanup
    ~Engine();
};
//


#endif //GEJM_ENGINE_H
