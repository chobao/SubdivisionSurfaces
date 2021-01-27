#include "doo_solver.h"
#include <unordered_map>

namespace SubDivision {
    bool DooSabinSolver::Run(const Mesh& mesh, Mesh& updated_mesh) {
        std::vector<Eigen::Vector3d> edge_points = GetEdgePoints(mesh);  //mesh.edges <-> edge_points (index consistence)
        std::vector<Eigen::Vector3d> face_points = GetFacePoints(mesh);  //mesh.polygons <-> face_points (index consistence)
        std::vector<std::vector<index_t>> updated_polygon_vertices;
        std::vector<std::vector<index_t>> updated_edge_vertices;
        std::vector<std::vector<index_t>> updated_vertex_vertices;
        std::vector<std::vector<index_t>> polygons;
        if(verbose) {
            std::cout<<"edge_points:\n";
            CommonUtils::PrintEigenVector(edge_points);
            std::cout<<"face_points:\n";
            CommonUtils::PrintEigenVector(face_points);
        }
        

        if(!UpdateVertices(mesh, face_points, edge_points, updated_polygon_vertices,updated_edge_vertices,
               updated_vertex_vertices)) {
            return false;
        }
        if(verbose) {
            std::cout<<"UpdateVertices vertex table:\n";
            PrintPointTable();
            std::cout<<"UpdateVertices polygon vertices:\n";
            for(int i = 0 ; i < updated_polygon_vertices.size() ; i++) {
                std::vector<index_t> out_poly(updated_polygon_vertices[i]);
                for(int j = 0; j < out_poly.size() ; j++) {
                    out_poly[j]++;
                }
                CommonUtils::Print<index_t>(out_poly);
            }
            std::cout<<"UpdateVertices edge vertices:\n";
            for(int i = 0 ; i < updated_edge_vertices.size() ; i++) {
                std::vector<index_t> out_poly(updated_edge_vertices[i]);
                for(int j = 0; j < out_poly.size() ; j++) {
                    out_poly[j]++;
                }
                CommonUtils::Print<index_t>(out_poly);
            }
            std::cout<<"UpdateVertices vertex vertices:\n";
            for(int i = 0 ; i < updated_vertex_vertices.size() ; i++) {
                std::vector<index_t> out_poly(updated_vertex_vertices[i]);
                for(int j = 0; j < out_poly.size() ; j++) {
                    out_poly[j]++;
                }
                CommonUtils::Print<index_t>(out_poly);
            }

        }
        
        Divide(mesh,updated_polygon_vertices,updated_edge_vertices,updated_vertex_vertices,
               polygons);
        if(verbose) {
            std::cout<<"Divide:\n";
            for(int i = 0 ; i < polygons.size() ; i++) {
                std::vector<index_t> out_poly(polygons[i]);
                for(int j = 0; j < out_poly.size() ; j++) {
                    out_poly[j]++;
                }
                CommonUtils::Print<index_t>(out_poly);
            }
        }
        MakeUpMesh(polygons, updated_mesh);
        if(verbose) {
            std::cout<<"MakeUpMesh:\n";
            for(int i = 0 ; i < polygons.size() ; i++) {
                std::vector<index_t> out_poly(polygons[i]);
                for(int j = 0; j < out_poly.size() ; j++) {
                    out_poly[j]++;
                }
                CommonUtils::Print<index_t>(out_poly);
            }
        }
        return true;
    } 


    bool DooSabinSolver::UpdateVertices(const Mesh& mesh, const std::vector<Eigen::Vector3d>& face_points, 
                            const std::vector<Eigen::Vector3d>& edge_points,
                            std::vector<std::vector<index_t>>& updated_polygon_vertices,
                            std::vector<std::vector<index_t>>& updated_edge_vertices,
                            std::vector<std::vector<index_t>>& updated_vertex_vertices) {
        const auto& polygons = mesh.Polygons();
        updated_polygon_vertices.resize(polygons.size());
        updated_edge_vertices.resize(mesh.Edges().size());
        updated_vertex_vertices.resize(mesh.Vertices().size());

        for(int i = 0 ; i < polygons.size() ;i++) {
            const std::shared_ptr<Polygon>& polygon = polygons[i];
            const std::vector<index_t>& polygon_edges = polygon->edges;
            const std::vector<index_t>& points = polygon->points;
            //collect the associated edges of each vertex in the polygon
            std::unordered_map<index_t, std::vector<index_t>> asso_edges_per_vertex;
            asso_edges_per_vertex.reserve(points.size());
            for(int j = 0 ; j < polygon_edges.size() ; j++) {
                const std::shared_ptr<Edge>& edge = mesh.EdgeElement(polygon_edges[j]);
                asso_edges_per_vertex[edge->id1].emplace_back(polygon_edges[j]);
                asso_edges_per_vertex[edge->id2].emplace_back(polygon_edges[j]);
            }
            // sum up the edge point of associated edges and face point for each vertex in polgyon
            for(const auto vertex_id : points) {
                const auto& asso_edges = asso_edges_per_vertex.at(vertex_id);
                if(asso_edges.size() != 2) {
                    std::cerr << "Error: the number of vertex in the polygon does not equals to 2.\n";
                    continue;
                }
                Eigen::Vector3d sum_edge_points = Eigen::Vector3d::Zero();
                for(const auto& edge_id : asso_edges) {
                    sum_edge_points += edge_points[edge_id];
                }

                const Eigen::Vector3d old_point = mesh.VertexPoint(vertex_id);
                Eigen::Vector3d updated_vertices = (sum_edge_points + old_point + face_points[i]) / 4.0;
                index_t updated_vertex_id = CreatePoint(updated_vertices);

                // link the used primitives with updated vertex
                updated_polygon_vertices[i].emplace_back(updated_vertex_id);
                for(const auto& edge_id : asso_edges) {
                    updated_edge_vertices[edge_id].emplace_back(updated_vertex_id);
                }
                updated_vertex_vertices[vertex_id].emplace_back(updated_vertex_id);
                
            }
            
        }
        return true;
    }

    bool DooSabinSolver::Divide(const Mesh& mesh,
                const std::vector<std::vector<index_t>>& updated_polygon_vertices,
                const std::vector<std::vector<index_t>>& updated_edge_vertices,
                const std::vector<std::vector<index_t>>& updated_vertex_vertices,
                std::vector<std::vector<index_t>>& polygons) {
        for(const auto& vertices :updated_polygon_vertices) {
            //generate face 	
            if(vertices.size() >= 3) {
                polygons.emplace_back(vertices);
            }
        }
        for(const auto& vertices: updated_edge_vertices) {
            if(vertices.size() >= 3) {
                polygons.emplace_back(vertices);
            }
        }
        for(const auto& vertices: updated_vertex_vertices) {
            if(vertices.size() >= 3) {
                polygons.emplace_back(vertices);
            }
            
        }
        return true;
    }
}