#pragma once
#include "subdivision_solver.h"

namespace SubDivision {

    class CatmullClarkSolver: public SubDivisionSolver {
    public:
        void Run(const Mesh& mesh, Mesh& updated_mesh);

        void UpdateEdgePoints(const Mesh& mesh, 
                    const std::vector<index_t>& edge_points, const std::vector<index_t>& face_points, 
                    std::vector<index_t>& updated_edge_points);
        void UpdateVertices(const Mesh& mesh, const std::vector<index_t>& face_points, 
                            const std::vector<index_t>& edge_points,
                            std::vector<index_t>& updated_vertices);

        void Divide(const Mesh& mesh, const std::vector<index_t>& updated_vertices,
                    const std::vector<index_t>& updated_edge_points,
                    std::vector<std::vector<index_t>>& polygons);
    
    };
}