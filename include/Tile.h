#ifndef GEJM_TILE_H
#define GEJM_TILE_H



//Tile class declaration
class Tile
{
public:
  //public fields
    int id;
    int race;
    bool matched;
    int window_x_left;
    int window_x_right;
    int window_y_top;
    int window_y_bottom;
    bool reversed;

  //Constructor
    Tile(int ajdi);
};
//


#endif //GEJM_TILE_H