#include "visualization/gl_viewer.h"
#include "base/model.h"
#include "utils/config.h"


int testRendering() {
    GLRendering::Viewer& viewer =  GLRendering::Viewer::Instance();
    Model model;
    //ConvertToTriangularMesh 
    std::shared_ptr<std::vector<float>> triangular_mesh_data;
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.1f, 0.5f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.1f, 0.5f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.1f, 0.5f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.1f, 0.5f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.1f, 0.5f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 0.0f, 0.1f, 0.5f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.3f, 0.3f, 0.5f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.3f, 0.3f, 0.5f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.3f, 0.3f, 0.5f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.3f, 0.3f, 0.5f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.3f, 0.3f, 0.5f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f, 0.3f, 0.3f, 0.5f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.1f, 0.3f, 0.5f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.1f, 0.3f, 0.5f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.1f, 0.3f, 0.5f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f, 0.1f, 0.3f, 0.5f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.1f, 0.3f, 0.5f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f, 0.1f, 0.3f, 0.5f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.6f, 0.3f, 0.2f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.6f, 0.3f, 0.2f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.6f, 0.3f, 0.2f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f, 0.6f, 0.3f, 0.2f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.6f, 0.3f, 0.2f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f, 0.6f, 0.3f, 0.2f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.7f, 0.6f, 0.2f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.7f, 0.6f, 0.2f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.7f, 0.6f, 0.2f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.7f, 0.6f, 0.2f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f, 0.7f, 0.6f, 0.2f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f, 0.7f, 0.6f, 0.2f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.7f, 0.6f, 0.6f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.7f, 0.6f, 0.6f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.7f, 0.6f, 0.6f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.7f, 0.6f, 0.6f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f, 0.7f, 0.6f, 0.6f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f, 0.7f, 0.6f, 0.6f
    };
    viewer.SetUp(800, 600, "../../src/visualization/model_v.glsl", "../../src/visualization/model_f.glsl");
    viewer.CreateMeshPNC(vertices, 9, 36, glm::vec3(0.4, 0.4, 0.0), glm::mat4(1.0));
    viewer.Run();
    return 0;
}

int solve(){
    CommonUtils::Config config;
    config.view_width = 800, config.view_height = 600;
    config.model_path = "../../res/teapot.obj";
    config.vertex_shader_path = "../../src/visualization/model_v.glsl";
    config.fragment_shader_path = "../../src/visualization/model_f.glsl";




    //config.SetUp("../../config.yaml");
    GLRendering::Viewer& viewer =  GLRendering::Viewer::Instance();
    const int scr_width = config.view_width, scr_height = config.view_height;
    Model model;
    model.LoadObj(config.model_path);
    model.SetShaderColor(scr_width, scr_height);
    std::shared_ptr<std::vector<float>> vertices;
    size_t num_vertex;
    std::tie(vertices, num_vertex) = model.ConvertToTriangularMesh();
    
    viewer.SetUp(scr_width, scr_height, config.vertex_shader_path, config.fragment_shader_path);
    viewer.CreateMeshPNC(vertices->data(), 9, num_vertex, glm::vec3(0.4, 0.4, 0.0) ,glm::mat4(1.0));
    viewer.Run();
    return 0;
}

int main(){
    solve();
}