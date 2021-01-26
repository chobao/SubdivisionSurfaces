#include "catmull_solver.h"
namespace SubDivision {
    bool CatmullClarkSolver::Run(const Mesh& mesh, Mesh& updated_mesh) {
        std::vector<Eigen::Vector3d> edge_points = GetEdgePoints(mesh);  //mesh.edges <-> edge_points (index consistence)
        std::vector<Eigen::Vector3d> face_points = GetFacePoints(mesh);  //mesh.polygons <-> face_points (index consistence)
        std::vector<index_t> updated_face_points;  //mesh.polygons <-> updated_face_points (index consistence)
        std::vector<index_t> updated_edge_points; //mesh.edges <-> updated_edge_points (index consistence)
        std::vector<index_t> updated_vertices; //mesh.vertices <-> updated_vertices (index consistence)
        std::vector<std::vector<index_t>> polygons;
        if(verbose) {
            std::cout<<"edge_points:\n";
            CommonUtils::PrintEigenVector(edge_points);
            std::cout<<"face_points:\n";
            CommonUtils::PrintEigenVector(face_points);
        }
        


        UpdateFacePoints(face_points, updated_face_points);
        if(verbose) {
            std::cout<<"UpdateFacePoints:\n";
            PrintPointTable();
        }
        UpdateEdgePoints(mesh, face_points, updated_edge_points);
        if(verbose) {
            std::cout<<"UpdateEdgePoints:\n";
            PrintPointTable();
        }
        if(!UpdateVertices(mesh, face_points, edge_points, updated_vertices)) {
            return false;
        }
        if(verbose) {
            std::cout<<"UpdateVertices:\n";
            PrintPointTable();
        }
        Divide(mesh, updated_face_points, updated_edge_points, updated_vertices, polygons);
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

    void CatmullClarkSolver::UpdateFacePoints(const std::vector<Eigen::Vector3d>& face_points,
                                            std::vector<index_t>& updated_face_points)   {
        updated_face_points.resize(face_points.size());
        for(int i = 0; i < face_points.size() ; i++) {
            updated_face_points[i] = CreatePoint(face_points[i]);
        }
    }

    void CatmullClarkSolver::UpdateEdgePoints(const Mesh& mesh,
                    const std::vector<Eigen::Vector3d>& face_points,
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
                Eigen::Vector3d sum_face_pt = Eigen::Vector3d::Zero();
                //collect neighbour polygons(# should be 2)
                for(const auto& id : asso_polygons) {
                    sum_face_pt += face_points[id];
                }
                updated_edge_points[i] = CreatePoint((p1 + p2 + sum_face_pt) / 4.0);
            }
        }
    }

    bool CatmullClarkSolver::UpdateVertices(const Mesh& mesh, const std::vector<Eigen::Vector3d>& face_points,
                            const std::vector<Eigen::Vector3d>& edge_points,
                            std::vector<index_t>& updated_vertices) {
        const auto& vertices = mesh.Vertices();

        updated_vertices.resize(vertices.size());
        for(int i = 0 ;i < vertices.size() ; i++) {
            const std::shared_ptr<Vertex>& vertex = vertices[i];

            const Eigen::Vector3d& old_point = vertex->p;
            size_t num_asso_edges = vertex->associated_edges.size();
            size_t num_asso_polygons = vertex->associated_polygons.size();
            bool b_boundary = num_asso_edges != num_asso_polygons;

            //collect neighbour edge points
            Eigen::Vector3d edge_pt = Eigen::Vector3d::Zero();
            for(const auto& id : vertex->associated_edges) {
                edge_pt += edge_points[id];
            }
            edge_pt = edge_pt / num_asso_edges;


            if(b_boundary) {
                Eigen::Vector3d updated_vertex = edge_pt * 0.25 + old_point * 0.75;
                updated_vertices[i] = CreatePoint(updated_vertex);

            } else {
                //collect neighbour face points
                Eigen::Vector3d face_pt =  Eigen::Vector3d::Zero();
                for(const auto& id : vertex->associated_polygons) {
                    face_pt += face_points[id];
                }
                face_pt = face_pt / num_asso_polygons;


                if(num_asso_polygons < 3) {
                    std::cerr << "Error: There is line which may caused by level of subdivision is too high.\n";
                    return false;
                }
                Eigen::Vector3d updated_vertex = (face_pt + 2 * edge_pt + (num_asso_polygons - 3) * old_point)/ num_asso_polygons;

                updated_vertices[i] = CreatePoint(updated_vertex);
            }
        }
        return true;

    }

    void CatmullClarkSolver::Divide(const Mesh& mesh, const std::vector<index_t>& updated_face_points,
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
                    //four points may not be on a plane
                    // updated_polygons.emplace_back(std::vector<index_t>{updated_face_points[i], updated_edge_points[edge_id1],
                    //                                            updated_vertices[vertex_id], updated_edge_points[edge_id2] });
                      updated_polygons.emplace_back(std::vector<index_t>{updated_face_points[i], updated_edge_points[edge_id1],
                                                               updated_vertices[vertex_id]});
                      updated_polygons.emplace_back(std::vector<index_t>{updated_face_points[i],
                                                               updated_vertices[vertex_id], updated_edge_points[edge_id2] });
                } else {
                    std::cerr << "neighbour edges are not continous.\n";
                }
            }
        }
    }
}