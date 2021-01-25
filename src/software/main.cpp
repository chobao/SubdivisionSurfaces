#include "visualization/gl_viewer.h"
#include "base/model.h"
#include "utils/config.h"
#include "utils/io_utils.h"
#include "subdivision/mesh.h"


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

int testModel( const CommonUtils::Config& config){
   

    //config.SetUp("../../config.yaml");
    
    const int scr_width = config.view_width, scr_height = config.view_height;
    Model model;
    model.LoadObj(config.model_path);
    model.SetShaderColor(scr_width, scr_height);
    std::shared_ptr<std::vector<float>> vertices;
    size_t num_vertex;
    std::tie(vertices, num_vertex) = model.ConvertToTriangularMesh();
    

    GLRendering::Viewer& viewer =  GLRendering::Viewer::Instance();
    viewer.SetUp(scr_width, scr_height, config.vertex_shader_path, config.fragment_shader_path);
    viewer.CreateMeshPNC(vertices->data(), 9, num_vertex, glm::vec3(0.4, 0.4, 0.0) ,glm::mat4(1.0));
    viewer.Run();
    return 0;
}

int solve( const CommonUtils::Config& config) {
    const std::string file_path;
    const int scr_width = config.view_width, scr_height = config.view_height;

    std::vector<Eigen::Vector3d> vertices;
    std::vector<std::vector<index_t>> polygons;
    CommonUtils::LoadObj(config.model_path, vertices, polygons);
    SubDivision::Mesh mesh1;
    mesh1.SetUp(vertices, polygons);
    mesh1.PrintObj();
    mesh1.PrintPolygon();

    std::shared_ptr<std::vector<float>> pData;
    size_t num_vertex;
    std::tie(pData, num_vertex) = mesh1.ConvertToTriangularMesh();
    

    GLRendering::Viewer& viewer =  GLRendering::Viewer::Instance();
    viewer.SetUp(scr_width, scr_height, config.vertex_shader_path, config.fragment_shader_path);
    viewer.CreateMeshPNC(pData->data(), 9, num_vertex, glm::vec3(0.4, 0.4, 0.0) ,glm::mat4(1.0));
    viewer.Run();
}

int main(){
     CommonUtils::Config config;
    config.view_width = 800, config.view_height = 600;
    config.model_path = "/home/baochong/Projects/HomeWork/GeometryModeling/SubdivisionSurface/res/teapot.obj";
    config.vertex_shader_path = "../../src/visualization/model_v.glsl";
    config.fragment_shader_path = "../../src/visualization/model_f.glsl";
    solve(config);
}