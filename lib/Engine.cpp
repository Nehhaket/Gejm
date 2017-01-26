#include "../include/Engine.h"


//Engine class implementation

//Constructor
Engine::Engine() : _config(*new Config(_config_path)), _timer(*new Timer){
  //SDL_IMG initialization and error handling

    int flags = IMG_INIT_JPG|IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags)
    {
        logSDLError(std::cout, "SDL_Init");
        SDL_Quit();
    }

  //SDL_TTF initialization and error handling
    /*if (TTF_Init() == -1)
    {
        std::cout << "Init TTF  failt : " << SDL_GetError() << std::endl;
        logSDLError(std::cout, "SDL_Init");
        SDL_Quit();
    }*/

  //Window creation
    _window = SDL_CreateWindow("Memory", 100, 100, _config.screen_width, _config.screen_height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    //Error handling
    if (_window == NULL)
    {
        logSDLError(std::cout, "CreateWindow");
        SDL_Quit();
    }
    SDL_SetWindowMinimumSize(_window, 480, 480);

  //Render creation
    _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    //Error handling
    if (_renderer == NULL)
    {
        logSDLError(std::cout, "CreateRenderer");
        SDL_DestroyWindow(_window);
        SDL_Quit();
    }

  //Background texture loading
    _background_texture = loadTexture(_config.background_path, _renderer);
    //Error handling
    if (_background_texture == NULL)
    {
        cleanup(_background_texture, _renderer, _window);
        IMG_Quit();
        SDL_Quit();
    }

  //Reverse texture loading
    _tile_reverse_texture = loadTexture(_config.tile_reverse_path, _renderer);
    //Error handling
    if (_tile_reverse_texture == NULL)
    {
        cleanup(_tile_reverse_texture, _renderer, _window);
        IMG_Quit();
        SDL_Quit();
    }

    _darker = loadTexture(_config.darkerer_path, _renderer);
    if (_tile_reverse_texture == NULL)
    {
        cleanup(_darker, _renderer, _window);
        IMG_Quit();
        SDL_Quit();
    }

  //Front textures loading
    for (int i = 0; i < _config.tile_front_paths.size(); i++)
    {
        SDL_Texture *texture = loadTexture(_config.tile_front_paths[i], _renderer);
        _tile_front_textures.push_back(texture);
      //Error handling
        if (_tile_front_textures[i] == NULL)
        {
            cleanup(_tile_front_textures[i], _renderer, _window);
            IMG_Quit();
            SDL_Quit();
        }
    }

    /*
     * std::string temp = SDL_GetBasePath();
     * temp += "UbuntuMono-R.ttf";
     * _Font = TTF_OpenFont(temp.c_str(), 20);
     * if(_Font == NULL)
     * {
     *     std::cout << TTF_GetError();
     * }
     * _White = {255, 255, 255};
     */

  //Misc
    _event.type = 0;
    _close = false;

    setDifficultyLevels();
    _config.difficulty.id = std::stoi(_config._properties.get<std::string>("difficulty_level"));
    setDifficulty();

  //Menu buttons creation
    createButtons();
  //

  //Initial tiles creation
    createTiles();
  //

    updateSize();

    _last_clicked.id = -1;
    _last_clicked.race = -1;
    _in_menu = false;
    _in_settings = false;
    _moves_count = 0;
}
void Engine::setDifficultyLevels() {
    DifficultyLevel _temp_difficulty_level;

    // 0: 3x2
    _temp_difficulty_level.id = 0;
    _temp_difficulty_level._tiles_in_row = 3;
    _temp_difficulty_level._tiles_in_column = 2;
    _temp_difficulty_level._texture = loadTexture(_config.easy, _renderer);
    _config.difficulty_levels.push_back(_temp_difficulty_level);

    // 1: 4x3
    _temp_difficulty_level.id = 1;
    _temp_difficulty_level._tiles_in_row = 4;
    _temp_difficulty_level._tiles_in_column = 3;
    _temp_difficulty_level._texture = loadTexture(_config.normal, _renderer);
    _config.difficulty_levels.push_back(_temp_difficulty_level);

    // 2: 4x4
    _temp_difficulty_level.id = 2;
    _temp_difficulty_level._tiles_in_row = 4;
    _temp_difficulty_level._tiles_in_column = 4;
    _temp_difficulty_level._texture = loadTexture(_config.intermediate, _renderer);
    _config.difficulty_levels.push_back(_temp_difficulty_level);

    // 3: 5x4
    _temp_difficulty_level.id = 3;
    _temp_difficulty_level._tiles_in_row = 5;
    _temp_difficulty_level._tiles_in_column = 4;
    _temp_difficulty_level._texture = loadTexture(_config.advanced, _renderer);
    _config.difficulty_levels.push_back(_temp_difficulty_level);

    // 4: 6x5
    _temp_difficulty_level.id = 4;
    _temp_difficulty_level._tiles_in_row = 6;
    _temp_difficulty_level._tiles_in_column = 5;
    _temp_difficulty_level._texture = loadTexture(_config.hard, _renderer);
    _config.difficulty_levels.push_back(_temp_difficulty_level);
}
void Engine::setDifficulty() {
    _config.difficulty._tiles_in_row = _config.difficulty_levels[_config.difficulty.id]._tiles_in_row;
    _config.difficulty._tiles_in_column = _config.difficulty_levels[_config.difficulty.id]._tiles_in_column;
    _config.difficulty._texture = _config.difficulty_levels[_config.difficulty.id]._texture;
}

