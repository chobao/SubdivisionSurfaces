#pragma once
#include <string>
#include <iostream>

namespace CommonUtils{

struct Config{
    std::string model_path;
    int view_width, view_height;
    std::string vertex_shader_path, fragment_shader_path;
    int maximum_level;

    // bool SetUp(std::string config_path) {
    //     cv::FileStorage fs;
    //     fs.open(config_path, cv::FileStorage::READ);
    //     if (!fs.isOpened())
    //     {
    //         std::cerr << "Failed to open " << config_path << std::endl;
    //         return false;
    //     }

    //     cv::FileNode node_width = fs["width"];
    //     cv::FileNode node_height = fs["height"];
    //     cv::FileNode node_model_path = fs["model_path"];
    //     model_path = std::string(node_model_path);
    //     view_width = int(node_width);
    //     view_height = int(node_height);


    //     return true;
    // }



    
};

}
