#pragma once
#include "mesh.h"


namespace SubDivision { 

    class SubDivisionSolver {
    public:
        std::vector<Eigen::Vector3d> GetEdgePoints(const Mesh& mesh);
        std::vector<Eigen::Vector3d> GetFacePoints(const Mesh& mesh);
        
        inline const Eigen::Vector3d& PointElement(index_t i) {return  point_table_.at(i);} 
        inline index_t CreatePoint(const Eigen::Vector3d& p) {
            point_table_.emplace_back(p);
            return point_table_.size() - 1;
        } 

        void MakeUpMesh(std::vector<std::vector<index_t>>& polygons,
                        Mesh& updated_mesh); //order polygons in clockwise
        

        void PrintPointTable() {
            for(int i = 0 ; i < point_table_.size() ; i++) {
                std::cout<<"("<<i<<")"<<point_table_[i].transpose()<<"\n";
            }
        }

        virtual bool Run(const Mesh& mesh, Mesh& updated_mesh) = 0;


    protected:
        std::vector<Eigen::Vector3d> point_table_;
    };
}