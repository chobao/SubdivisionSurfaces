#include "doo_solver.h"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

namespace SubDivision {
    bool DooSabinSolver::Run(const Mesh& mesh, Mesh& updated_mesh) {
        std::vector<Eigen::Vector3d> edge_points = GetEdgePoints(mesh);  //mesh.edges <-> edge_points (index consistence)
        std::vector<Eigen::Vector3d> face_points = GetFacePoints(mesh);  //mesh.polygons <-> face_points (index consistence)
        std::vector<std::vector<index_t>> updated_polygons;
        std::vector<std::vector<index_t>> updated_edge_polygons;
        std::vector<std::vector<index_t>> updated_vertex_polygons;
        std::vector<std::vector<index_t>> polygons;
        std::map<index_t,std::map<index_t, index_t>> indexed_vertices;
        if(verbose) {
            std::cout<<"edge_points:\n";
            CommonUtils::PrintEigenVector(edge_points);
            std::cout<<"face_points:\n";
            CommonUtils::PrintEigenVector(face_points);
        }
        

        if(!UpdateVertices(mesh, face_points, edge_points, indexed_vertices,
               updated_vertex_polygons)) {
            return false;
        }
        if(verbose) {
            std::cout<<"UpdateVertices vertex table:\n";
            PrintPointTable();
            std::cout<<"UpdateVertices polygon vertices:\n";
            for(int i = 0 ; i < updated_vertex_polygons.size() ; i++) {
                std::vector<index_t> out_poly(updated_vertex_polygons[i]);
                for(int j = 0; j < out_poly.size() ; j++) {
                    out_poly[j]++;
                }
                CommonUtils::Print<index_t>(out_poly);
            }
            
            

        }

        UpdatedEdgePolygon(mesh, indexed_vertices,updated_edge_polygons);
        if(verbose) {
            std::cout<<"UpdatedEdgePolygon edge vertices:\n";
            for(int i = 0 ; i < updated_edge_polygons.size() ; i++) {
                std::vector<index_t> out_poly(updated_edge_polygons[i]);
                for(int j = 0; j < out_poly.size() ; j++) {
                    out_poly[j]++;
                }
                CommonUtils::Print<index_t>(out_poly);
            }
        }

        UpdateVertexPolygon(mesh, indexed_vertices,updated_vertex_polygons);
        if(verbose) {
            std::cout<<"UpdateVertexPolygon vertex vertices:\n";
            for(int i = 0 ; i < updated_vertex_polygons.size() ; i++) {
                std::vector<index_t> out_poly(updated_vertex_polygons[i]);
                for(int j = 0; j < out_poly.size() ; j++) {
                    out_poly[j]++;
                }
                CommonUtils::Print<index_t>(out_poly);
            }
        }


        
        Divide(mesh,updated_polygons,updated_edge_polygons,updated_vertex_polygons,
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
                            std::map<index_t,std::map<index_t, index_t>>& indexed_vertices,
                            std::vector<std::vector<index_t>>& updated_polygons) {
        const auto& polygons = mesh.Polygons();
        updated_polygons.resize(polygons.size());

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
                const std::shared_ptr<Vertex>& vertex = mesh.VertexElement(vertex_id);
                index_t updated_vertex_id;
                //speical care for  boundary vertex
                if(b_solve_boundary && vertex->associated_polygons.size() != vertex->associated_edges.size()) {
                    if(processed_boundary_vertex_.count(vertex_id)) {
                        updated_vertex_id = processed_boundary_vertex_.at(vertex_id);
                    } else {
                        updated_vertex_id = CreatePoint(vertex->p);
                        processed_boundary_vertex_[vertex_id] = updated_vertex_id;
                    }
                } else {
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
                    updated_vertex_id = CreatePoint(updated_vertices);
                }
                
                

                // link the used primitives with updated vertex
                updated_polygons[i].emplace_back(updated_vertex_id); //ordered
                indexed_vertices[i][vertex_id] = updated_vertex_id;
            }
            
        }
        return true;
    }

    bool DooSabinSolver::UpdatedEdgePolygon(const Mesh& mesh, const std::map<index_t,std::map<index_t, index_t>>& indexed_vertices,
                                std::vector<std::vector<index_t>>& updated_edge_polygons) {
        const auto& polygons = mesh.Polygons();
        std::vector<bool> edge_status(mesh.Edges().size(), false);
        //traverse polygon to comfirm the clockwise edge polygon
        for(index_t polygon_id = 0; polygon_id < polygons.size() ; polygon_id++) {
            const std::shared_ptr<Polygon>& polygon = polygons[polygon_id];
            const auto& points = polygon->points;
            const auto& polygon_edges = polygon->edges;
            for(int i = 0 ,sz = points.size(); i < sz ; i++) {
                index_t vertex_id1 = points[i];
                index_t vertex_id2 = points[(i+1)%sz];
                //seach corresponding edge id
                index_t edge_id;
                if(!mesh.FindEdge(polygon_id, i, vertex_id1, vertex_id2, edge_id)) {
                    std::cerr<<"Error: None corresponding edge between ("<<vertex_id1<<" -> "<<vertex_id2<<" is found.\n";
                    break;
                }
                if(edge_status[edge_id]) {
                    continue;
                }

                std::shared_ptr<Edge> edge = mesh.EdgeElement(edge_id);
                
                
                if(edge->associated_polygons.size() < 2) {
                    continue;
                }
                //seach neighbour polygon of current polygon touching current edge
                index_t polygon_id2 = polygon_id;
                if(!edge->FindNeighbourPolygon(polygon_id, polygon_id2)) {
                    std::cerr <<"Error: none polygon near current polygon touches edge "<<edge_id<<" in `UpdatedEdgePolygon` \n";
                    break;
                }
                edge_status[edge_id] = true;
                //place points in order consistent to that of input polygon
                index_t id21 = indexed_vertices.at(polygon_id2).at(vertex_id1);
                index_t id22 = indexed_vertices.at(polygon_id2).at(vertex_id2);
                index_t id12 = indexed_vertices.at(polygon_id).at(vertex_id2);
                index_t id11 = indexed_vertices.at(polygon_id).at(vertex_id1);
                if(id22 == id12) { // caused by processing boundary case i.e. vertex 2 is boundary point
                    updated_edge_polygons.emplace_back(std::vector<index_t>{
                                                                        id21,
                                                                        id12,
                                                                        id11});
                } else if(id21 == id11) { // caused by processing boundary point
                    updated_edge_polygons.emplace_back(std::vector<index_t>{id21,
                                                                        id22,
                                                                        id12});
                } else {
                     updated_edge_polygons.emplace_back(std::vector<index_t>{id21,
                                                                        id22,
                                                                        id12,
                                                                        id11});
                }
               

            }
        }

        return true;
    }

    bool DooSabinSolver::Divide(const Mesh& mesh,
                const std::vector<std::vector<index_t>>& updated_polygons,
                const std::vector<std::vector<index_t>>& updated_edge_polygons,
                const std::vector<std::vector<index_t>>& updated_vertex_polygons,
                std::vector<std::vector<index_t>>& polygons) {
        for(const auto& vertices :updated_polygons) {
            //generate face 	
            if(vertices.size() >= 3) {
                polygons.emplace_back(vertices);
            }
        }
        for(const auto& vertices: updated_edge_polygons) {
            if(vertices.size() >= 3) {
                polygons.emplace_back(vertices);
            }
        }
        for(const auto& vertices: updated_vertex_polygons) {
            if(vertices.size() >= 3) {
                polygons.emplace_back(vertices);
            }
            
        }
        return true;
    }

    bool DooSabinSolver::UpdateVertexPolygon(const Mesh& mesh, const std::map<index_t,std::map<index_t, index_t>>& indexed_vertices,
                                std::vector<std::vector<index_t>>& updated_vertex_polygons) {
        const auto& vertices = mesh.Vertices();
        for(int vertex_id = 0 ; vertex_id < vertices.size() ; vertex_id++) {
            const std::shared_ptr<Vertex>& vertex = vertices[vertex_id];
            //speical care for  boundary vertex
            if(b_solve_boundary && vertex->associated_polygons.size() != vertex->associated_edges.size()) {
                continue;
            }
            const auto& asso_polygon = vertex->associated_polygons;
            // choose the first associated polyon of vertex as beginning
            index_t polygon_id = asso_polygon[0]; 
            // store the travelling sequence of associated polygons
            std::vector<index_t> ordered_polygons;
            ordered_polygons.reserve(asso_polygon.size());
            // check the correctness of data
            std::unordered_set<index_t> vis_polygon;
            vis_polygon.reserve(asso_polygon.size());
            
            bool b_succ = true;
            size_t last_edge_id;
            for(int j = 0 ; j < asso_polygon.size() ; j++) {
                
                if(vis_polygon.count(polygon_id)) {
                    std::cerr<<"Error: a unexpected loop in vertex associated_polygons\n";
                    b_succ = false;
                    break;
                }
                vis_polygon.insert(polygon_id);
                ordered_polygons.emplace_back(polygon_id);
                ////
                //find the common edge of next asscoiated polygon near current associated polygon
                ////
                const std::shared_ptr<Polygon>& cur_asso_polygon = mesh.PolygonElement(polygon_id);
                const auto& points = cur_asso_polygon->points;
                //search the index of vertex  in point list of associated polygon
                auto it = std::find (points.begin(), points.end(), vertex_id);
                if (it == points.end()) {
                    std::cerr<<"Error: can not find vertex in associated polygon\n";
                    b_succ = false;
                    break;
                }
                int index = std::distance(points.begin(), it);
                assert(points[index] == vertex_id);
                // find edge whose end point is vertex in associated polygon
                const size_t num_points = cur_asso_polygon->points.size();
                int prev_index = (index - 1 + num_points) % num_points;
                //seach common edge id
                index_t edge_id;

                if(ordered_polygons.size() == 1) {
                    //assume that first edge starting from vertex in the polygon, the second edge should end at the vertex in the polygon
                    if(!mesh.FindEdge(polygon_id, prev_index, points[prev_index], points[index], edge_id)) {
                        std::cerr<<"Error: None corresponding edge between ("<<points[prev_index]<<" -> "<<points[index]<<" is found.\n";
                        break;
                    }
                    last_edge_id = edge_id;
                }
                else {
                    if(!mesh.FindEdge(polygon_id, prev_index, points[prev_index], points[index], edge_id)) {
                        std::cerr<<"Error: None corresponding edge between ("<<points[prev_index]<<" -> "<<points[index]<<" is found.\n";
                        break;
                    }
                    //select the edge difference from last edge in the polygon
                    if(edge_id == last_edge_id) {
                        int nxt_index = (index + 1)% num_points;
                        if(!mesh.FindEdge(polygon_id, index, points[index], points[nxt_index], edge_id)) {
                            std::cerr<<"Error: None corresponding edge between ("<<points[prev_index]<<" -> "<<points[index]<<" is found.\n";
                            break;
                        }
                        if(edge_id == last_edge_id) {
                            std::cerr<<"Error: No more edges are difference withlast edge ("<<last_edge_id<<" .\n";
                            break;
                        }
                    }
                    last_edge_id = edge_id;
                }
                
                std::shared_ptr<Edge> edge = mesh.EdgeElement(edge_id);
                //seach neighbour polygon of current polygon both touching current edge
                if(edge->associated_polygons.size() < 2) {  //solve for boundary where should stop
                    if(j + 1 != asso_polygon.size()) {
                        std::cout<<"Error: searching in associated polygons is stopped by boundary unexpectly.\n";
                    }
                    break;
                }
                index_t polygon_id2;
                if(!edge->FindNeighbourPolygon(polygon_id, polygon_id2)) {
                    std::cerr <<"Error: none polygon near current polygon touches edge "<<edge_id<<"in `UpdateVertexPolygon`\n";
                    break;
                }
                polygon_id = polygon_id2;
            }
            if(b_succ) {
                std::vector<index_t> ordered_polygon_vertex;
                ordered_polygon_vertex.reserve(ordered_polygons.size());
                for(const auto& polygon_id : ordered_polygons) {
                    ordered_polygon_vertex.emplace_back(indexed_vertices.at(polygon_id).at(vertex_id));
                }
                updated_vertex_polygons.emplace_back(ordered_polygon_vertex);
            }
        }
        return true;
    }
}