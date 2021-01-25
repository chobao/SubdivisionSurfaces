#include "subdivision_solver.h"

namespace SubDivision {
        std::vector<index_t> SubDivisionSolver::GetEdgePoints(const Mesh& mesh) {

                const std::vector<std::shared_ptr<Edge>>& edges = mesh.Edges();
                std::vector<index_t> edge_points(edges.size());

                for(int i = 0 ; i < edges.size() ; i++) {
                    const Eigen::Vector3d& p1 = mesh.VertexPoint(edges[i]->id1);
                    const Eigen::Vector3d& p2 = mesh.VertexPoint(edges[i]->id2);
                    Eigen::Vector3d mid_point = (p1 + p2) / 2.0;
                    index_t  edge_point_id = CreatePoint(mid_point);
                    edge_points[i] = edge_point_id;
                }
                return edge_points;
        }

        std::vector<index_t> SubDivisionSolver::GetFacePoints(const Mesh& mesh) {

            const std::vector<std::shared_ptr<Polygon>>& polygons = mesh.Polygons();
            std::vector<index_t> face_points(polygons.size());

            Eigen::Vector3d sum_points = Eigen::Vector3d::Zero();
            for(int i = 0 ; i < polygons.size() ; i++) {
                const auto& points = polygons[i]->points;
                const size_t num_points = points.size();
                for(int j = 0 ; j < num_points ; j++) {
                    sum_points += mesh.VertexPoint(points[j]);
                }
                face_points[i] = CreatePoint(sum_points / static_cast<double>(num_points));
            }

            return face_points;
        }


}