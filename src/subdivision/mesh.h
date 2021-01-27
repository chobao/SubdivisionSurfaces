#pragma once
#include <vector>
#include <Eigen/Geometry>
#include <Eigen/Core>
#include <memory>
#include "utils/common_utils.h"
#include "base/model.h"

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
        bool FindNeighbourPolygon(const index_t& polygon_id1, index_t& polygon_id2) const{
                for(const auto& polygon_idi: associated_polygons) {
                    if(polygon_id1 != polygon_idi) {
                        polygon_id2 = polygon_idi;
                        return true;
                    }
                }
                return false;
        }
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
        std::tuple<std::vector<float>, size_t> ConvertToTriangularMesh();

        static bool CommonVertex(const std::shared_ptr<Edge>& e1, const std::shared_ptr<Edge>& e2, index_t& vertex_id) {
            if(e1->id1 == e2->id1 || e1->id1 == e2->id2) {
                vertex_id = e1->id1;
                return true;
            } else if(e1->id2 == e2->id1 || e1->id2 == e2->id2) {
                vertex_id = e1->id2;
                return true;
            }
            return false;
        }

        inline const std::shared_ptr<Edge> EdgeElement(const int i) const {return edges_.at(i);}
        inline const std::shared_ptr<Vertex> VertexElement(const int i) const {return vertices_.at(i);}
        inline const Eigen::Vector3d& VertexPoint(const int i) const {return vertices_.at(i)->p;}

        inline const std::shared_ptr<Polygon> PolygonElement(const int i) const {return polygons_.at(i);}
        inline const std::vector<std::shared_ptr<Vertex>>& Vertices() const {return vertices_;}
        inline const std::vector<std::shared_ptr<Edge>>& Edges() const {return edges_;}
        inline const std::vector<std::shared_ptr<Polygon>>& Polygons() const {return polygons_;}
        void PrintPolygon();
        void PrintObj();
        void FeedNorm(const Model& model);

        inline bool FindEdge(const index_t& polygon_id, const index_t& start_index,
                             const index_t& vertex_id1,const index_t& vertex_id2,
                             index_t& edge_id) const { // find the edge whose start points is vertex1 and end point is vertex 2 in the polygon
            const std::shared_ptr<Polygon>& polygon = polygons_[polygon_id];
            const auto& polygon_edges = polygon->edges;
            for(int j = 0 ,sz = polygon_edges.size(); j < sz ; j++) {
                int index = (j + start_index) % sz;
                const std::shared_ptr<Edge>& edge  = EdgeElement(polygon_edges[index]);
                if((edge->id1 == vertex_id1 && edge->id2 == vertex_id2) || 
                    (edge->id1 == vertex_id2 && edge->id2 == vertex_id1)) {
                        edge_id = polygon_edges[index];
                        return true;
                }
            }
            return false;
        }

       

private:
         void ResetNorm();

        std::vector<std::shared_ptr<Vertex>> vertices_;
        std::vector<std::shared_ptr<Edge>> edges_;
        std::vector<std::shared_ptr<Polygon>> polygons_;
        const bool verbose = false;


    };
}
