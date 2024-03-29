#include "mesh.h"
#include <unordered_map>
#include <map>
#include <cmath>

namespace SubDivision {

    void Mesh::SetUp(const std::vector<Eigen::Vector3d>& vertices, 
                   const std::vector<std::vector<index_t>>& polygons) {
        
        /////////////////////////
        //establish vertex table
        /////////////////////////
        vertices_.reserve(vertices.size());
        for(int i = 0 ; i < vertices.size() ; i++) {
            auto vertex = std::make_shared<Vertex>();
            vertex->id = i;
            vertex->p = vertices[i];
            vertices_.emplace_back(vertex);
        }
        /////////////////////////
        //establish edge table and polygon table
        /////////////////////////
        polygons_.reserve(polygons.size());
        std::map<std::pair<index_t,index_t>, size_t> edge_pairs;
        for(size_t i = 0 ; i < polygons.size() ; i++) {
            const std::vector<index_t>& polygon_i = polygons[i];
            auto res_polygon = std::make_shared<Polygon>();

            //insert vertices
            res_polygon->points.resize(polygon_i.size());
            for(int j = 0 ,sz = polygon_i.size(); j < sz ; j++) {
                res_polygon->points[j] = polygon_i[j]; // alternative
            }
            
            //create and insert edges
            res_polygon->edges.reserve(polygon_i.size());
            for(int j = 0 ,sz = polygon_i.size(); j < sz ; j++) {
                int nxt = (j + 1) % sz;

                std::pair<index_t, index_t> edge_pair(polygon_i[j], polygon_i[nxt]);
                if(edge_pair.first > edge_pair.second) {
                    std::swap(edge_pair.first, edge_pair.second);
                }
                if(verbose && edge_pair.first >= edge_pair.second) {
                    //std::cerr<<"edge_pair  "<<edge_pair.first<<", "<<edge_pair.second<<"\n";
                }
                assert(edge_pair.first < edge_pair.second);

                //create edge if necessary
                if(!edge_pairs.count(edge_pair)) { 
                    edge_pairs[edge_pair] = edges_.size();
                    auto edge = std::make_shared<Edge>();
                    edge->id1 = edge_pair.first;  // alternative
                    edge->id2 = edge_pair.second;  // alternative
                    edges_.emplace_back(edge);
                }
                index_t edge_id = edge_pairs.at(edge_pair);
                res_polygon->edges.emplace_back(edge_id);  // alternative
            }

            polygons_.emplace_back(res_polygon);
   
          
        }

        /////////////////////////
        //establish association between vertex & edge table and polygon table
        /////////////////////////

        for(size_t i = 0 ; i < polygons_.size() ; i++) {
            const auto& polygon = polygons_[i];
            for(auto& vertex: polygon->points) {
                vertices_[vertex]->associated_polygons.emplace_back(i); // alternative
            }

             for(auto& edge: polygon->edges) {
                edges_[edge]->associated_polygons.emplace_back(i); // alternative
            }
        }

        /////////////////////////
        //establish association between vertex and edge table
        /////////////////////////

        for(size_t i = 0 ; i < edges_.size() ; i++) {
            const auto& edge = edges_[i];
            vertices_[edge->id1]->associated_edges.emplace_back(i); // alternative
            vertices_[edge->id2]->associated_edges.emplace_back(i); // alternative
        }

        ResetNorm();
    }

    void Mesh::ResetNorm() {

        for(int i = 0 ; i < polygons_.size() ; i++) {
            const size_t num_points = polygons_[i]->NumPoints();
            polygons_[i]->norm = Eigen::Vector3d::Zero();
            if(num_points <= 2) {
                std::cout<<"Warning: line exists in mesh.\n";
                continue;
            }
            const Eigen::Vector3d& p1 = VertexPoint(polygons_[i]->points[0]);
            const Eigen::Vector3d& p2 = VertexPoint(polygons_[i]->points[1]);
            const Eigen::Vector3d& p3 = VertexPoint(polygons_[i]->points[2]);
            polygons_[i]->norm = ((p2 - p1).cross(p3 - p1)).normalized();
        }
    }

    // void Mesh::FeedNorm(const Model& model) {

    //     for(int i = 0 ; i < polygons_.size() ; i++) {
            
    //         const Eigen::Vector3d plane_norm = model.PlaneElement(i).Norm();
    //         double norm1 = polygons_[i]->norm.norm();
    //         double norm2 = plane_norm.norm();
    //         std::cout<<i<<" ";
    //         std::cout<<"polygons_ :"<<polygons_[i]->norm.transpose()<<"\n";
    //         std::cout<<"plane_norm :"<<plane_norm.transpose()<<"\n";
    //         std::cout<<std::fixed<<std::setprecision(5)<<"polygons_[i]->norm.dot(plane_norm):"<<(double)polygons_[i]->norm.dot(plane_norm)<<"\n";
    //         std::cout<<std::fixed<<std::setprecision(5)<<"norm multiplication:"<<(double)(norm1 * norm2)<<"\n";

