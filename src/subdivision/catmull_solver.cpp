#include "catmull_solver.h"
namespace SubDivision {
    void CatmullClarkSolver::Run(const Mesh& mesh, Mesh& updated_mesh) {
        std::vector<index_t> edge_points = GetEdgePoints(mesh);
        std::vector<index_t> face_points = GetFacePoints(mesh);
        // UpdateEdgePoints();
        // UpdateVertices();
        // Divide();
        // MakeUpMesh();
    }
}