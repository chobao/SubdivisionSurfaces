#include "visualization/gl_viewer.h"
#include "utils/config.h"



void Run(const CommonUtils::Config& config) {
    GLRendering::Viewer& viewer =  GLRendering::Viewer::Instance();
    if(viewer.SetUp(config.view_width, config.view_height, config.vertex_shader_path, config.fragment_shader_path, config)) {
        viewer.Run();
    }
}


int main(int argc, const char* argv[]){
    if(argc != 2) {
        std::cout<<"Please enter mainSubdivisionSurface [obj_path]";
        return 0;
    }

     CommonUtils::Config config;
    config.view_width = 800, config.view_height = 600;
    config.model_path = argv[1];
    config.vertex_shader_path = "/home/baochong/Projects/HomeWork/GeometryModeling/SubdivisionSurface/src/visualization/model_v.glsl";
    config.fragment_shader_path = "/home/baochong/Projects/HomeWork/GeometryModeling/SubdivisionSurface/src/visualization/model_f.glsl";
    config.maximum_level = 3;
    Run(config);
    //test3dOrdering();
}