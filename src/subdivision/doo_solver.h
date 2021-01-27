#pragma once
#include "subdivision_solver.h"
#include <map>


namespace SubDivision {

    class DooSabinSolver: public SubDivisionSolver {
    public:
        bool Run(const Mesh& mesh, Mesh& updated_mesh);

        bool UpdateVertices(const Mesh& mesh, const std::vector<Eigen::Vector3d>& face_points, 
                            const std::vector<Eigen::Vector3d>& edge_points, 
                            std::map<index_t,std::map<index_t, index_t>>& indexed_vertices,
                            std::vector<std::vector<index_t>>& updated_polygons);

        bool UpdatedEdgePolygon(const Mesh& mesh, const std::map<index_t,std::map<index_t, index_t>>& indexed_vertices,
                                std::vector<std::vector<index_t>>& updated_edge_polygons);

        bool UpdateVertexPolygon(const Mesh& mesh, const std::map<index_t,std::map<index_t, index_t>>& indexed_vertices,
                                std::vector<std::vector<index_t>>& updated_vertex_polygons);
        bool Divide(const Mesh& mesh,
                const std::vector<std::vector<index_t>>& updated_polygons,
                const std::vector<std::vector<index_t>>& updated_edge_polygons,
                const std::vector<std::vector<index_t>>& updated_vertex_polygons,
                std::vector<std::vector<index_t>>& polygons);
        
        const bool verbose = true;
    };
}