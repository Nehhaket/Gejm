#include "../include/Config.h"



//Config class implementation


//Constructor
Config::Config(std::string config_path) {
    _the_path = config_path;
    loadSettings();
}

//Loading settings
void Config::loadSettings() {
    boost::property_tree::ini_parser::read_ini(_the_path, _properties);
    resume_button_path = _properties.get<std::string>("resume_button_path");
    new_game_button_path = _properties.get<std::string>("new_game_button_path");
    settings_button_path = _properties.get<std::string>("settings_button_path");
  //Loading public fields
    exit_button_path = _properties.get<std::string>("exit_button_path");
    tile_reverse_path = _properties.get<std::string>("tile_reverse_path");
    background_path = _properties.get<std::string>("background_path");
    darkerer_path = _properties.get<std::string>("darkerer_path");
    easy = _properties.get<std::string>("easy");
    normal = _properties.get<std::string>("normal");
    intermediate = _properties.get<std::string>("intermediate");
    advanced = _properties.get<std::string>("advanced");
    hard = _properties.get<std::string>("hard");

    //Loading front paths vector
    for(int i = 1; i < 16; i++)
    {
        std::string temp_path = "tile_front_path_" + std::to_string(i);
        tile_front_paths.push_back(_properties.get<std::string>(temp_path));
    }
  //
}

//Saving
void Config::saveSettings() {
    _properties.put("difficulty_level", std::to_string(difficulty.id));
    boost::property_tree::ini_parser::write_ini(_the_path, _properties);
}

//Loading scores
void Config::loadScoresTable() {
    boost::property_tree::ini_parser::read_ini(_scores_table_path, _scores_table);
}

//Adding score
void Config::addScore(Score score) {
    _scores_table.put(score.name + ".time", std::to_string(score.time));
    _scores_table.put(score.name + ".moves", std::to_string(score.moves));
    boost::property_tree::ini_parser::write_ini(_scores_table_path, _scores_table);
}