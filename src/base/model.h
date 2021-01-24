// This file is part of Occlusion_Culling
// Author: Bao Chong
// Date:2020/11
#pragma once

#ifndef OCCLUSION_CULLING_MODEL_H
#define OCCLUSION_CULLING_MODEL_H

#include "utils/geometry_utils.h"
#include "utils/common_utils.h"
#include <memory>

using GeometryUtils::Plane;
using namespace CommonUtils;


class Model {
public:
    typedef std::shared_ptr<Model> Ptr;

    Model(){}
    static Model::Ptr CreateModelPtr() {
            return Model::Ptr(new Model());
    }

    bool LoadObj(const std::string& file_path);
    void TransformationOntoScreen(uint width, uint height, Eigen::Vector3d& screen_model_center);
    void ConstructRandomColors();
    void SetShaderColor(const int width, const int height);
    
    inline const std::vector<Plane>& Planes() const { return planes_; }
    inline const size_t NumPlanes() const { return planes_.size(); }
    inline const Plane& PlaneElement(size_t i) const { return planes_[i];}
    inline const std::vector<Color>& Colors() const { return colors_; }
    inline const std::vector<Color>* ConstColorPtr() const { return const_cast<const std::vector<Color>*>(&colors_); /*return &colors_;*/ }
    inline const Color& ColorElement(size_t i) const { return colors_[i]; }

    void Print() const {
        for(int i = 0 ; i < planes_.size() ; i++) {
            //planes_.Print();
            std::cout<<"Plane "<<i<<"\n";
            for(int j = 0 ; j < planes_[i].NumPoints() ; j++) {
                std::cout<<"p("<<j<<"): "<<std::fixed<<std::setprecision(9) <<planes_[i].PointElement(j).transpose()<<"\n";
            }
        }
    }
    
    std::tuple<std::shared_ptr<std::vector<float>>, size_t> ConvertToTriangularMesh();


private:
    std::vector<Plane> planes_;
    std::vector<Color> colors_;
    
};

#endif