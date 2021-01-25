#pragma once
#include <vector>
#include <Eigen/Geometry>
#include <Eigen/Core>
#include <memory>
#include "utils/common_utils.h"


namespace SubDivision {
     using CommonUtils::index_t;

    struct Vertex{
        int id;  //Note: must consistent to its index
        Eigen::Vector3d p;
        std::vector<index_t> associated_edges;  //for updating vertex points
        std::vector<index_t> associated_polygons;  //for updating vertex points
    };
    
    struct Edge {
        index_t id1;
	    index_t id2;
        std::vector<index_t> associated_polygons;
    };

    struct Polygon {
        std::vector<index_t>  points; //for computing face points
        std::vector<index_t>  edges; //for connecting new face
        Eigen::Vector3d norm;

        size_t NumPoints() { return points.size();}
    };

    class Mesh {
public:
        void SetUp(const std::vector<Eigen::Vector3d>& vertices, 
                   const std::vector<std::vector<index_t>>& polygons);
        std::tuple<std::shared_ptr<std::vector<float>>, size_t> ConvertToTriangularMesh();

        bool CommonVertex(const Edge& e1, const Edge& e2, index_t& vertex_id);

        inline const std::shared_ptr<Edge> EdgeElement(const int i) const {return edges_.at(i);}
        inline const std::shared_ptr<Vertex> VertexElement(const int i) const {return vertices_.at(i);}
        inline const Eigen::Vector3d& VertexPoint(const int i) const {return vertices_.at(i)->p;}

        inline const std::shared_ptr<Polygon> polygonElement(const int i) const {return polygons_.at(i);}
        inline const std::vector<std::shared_ptr<Vertex>>& Vertices() const {return vertices_;}
        inline const std::vector<std::shared_ptr<Edge>>& Edges() const {return edges_;}
        inline const std::vector<std::shared_ptr<Polygon>>& Polygons() const {return polygons_;}
        void PrintPolygon();
        void PrintObj();

       

private:
         void ResetNorm();

        std::vector<std::shared_ptr<Vertex>> vertices_;
        std::vector<std::shared_ptr<Edge>> edges_;
        std::vector<std::shared_ptr<Polygon>> polygons_;


    };
}
