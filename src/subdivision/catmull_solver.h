#pragma once
#include "subdivision_solver.h"

namespace SubDivision {

    class CatmullClarkSolver: public SubDivisionSolver {
    public:
        bool Run(const Mesh& mesh, Mesh& updated_mesh) override;

        void UpdateFacePoints(const std::vector<Eigen::Vector3d>& face_points, 
                              std::vector<index_t>& updated_face_points);

        void UpdateEdgePoints(const Mesh& mesh, 
                    const std::vector<Eigen::Vector3d>& face_points, 
                    std::vector<index_t>& updated_edge_points);
        bool UpdateVertices(const Mesh& mesh, const std::vector<Eigen::Vector3d>& face_points, 
                            const std::vector<Eigen::Vector3d>& edge_points,
                            std::vector<index_t>& updated_vertices);

        void Divide(const Mesh& mesh, const std::vector<index_t>& updated_face_points,
                    const std::vector<index_t>& updated_edge_points,
                    const std::vector<index_t>& updated_vertices,
                    std::vector<std::vector<index_t>>& updated_polygons);
        const bool verbose = false;
    };
}