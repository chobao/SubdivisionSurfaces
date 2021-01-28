#include "visualization/gl_viewer.h"
#include "utils/config.h"



void Run(const CommonUtils::Config& config) {
    GLRendering::Viewer& viewer =  GLRendering::Viewer::Instance();
    if(viewer.SetUp(config.view_width, config.view_height, config.vertex_shader_path, config.fragment_shader_path, config)) {
        viewer.Run();
    }
}


int main(int argc, const char* argv[]){
    if(argc != 4) {
        std::cout<<"Please enter mainSubdivisionSurface [obj_path] [vertex_shader_path] [fragment_shader_path]\n";
        return 0;
    }

     CommonUtils::Config config;
    config.view_width = 800, config.view_height = 600;
    config.model_path = argv[1];
    config.vertex_shader_path = argv[2];
    config.fragment_shader_path = argv[3];
    config.maximum_level = 3;
    Run(config);
    //test3dOrdering();
}