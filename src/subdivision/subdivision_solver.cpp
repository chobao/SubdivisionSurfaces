#include "subdivision_solver.h"

namespace SubDivision {
        std::vector<Eigen::Vector3d> SubDivisionSolver::GetEdgePoints(const Mesh& mesh) {

                const std::vector<std::shared_ptr<Edge>>& edges = mesh.Edges();
                std::vector<Eigen::Vector3d> edge_points(edges.size());

                for(int i = 0 ; i < edges.size() ; i++) {
                    const Eigen::Vector3d& p1 = mesh.VertexPoint(edges[i]->id1);
                    const Eigen::Vector3d& p2 = mesh.VertexPoint(edges[i]->id2);
                    Eigen::Vector3d mid_point = (p1 + p2) / 2.0;
                    //index_t  edge_point_id = CreatePoint(mid_point);
                    edge_points[i] = mid_point;
                }
                return edge_points;
        }

        std::vector<Eigen::Vector3d> SubDivisionSolver::GetFacePoints(const Mesh& mesh) {

            const std::vector<std::shared_ptr<Polygon>>& polygons = mesh.Polygons();
            std::vector<Eigen::Vector3d> face_points(polygons.size());

            for(int i = 0 ; i < polygons.size() ; i++) {
                const auto& points = polygons[i]->points;
                const size_t num_points = points.size();
                Eigen::Vector3d sum_points = Eigen::Vector3d::Zero();
                for(int j = 0 ; j < num_points ; j++) {
                    sum_points += mesh.VertexPoint(points[j]);
                }
                face_points[i] = sum_points / static_cast<double>(num_points);
            }

            return face_points;
        }

        void SubDivisionSolver::MakeUpMesh(std::vector<std::vector<index_t>>& polygons,
                        Mesh& updated_mesh) {
            // for(auto& polygon: polygons) {
            //     const Eigen::Vector3d& p0 = point_table_.at(polygon[0]);
            //     Eigen::Vector3d norm = ((point_table_.at(polygon[1]) - p0).cross(point_table_.at(polygon[2]) - p0)).normalized();
            //     GeometryUtils::SortConvexPointIndexSetInClockWise(point_table_, norm, polygon);
            // }
            updated_mesh.SetUp(point_table_, polygons);
        }   


}