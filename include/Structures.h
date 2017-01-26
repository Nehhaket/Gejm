#ifndef GEJM_STRUCTURES_H
#define GEJM_STRUCTURES_H


#include <SDL2/SDL.h>
#include <SDL_render.h>
#include "Tile.h"



//WindowCoords structure
struct WindowCoords {
    int x_left, y_top, x_right, y_bottom;
};

//LastClicked structure
struct LastClicked {
    int id, race;
};

//Difficulty level structure
struct DifficultyLevel {
    int _tiles_in_row, _tiles_in_column;
    int id;
    SDL_Texture* _texture;
};

//Button structure
struct Button {
    int id;
    SDL_Texture* texture;
    WindowCoords window_coords;
};

struct Score {
    std::string name;
    int moves;
    double time;
};

//CustomLessTileId structure
struct {
    bool operator()(Tile a, Tile b)
    {
        return a.id < b.id;
    }
} CustomLessTileId;

struct {
    bool operator()(Score a, Score b)
    {
        return a.time * a.moves < b.time * b.moves;
    }
} CustomLessScoreId;

#endif //GEJM_STRUCTURES_H