#pragma once
#include "mesh.h"


namespace SubDivision { 

    class SubDivisionSolver {
    public:
        std::vector<index_t> GetEdgePoints(const Mesh& mesh);
        std::vector<index_t> GetFacePoints(const Mesh& mesh);
        
        inline const Eigen::Vector3d& PointElement(index_t i) {return  point_table_.at(i);} 
        inline index_t CreatePoint(const Eigen::Vector3d& p) {
            point_table_.emplace_back(p);
            return point_table_.size() - 1;
        } 

        void MakeUpMesh(std::vector<std::vector<index_t>>& polygons,
                        Mesh& updated_mesh); //order polygons in clockwise


    protected:
        std::vector<Eigen::Vector3d> point_table_;
    };
}