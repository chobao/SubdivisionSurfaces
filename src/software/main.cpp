#include "visualization/gl_viewer.h"
#include "base/model.h"
#include "utils/config.h"
#include "utils/io_utils.h"
#include "subdivision/mesh.h"
#include "subdivision/catmull_solver.h"
#include "utils/geometry_utils.h"
#include <numeric>


int testRendering( const CommonUtils::Config& config) {
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
    viewer.SetUp(800, 600, config.vertex_shader_path, config.fragment_shader_path);
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
    const size_t num_levels = 2;

    std::vector<Eigen::Vector3d> vertices;
    std::vector<std::vector<index_t>> polygons;
    CommonUtils::LoadObj(config.model_path, vertices, polygons);
    std::vector<SubDivision::Mesh> meshes;
    meshes.reserve(num_levels);
    meshes.emplace_back(SubDivision::Mesh());
    meshes[0].SetUp(vertices, polygons);
    
    meshes[0].PrintObj();
    //meshes[0]->PrintPolygon();

    SubDivision::CatmullClarkSolver clark_division_solver;
    for(int i = 1 ; i < num_levels ; i++) {
        SubDivision::Mesh updated_mesh;
        clark_division_solver.Run((meshes[i-1]), (updated_mesh));
        meshes.emplace_back(updated_mesh);
    }

    

    GLRendering::Viewer& viewer =  GLRendering::Viewer::Instance();
    if(viewer.SetUp(scr_width, scr_height, config.vertex_shader_path, config.fragment_shader_path)) {

        for(int i = 1 ; i < num_levels ; i++) {
            std::vector<float> pData;
            size_t num_vertex;
            std::tie(pData, num_vertex) = meshes[i].ConvertToTriangularMesh();
            std::cout<<i<<" pData->size(): "<<pData.size()<<"\n";
            viewer.CreateMeshPNC(pData.data(), 9, num_vertex, glm::vec3(0.4, 0.4, 0.0) ,glm::mat4(1.0));
            break;
        }
        viewer.Run();
    }
    
}

void test3dOrdering() {

    double nonconvex_array[] = {
-1,0,-1,
-0.5,-2,-1,
0,-1,-1,
0.5,-2,-1,
1,0,-1,
0.5,2,-1,
0,1,-1,
-0.5,2,-1
    };
    double array[] = {
0.252253,2.18528,4.1547,
-0.013863,2.3402,4.07724,
-0.260821,2.1617,4.1665,
-0.147333,1.89646,4.29911,
0.169764,1.91104,4.29182
    };
    int num_vertex = 5;
    std::vector<Eigen::Vector3d> points(num_vertex);
    for(int i = 0 ;i < num_vertex; i++) {
        points[i] = Eigen::Vector3d(array[i*3], array[i*3+1], array[i*3+2]);
    }
    Eigen::Vector3d norm;
    assert(num_vertex >= 3);
    norm = ((points[1] - points[0]).cross(points[2] - points[0])).normalized();
    
    std::vector<bool> vis(num_vertex, false);
    std::vector<index_t> indexes(num_vertex);
    std::cout<<"input order: ";
    for(int i = 0 ; i < num_vertex; i++) {
        int number = rand()%num_vertex;
        while(vis[number]) {
            number = rand()%num_vertex;
        }
        vis[number] = true;
        indexes[i] = number;
        std::cout<<number+1<<" ";
    }
    std::cout<<"\n";
    GeometryUtils::SortConvexPointIndexSetInClockWise(points, norm, indexes);
    std::cout<<"output:";
    for(int i = 0 ;i < indexes.size() ; i++) {
        std::cout<<indexes[i]+1<<" ";
    }
    std::cout<<"\n";

}


int main(){
     CommonUtils::Config config;
    config.view_width = 800, config.view_height = 600;
    config.model_path = "/home/baochong/Projects/HomeWork/GeometryModeling/SubdivisionSurface/res/cube.obj";
    config.vertex_shader_path = "/home/baochong/Projects/HomeWork/GeometryModeling/SubdivisionSurface/src/visualization/model_v.glsl";
    config.fragment_shader_path = "/home/baochong/Projects/HomeWork/GeometryModeling/SubdivisionSurface/src/visualization/model_f.glsl";
    solve(config);
    //test3dOrdering();
}