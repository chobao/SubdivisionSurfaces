#pragma once
#include "subdivision_solver.h"

namespace SubDivision {
    class LoopSolver: public SubDivisionSolver {
    public:
        bool Run(const Mesh& mesh, Mesh& updated_mesh);

        bool CheckCheirality(const Mesh& mesh);

        void UpdateEdgePoints(const Mesh& mesh, 
                    std::vector<index_t>& updated_edge_points);
        bool UpdateVertices(const Mesh& mesh,
                            std::vector<index_t>& updated_vertices);

        void Divide(const Mesh& mesh,
                    const std::vector<index_t>& updated_edge_points,
                    const std::vector<index_t>& updated_vertices,
                    std::vector<std::vector<index_t>>& updated_polygons);
        const bool verbose = true;
    };
}