#include "loop_solver.h"
namespace SubDivision {

    bool LoopSolver::CheckCheirality(const Mesh& mesh) {
        const auto& polygons = mesh.Polygons();

        for(int i = 0 ; i < polygons.size(); i++) {
            if(polygons[i]->NumPoints() != 3 || polygons[i]->edges.size() != 3) {
                return false;
            }
        }
        return true;
    }

    bool LoopSolver::Run(const Mesh& mesh, Mesh& updated_mesh) {
        
        if(!CheckCheirality(mesh)) {
            //std::cerr << "Error: input mesh contains non-triangles.\n";
            return false;
        }
        
        std::vector<index_t> updated_edge_points; //mesh.edges <-> updated_edge_points (index consistence)
        std::vector<index_t> updated_vertices; //mesh.vertices <-> updated_vertices (index consistence)
        std::vector<std::vector<index_t>> polygons;
        
        UpdateEdgePoints(mesh, updated_edge_points);
        if(verbose) {
            std::cout<<"UpdateEdgePoints:\n";
            PrintPointTable();
        }
        if(!UpdateVertices(mesh, updated_vertices)) {
            return false;
        }
        if(verbose) {
            std::cout<<"UpdateVertices:\n";
            PrintPointTable();
        }
        Divide(mesh, updated_edge_points, updated_vertices, polygons);
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

    void LoopSolver::UpdateEdgePoints(const Mesh& mesh, 
                    std::vector<index_t>& updated_edge_points) {
        const auto& edges = mesh.Edges();
        updated_edge_points.resize(edges.size());
        for(int i = 0 ; i < edges.size() ; i++) {
            const std::shared_ptr<Edge>& edge = edges[i];
            const Eigen::Vector3d& p1 = mesh.VertexPoint(edge->id1);
            const Eigen::Vector3d& p2 = mesh.VertexPoint(edge->id2);
            const std::vector<index_t>& asso_polygons = edge->associated_polygons;
            bool b_boundary = asso_polygons.size() <= 1 ? true : false;
            if(b_boundary) {
                updated_edge_points[i] = CreatePoint((p1 + p2) / 2.0);
            } else {
                Eigen::Vector3d sum_neighbour_vertices = Eigen::Vector3d::Zero();
                for(const auto& polygon_id : asso_polygons) {  // #asso_polygons should be 2
                    const auto& points = mesh.PolygonElement(polygon_id)->points;
                    for(const auto& vertex_id : points) {
                        if(vertex_id != edge->id1 && vertex_id!= edge->id2) {
                            sum_neighbour_vertices += mesh.VertexPoint(vertex_id);
                        }
                    }
                }
                Eigen::Vector3d updated_edge_point = 0.375 * (p1+p2) + 0.125 * sum_neighbour_vertices;
                updated_edge_points[i] = CreatePoint(updated_edge_point);

            }
        }
    }

    bool LoopSolver::UpdateVertices(const Mesh& mesh,
                            std::vector<index_t>& updated_vertices) {
        const auto& vertices = mesh.Vertices();

        updated_vertices.resize(vertices.size());
        for(int i = 0 ;i < vertices.size() ; i++) {
            const std::shared_ptr<Vertex>& vertex = vertices[i];

            const Eigen::Vector3d& old_point = vertex->p;
            
            size_t degree = 0;

            // calculate the sum of neighbour vertices
            Eigen::Vector3d sum_assc_verices = Eigen::Vector3d::Zero();
            for(const auto& edge_id : vertex->associated_edges) {
                const std::shared_ptr<Edge>& edge = mesh.EdgeElement(edge_id);
                index_t id = edge->id1;
                if(id == i) {  // `i` is the id of current vertex
                    id = edge->id2;
                    if(id == i) {
                        //std::cerr <<"Warning: a point is included in edges.\n";
                        continue;
                    }
                }
                sum_assc_verices += mesh.VertexPoint(id);
                degree++;
            }
            bool b_boundary = degree < 3;

            // update vertex
            if(b_boundary) {
                Eigen::Vector3d updated_vertex = 0.125 * sum_assc_verices + 0.75 * old_point;
                updated_vertices[i] = CreatePoint(updated_vertex);
            } else {
                float beta = 0.0;
                if(degree == 3) {
                    beta = 0.185;
                } else {
                    beta = 3.0 / (8 * degree);
                }
                Eigen::Vector3d updated_vertex = beta * sum_assc_verices + (1 - degree * beta) * old_point;
                updated_vertices[i] = CreatePoint(updated_vertex);
            }

        }
        return true;
    }

    void LoopSolver::Divide(const Mesh& mesh,
                    const std::vector<index_t>& updated_edge_points,
                    const std::vector<index_t>& updated_vertices,
                    std::vector<std::vector<index_t>>& updated_polygons) {
        const auto& polygons = mesh.Polygons();
        for(int i = 0 ; i < polygons.size() ;i++) {
            const std::shared_ptr<Polygon>& polygon = polygons[i];
            const std::vector<index_t>& edges = polygon->edges;
            for(int j = 0 , sz = edges.size() ; j < sz; j++) {  //neighbour edges must have common vertex
                int edge_id1 = edges[j];
                int edge_id2 = edges[(j + 1)%sz];
                index_t vertex_id;
                if(Mesh::CommonVertex(mesh.EdgeElement(edge_id1), mesh.EdgeElement(edge_id2), vertex_id)) {
                    updated_polygons.emplace_back(std::vector<index_t>{updated_edge_points[edge_id1], updated_vertices[vertex_id],
                                                                        updated_edge_points[edge_id2]});
                } else {
                    //std::cerr << "neighbour edges are not continous.\n";
                }
            }
            assert(edges.size() == 3);
            updated_polygons.emplace_back(std::vector<index_t>{updated_edge_points[edges[0]], updated_edge_points[edges[1]],
                                                                        updated_edge_points[edges[2]]});
        }
    }


}