    //         double degree = polygons_[i]->norm.dot(plane_norm) / (norm1 * norm2);
    //         std::cout<<"degree: "<<degree<<"\n";
    //         std::cout<<i<<" "<<std::acos(degree) / 180.0 * M_PI <<"\n";
    //         //polygons_[i]->norm = plane_norm;
    //     }
    // }

    std::tuple<std::vector<float>, size_t> Mesh::ConvertToTriangularMesh() {
    
        std::vector<float> triangular_mesh;
        
        //collect data size
        size_t num_mesh = 0;
        for(int i = 0 ; i < polygons_.size() ; i++) {
            const size_t num_points = polygons_[i]->NumPoints();
            if(num_points <= 2) {
                std::cout<<"Warning: line exists in mesh.\n";
                continue;
            }
            num_mesh += num_points - 2;
        }
        //reserve vector size
        triangular_mesh.reserve(num_mesh * 27);

        //push vertex and color data by order `p(0) p(1) p(1) n(0) n(1) n(2) r g b`
        auto func = [&](const Eigen::Vector3d& p, const Eigen::Vector3d& norm){
            for(int k = 0 ; k < 3 ; k++) {
                triangular_mesh.emplace_back(p(k));
            }
            for(int k = 0 ; k < 3 ; k++) {
                triangular_mesh.emplace_back(norm(k));
            }
            triangular_mesh.emplace_back(0.0f);
            triangular_mesh.emplace_back(0.0f);
            triangular_mesh.emplace_back(0.0f);

        };

        size_t num_vertex = 0;
        for(int i = 0 ; i < polygons_.size() ; i++) {
            if(polygons_[i]->NumPoints() <= 2) {
                std::cout<<"Warning: line exists in mesh.\n";
                continue;
            }
            const std::vector<index_t>& points = polygons_[i]->points;

            const Eigen::Vector3d& p1 = VertexPoint(points[0]);
            for(int j = 1 ,sz = points.size() - 1; j < sz ;j++) {
                const Eigen::Vector3d& p2 = VertexPoint(points[j]);
                const Eigen::Vector3d& p3 = VertexPoint(points[j + 1]);
                Eigen::Vector3d norm = ((p2 - p1).cross(p3 - p1)).normalized();  // exists a nonplane-polygon 
                //push vertex
                func(p1, norm);
                func(p2, norm);
                func(p3, norm);
                num_vertex += 3;
            }
        }

        if(verbose) {
            std::cout<<"num_mesh: "<<num_mesh<<" , "<<"num_vertex: "<<num_vertex<<", "<<"triangular_mesh.size(): "<<triangular_mesh.size()<<"\n";
        }

        return {triangular_mesh, num_vertex};

    }

    void Mesh::PrintObj() {
        for(int i = 0 ; i < vertices_.size() ; i++) {
            std::cout<<i<<": ";
            std::cout<<vertices_[i]->p.transpose();
            std::cout<<"\n";
        }
        for(int i = 0 ; i < polygons_.size() ; i++) {
            std::cout<<i<<": ";
            for(const auto id: polygons_[i]->points) {
                const Eigen::Vector3d& p = VertexPoint(id);
                std::cout<<id<<" ";
            }
            std::cout<<"\n";
        }
    }

    void Mesh::PrintPolygon() {
        for(int i = 0 ; i < polygons_.size() ; i++) {
            const size_t num_points = polygons_[i]->points.size();
            std::cout<<i<<": ";
            for(const auto id: polygons_[i]->points) {
                const Eigen::Vector3d& p = VertexPoint(id);
                std::cout<<"("<<id<<"): ";
                std::cout<<p.transpose();
                std::cout<<"\n";
            }
            std::cout<<"norm:"<<polygons_[i]->norm.transpose();
            std::cout<<"\n";

        }
    }

    void Mesh::ScaleModel(const float max_scale) {
        
        // collect the center and scale of model
        Eigen::Vector3d model_center = Eigen::Vector3d::Zero();
        double max_x = -GeometryUtils::MILLION, max_y = - GeometryUtils::MILLION;
        double min_x = GeometryUtils::MILLION, min_y = GeometryUtils::MILLION;
        size_t num_vertexes = 0;
        
        for(const auto& vertex : vertices_) {
            const auto& point = vertex->p;
            model_center += point;

            max_x = max_x < point.x() ? point.x() : max_x;
            min_x = min_x > point.x() ? point.x() : min_x;
            max_y = max_y < point.y() ? point.y() : max_y;
            min_y = min_y > point.y() ? point.y() : min_y;

            num_vertexes++;
        }
        
        model_center /= num_vertexes;
        double max_radius = 0.0;
        for(const auto& vertex : vertices_) {
            double radius = (vertex->p - model_center).norm();
            max_radius = max_radius < radius ? radius : max_radius;
        }

        // Align model to screen frame
        Eigen::Vector3d translation_3d(0.0, 0.0, 0.0);

        double scale = max_scale / max_radius;

        for(auto& vertex : vertices_) {
            auto& point = vertex->p;
            point= (point - model_center) * scale + translation_3d;  //scale
        }

    }
}