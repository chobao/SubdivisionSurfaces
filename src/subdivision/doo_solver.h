#pragma once
#include "subdivision_solver.h"

namespace SubDivision {

    class DooSabinSolver: public SubDivisionSolver {
    public:
        bool Run(const Mesh& mesh, Mesh& updated_mesh);

        bool UpdateVertices(const Mesh& mesh, const std::vector<Eigen::Vector3d>& face_points, 
                            const std::vector<Eigen::Vector3d>& edge_points,
                            std::vector<std::vector<index_t>>& updated_polygon_vertices,
                            std::vector<std::vector<index_t>>& updated_edge_vertices,
                            std::vector<std::vector<index_t>>& updated_vertex_vertices);

        bool Divide(const Mesh& mesh,
                const std::vector<std::vector<index_t>>& updated_polygon_vertices,
                const std::vector<std::vector<index_t>>& updated_edge_vertices,
                const std::vector<std::vector<index_t>>& updated_vertex_vertices,
                std::vector<std::vector<index_t>>& polygons);
        
        const bool verbose = true;
    };
}