//SDL specific methods
void Engine::logSDLError(std::ostream &os, const std::string &msg) {
    os << msg << " error: " << SDL_GetError() << std::endl;
}
SDL_Texture* Engine::loadTexture(const std::string &file, SDL_Renderer *ren) {
    SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
    if (texture == NULL)
    {
        logSDLError(std::cout, "CreateTextureFromSurface");
    }
    return texture;
}
void Engine::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h) {
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;
    SDL_RenderCopy(ren, tex, NULL, &dst);
}
void Engine::renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y) {
    int w, h;
    SDL_QueryTexture(tex, NULL, NULL, &w, &h);
    renderTexture(tex, ren, x, y, w, h);
}

//Event handling and main logic
bool Engine::mainGame() {
    if(_close)
    {
        return false;
    }
    renderAll();
    while(SDL_PollEvent(&_event))
    {
        switch(_event.type)
        {
            case SDL_QUIT: {
                _close = true;
            } break;

            case SDL_KEYDOWN: {
                if(_event.key.keysym.sym == SDLK_ESCAPE)
                {
                    _timer.stop();
                    _in_menu = true;
                    updateSize();
                    while(mainMenu()){}
                    _timer.start();
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                int temp_id = checkIfTileClicked();
                if (temp_id+1)
                {
                    if (_last_clicked.id+1)
                    {
                        if(_last_clicked.id != _tiles[temp_id].id)
                        {
                            _moves_count++;
                            _tiles[temp_id].reversed = true;
                            renderAll();
                            std::this_thread::sleep_for(std::chrono::milliseconds(900));
                            if (_last_clicked.race == _tiles[temp_id].race)
                            {
                                for (Tile &tajl : _tiles)
                                {
                                    if (tajl.race == _last_clicked.race)
                                    {
                                        tajl.matched = true;
                                    }
                                }
                            }
                            for (Tile &tajl : _tiles)
                            {
                                tajl.reversed = false;
                            }
                            _last_clicked.id = -1;
                            _last_clicked.race = -1;
                        }
                    }
                    else
                    {
                        _moves_count++;
                        _tiles[temp_id].reversed = true;
                        _last_clicked.id = _tiles[temp_id].id;
                        _last_clicked.race = _tiles[temp_id].race;
                    }
                }
            } break;

            case SDL_WINDOWEVENT: {
                if (_event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    _config.screen_width = _event.window.data1;
                    _config.screen_height = _event.window.data2;
                    updateSize();
                }
            } break;

            default:
                break;
        }
    }
    if(checkIfDone())
    {
        _timer.stop();
        updateSize();
        Score score;
        score.name = "";
        score.time = _timer.getTime();
        score.moves = _moves_count;
        /*
         * std::string temp = std::to_string(_moves_count);
         * _score_surface = TTF_RenderText_Solid(_Font, temp.c_str(), _White);
         * _score_texture = SDL_CreateTextureFromSurface(_renderer, _score_surface);
         * SDL_FreeSurface(_score_surface);
         */
        while(mainEndScreen());
        _config.addScore(score);
    }
    return true;
}
bool Engine::mainMenu() {
    if(_close) {return false;}
    renderAll();
    while(SDL_PollEvent(&_event))
    {
        switch(_event.type)
        {
            case SDL_QUIT: {
                _in_menu = false;
                _close = true;
            } break;

            /*case SDL_KEYDOWN: {
                if (_event.key.keysym.sym == SDLK_ESCAPE)
                {
                    _in_menu = false;
                }
            } break;*/

            case SDL_MOUSEBUTTONDOWN: {
                int temp_id = checkIfMenuButtonClicked();
                if(temp_id+1)
                {
                    switch(temp_id)
                    {
                        case 0: {
                            _in_menu = false;
                        } break;

                        case 1: {
                            resetGame();
                            _in_menu = false;
                        } break;

                        case 2: {
                            _in_settings = true;
                            updateSize();
                            while(mainSettings()){}
                        } break;

                        case 3: {
                            _in_menu = false;
                            _close = true;
                        } break;

                        default:
                            break;
                    }
                }
            } break;

            case SDL_WINDOWEVENT: {
                if (_event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    _config.screen_width = _event.window.data1;
                    _config.screen_height = _event.window.data2;
                    updateSize();
                }
            } break;

            default:
                break;
        }
    }
    return _in_menu;
}
bool Engine::mainSettings() {
    renderSettings();
    while(SDL_PollEvent(&_event))
    {
        switch(_event.type)
        {
            case SDL_QUIT: {
                _close = true;
                _in_settings = false;
            }

            case SDL_KEYDOWN: {
                if (_event.key.keysym.sym == SDLK_ESCAPE)
                {
                    _in_settings = false;
                }
            } break;

            case SDL_MOUSEBUTTONDOWN: {
                if (checkIfSettingsButtonClicked())
                {
                    if(_config.difficulty.id < 4)
                    {
                        _config.difficulty.id++;
                    }
                    else
                    {
                        _config.difficulty.id = 0;
                    }
                    setDifficulty();
                    resetGame();
                }
            } break;

            case SDL_WINDOWEVENT: {
                if (_event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    _config.screen_width = _event.window.data1;
                    _config.screen_height = _event.window.data2;
                    updateSize();
                }
            } break;

            default:
                break;
        }
    }
    return _in_settings;
}
bool Engine::mainEndScreen() {
    renderEndScreen();
    while(SDL_PollEvent(&_event))
    {
        switch(_event.type)
        {
            case SDL_QUIT: {
                _close = true;
                return false;
            }

            case SDL_MOUSEBUTTONDOWN: {
                if(checkIfEndScreenButtonClicked())
                {
                    resetGame();
                    return false;
                }
            }

            case SDL_WINDOWEVENT: {
                if (_event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    _config.screen_width = _event.window.data1;
                    _config.screen_height = _event.window.data2;
                    updateSize();
                }
            } break;

            default:
                break;
        }
    }
    return true;
}
void Engine::game() {
    _timer.start();
    //The loop LOL
    while (mainGame());
    _moves_count;
}

//Why do I even comment all of those?
int Engine::checkIfTileClicked() {
    for(Tile &tile : _tiles)
    {
        if( tile.window_x_left < _event.button.x && _event.button.x < tile.window_x_right
            && tile.window_y_top < _event.button.y && _event.button.y < tile.window_y_bottom)
        {
            if(!tile.matched)
            {
                return tile.id;
            }
        }
    }
    return -1;
}
int Engine::checkIfMenuButtonClicked() {
    for(Button &button : _menu_buttons)
    {
        if( button.window_coords.x_left < _event.button.x && _event.button.x < button.window_coords.x_right
            && button.window_coords.y_top < _event.button.y && _event.button.y < button.window_coords.y_bottom)
        {
            return button.id;
        }
    }
    return -1;
}
int Engine::checkIfSettingsButtonClicked() {
    return (_settings_button.window_coords.x_left < _event.button.x && _event.button.x < _settings_button.window_coords.x_right
           && _settings_button.window_coords.y_top < _event.button.y && _event.button.y < _settings_button.window_coords.y_bottom);
}
bool Engine::checkIfEndScreenButtonClicked() {
    return (_end_game_button.window_coords.x_left < _event.button.x && _event.button.x < _end_game_button.window_coords.x_right
        && _end_game_button.window_coords.y_top < _event.button.y && _event.button.y < _end_game_button.window_coords.y_bottom);
}

//Checking if supposed to close the app
bool Engine::checkIfDone() {
    for (Tile &tile : _tiles)
    {
        if (!tile.matched)
        {
            return false;
        }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(_config.pause_duration));
    return true;
}

//Creating things and assigning races
void Engine::createTiles() {
  //Tile creation
    for(int i=0; i < _config.difficulty._tiles_in_column * _config.difficulty._tiles_in_row; i++)
    {
        Tile tile(i);
        _tiles.push_back(tile);
    }

  //Racial assignment
    std::srand(unsigned(std::time(NULL)));
    std::random_shuffle(_tiles.begin(), _tiles.end());

    for(int i=0; i < _tiles.size(); i+=2)
    {
        _tiles[i].race = i/2;
        _tiles[i+1].race = i/2;
    }

    std::sort(_tiles.begin(), _tiles.end(), CustomLessTileId);
}
void Engine::createButtons() {
  //Menu buttons
    Button temp_button;
    //Resume
    temp_button.id = 0;
    temp_button.texture = loadTexture(_config.resume_button_path, _renderer);
    _menu_buttons.push_back(temp_button);
    //New game
    temp_button.id = 1;
    temp_button.texture = loadTexture(_config.new_game_button_path, _renderer);
    _menu_buttons.push_back(temp_button);
    //Settings
    temp_button.id = 2;
    temp_button.texture = loadTexture(_config.settings_button_path, _renderer);
    _menu_buttons.push_back(temp_button);
    //Exit button
    temp_button.id = 3;
    temp_button.texture = loadTexture(_config.exit_button_path, _renderer);
    _menu_buttons.push_back(temp_button);

    _end_game_button = _menu_buttons[1];
    _settings_button.texture = _config.difficulty._texture;

}

//Whatever
WindowCoords Engine::getTileCoords(Tile &tile) {
    WindowCoords coords;
    coords.x_left = ((1+2*(tile.id%_config.difficulty._tiles_in_row))*_space_x+(tile.id%_config.difficulty._tiles_in_row)*_config.tile_width);
    coords.x_right = coords.x_left + _config.tile_width;
    coords.y_top = ((1+2*(tile.id/_config.difficulty._tiles_in_row))*_space_y+(tile.id/_config.difficulty._tiles_in_row)*_config.tile_height);
    coords.y_bottom = coords.y_top + _config.tile_height;
    return coords;
}
void Engine::resizeTile(Tile &tile) {
    WindowCoords coords = getTileCoords(tile);
    tile.window_x_left = coords.x_left;
    tile.window_x_right = coords.x_right;
    tile.window_y_top = coords.y_top;
    tile.window_y_bottom = coords.y_bottom;
}
void Engine::updateTiles() {
    for(Tile &tile : _tiles)
    {
        resizeTile(tile);
    }
}
void Engine::updateButtons() {
    _config.menu_buttons_space = _config.screen_height*25/1000;

    int temp_space = (_config.screen_height - (3 * _config.menu_buttons_space) - (4 * _config.button_height)) / 2;
    int x_left = (_config.screen_width - _config.button_width) / 2;
    int x_right = _menu_buttons[0].window_coords.x_left + _config.button_width;

    _menu_buttons[0].window_coords.x_left = x_left;
    _menu_buttons[0].window_coords.x_right = x_right;
    _menu_buttons[0].window_coords.y_top = (_menu_buttons[0].id * (_config.button_height + _config.menu_buttons_space)) + temp_space;
    _menu_buttons[0].window_coords.y_bottom = _menu_buttons[0].window_coords.y_top + _config.button_height;

    _menu_buttons[1].window_coords.x_left = x_left;
    _menu_buttons[1].window_coords.x_right = x_right;
    _menu_buttons[1].window_coords.y_top = (_menu_buttons[1].id * (_config.button_height + _config.menu_buttons_space)) + temp_space;
    _menu_buttons[1].window_coords.y_bottom = _menu_buttons[1].window_coords.y_top + _config.button_height;

    _menu_buttons[2].window_coords.x_left = x_left;
    _menu_buttons[2].window_coords.x_right = x_right;
    _menu_buttons[2].window_coords.y_top = (_menu_buttons[2].id * (_config.button_height + _config.menu_buttons_space)) + temp_space;
    _menu_buttons[2].window_coords.y_bottom = _menu_buttons[2].window_coords.y_top + _config.button_height;

    _menu_buttons[3].window_coords.x_left = x_left;
    _menu_buttons[3].window_coords.x_right = x_right;
    _menu_buttons[3].window_coords.y_top = (_menu_buttons[3].id * (_config.button_height + _config.menu_buttons_space)) + temp_space;
    _menu_buttons[3].window_coords.y_bottom = _menu_buttons[3].window_coords.y_top + _config.button_height;

    _end_game_button.window_coords.x_left = x_left;
    _end_game_button.window_coords.x_right = x_right;
    _end_game_button.window_coords.y_top = _menu_buttons[3].window_coords.y_top - (_config.button_height / 2);
    _end_game_button.window_coords.y_bottom = _end_game_button.window_coords.y_top + _config.button_height;

    _settings_button.window_coords.x_left = x_left;
    _settings_button.window_coords.x_right = x_right;
    _settings_button.window_coords.y_top = _menu_buttons[1].window_coords.y_top;
    _settings_button.window_coords.y_bottom = _menu_buttons[1].window_coords.y_bottom;
    _settings_button.id = _config.difficulty.id;
    _settings_button.texture = _config.difficulty._texture;
}
void Engine::updateSize() {
    int w, h;
    SDL_GetWindowSize(_window, &w, &h);

    _config.button_height = (h*57/100) / 4;
    _config.button_width = h*50/100;

    if(w/_config.difficulty._tiles_in_row > h/_config.difficulty._tiles_in_column)
    {
        _config.tile_height = (h*85/100) / _config.difficulty._tiles_in_column;
        _config.tile_width = _config.tile_height;
    }
    else
    {
        _config.tile_width = (w*85/100) / _config.difficulty._tiles_in_row;
        _config.tile_height = _config.tile_width;
    }

    _space_x = (_config.screen_width - (_config.difficulty._tiles_in_row * _config.tile_width)) / (2 * _config.difficulty._tiles_in_row);
    _space_y = (_config.screen_height - (_config.difficulty._tiles_in_column * _config.tile_height)) / (2 * _config.difficulty._tiles_in_column);

    updateTiles();
    updateButtons();
}

//Rendering
void Engine::renderClear() {
    SDL_RenderClear(_renderer);
}
void Engine::renderBackground() {
    renderTexture(_background_texture, _renderer, 0, 0, _config.screen_width, _config.screen_height);
}
void Engine::renderPresent() {
    SDL_RenderPresent(_renderer);
}
void Engine::renderTile(Tile &tile) {
    if (!tile.matched)
    {
        if (tile.reversed)
        {
            renderTexture(_tile_front_textures[tile.race], _renderer,
                          tile.window_x_left, tile.window_y_top,       //top left coords
                          _config.tile_width, _config.tile_height);    //size
        }
        else
        {
            renderTexture(_tile_reverse_texture, _renderer,
                          tile.window_x_left, tile.window_y_top,       //top left coords
                          _config.tile_width, _config.tile_height);    //size
        }
    }
}
void Engine::renderButton(Button &button) {
    renderTexture(button.texture, _renderer,
                  button.window_coords.x_left, button.window_coords.y_top,
                  _config.button_width, _config.button_height);
}
void Engine::renderGame() {
    renderBackground();
    for (Tile &tile : _tiles)
    {
        renderTile(tile);
    }
}
void Engine::renderMenu() {
    renderTexture(_darker, _renderer, 0, 0, _config.screen_width, _config.screen_height);
    for(Button &button : _menu_buttons)
    {
        renderButton(button);
    }
}
void Engine::renderSettings() {
    renderClear();
    renderGame();
    renderTexture(_darker, _renderer, 0, 0, _config.screen_width, _config.screen_height);
    renderButton(_settings_button);
    renderPresent();
}
void Engine::renderAll() {
    if(_in_menu)
    {
        renderClear();
        renderGame();
        renderMenu();
        renderPresent();
    }
    else
    {
        renderClear();
        renderGame();
        renderPresent();
    }
}
void Engine::renderEndScreen() {
    renderClear();
    renderBackground();
    renderTexture(_darker, _renderer, 0, 0, _config.screen_width, _config.screen_height);
    renderButton(_end_game_button);
    SDL_RenderCopy(_renderer, _score_texture, NULL, NULL);
    renderPresent();
}

//Resetting
void Engine::resetGame() {
    _tiles.clear();
    createTiles();
    updateTiles();
    updateSize();
    _timer.resetTime();
    _moves_count = 0;
}

//Destructor (cleanup)
Engine::~Engine() {
    for(int i=0; i < _tile_front_textures.size(); i++)
    {
        cleanup(_tile_front_textures[i]);
    }
    cleanup(_background_texture, _renderer, _window, _score_texture, _tile_reverse_texture, _score_surface);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    _config.saveSettings();
}
//