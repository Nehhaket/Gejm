#ifndef GEJM_CONFIG_H
#define GEJM_CONFIG_H


#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "Structures.h"



//Config class declaration
class Config
{
private:
  //Private fields
    boost::property_tree::ptree _scores_table;
    std::string _scores_table_path = "../scores_table.ini";
    std::string _the_path;

public:
  //Public methods
    // Constructor
    Config(std::string config_path);
    void saveSettings();
    boost::property_tree::ptree _properties;
    void loadSettings();
    void loadScoresTable();
    void addScore(Score score);

  //Public fields
    std::vector<DifficultyLevel> difficulty_levels;
    int screen_width = 750;
    int screen_height = 480;
    int button_width;
    int button_height;
    int tile_width;
    int tile_height;
    int pause_duration = 350;
    int menu_buttons_space;
    DifficultyLevel difficulty;
    //image paths
    std::string easy;
    std::string normal;
    std::string intermediate;
    std::string advanced;
    std::string hard;
    std::string resume_button_path;
    std::string new_game_button_path;
    std::string settings_button_path;
    std::string exit_button_path;
    std::string darkerer_path;
    std::string tile_reverse_path;
    std::string background_path;
    std::vector<std::string> tile_front_paths;
    std::vector<Score> scores;
};
//


#endif //GEJM_CONFIG_H