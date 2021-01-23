// This file is part of Occlusion_Culling
// Author: Bao Chong
// Date:2020/11

#include "model.h"
#include <fstream>
#include <iostream>
//#define DEBUG_INPUT
#define DEBUG


bool Model::LoadObj(const std::string& file_path) {

    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "fail to open file " + file_path << std::endl;
        return false;
    }
    std::string type;
    int face_cnt = 0, vertex_cnt = 0;

    std::vector<Eigen::Vector3d> m_vertexes;

    while (file >> type) {
        if (type == "v") {
            // vertex
            Eigen::Vector3d vt;
            file >> vt(0) >> vt(1) >> vt(2);
            m_vertexes.push_back(vt);
        } else if (type == "f") {
            
            std::string token;
            std::vector<int> index_v;
            std::string line;
            getline(file, line);
            std::stringstream ss(line);
            while(getline(ss, token, ' '))
            {
                if (token.size() == 0 || token[0] == '\r' || !(token[0] >= '0'&&token[0]<='9') ) continue;
#ifdef DEBUG_INPUT
                std::cout<<"DEBUG(BCHO): LoadObj(): token:"<<token<<"("<<std::stoi(token) - 1<<")"<<std::endl;
#endif
                index_v.push_back(std::stoi(token) - 1);
            }

            if (index_v.size() > 2) {
#ifdef TRIANGULAR
                for(int i = 0 ; i < index_v.size() ; i+=2) {
                    // int nxt_i = (i + 1)%(index_v.size());
                    int nxt_i = i + 1;
                    if(nxt_i >= index_v.size()) {
                        continue;
                    }
                    int after_nxt_i = (i + 2)%(index_v.size());

                    Eigen::Vector3d &a = m_vertexes[index_v[i]],
                         &b = m_vertexes[index_v[nxt_i]], &c = m_vertexes[index_v[after_nxt_i]];
                    // Vec3f &normal = Normalize(Cross(b - a, c - b));

                    // face.m_normal = normal;
                    // for (auto &i : vertex_idx) {
                    //     face.m_vertexes.push_back(m_vertexes[i]);
                    //     vertex_cnt++;
                    // }
                    planes_.push_back(Plane(a,b,c));
                    vertex_cnt += 3;
                    face_cnt++;
                }
#else
                std::vector<Eigen::Vector3d> vertexes;
                for(int i = 0 ; i < index_v.size() ; i++) {
                    vertexes.emplace_back(m_vertexes[index_v[i]]);
                }
                planes_.push_back(vertexes);
                vertex_cnt += vertexes.size();
                face_cnt++;
#endif 
            }
        }
    }
    file.close();
    
    ConstructRandomColors();
    std::cout << "Model has " << face_cnt << " faces and " << vertex_cnt << " vertexes"
         << std::endl;
    return true;
}

void Model::ConstructRandomColors() {
    colors_.resize(planes_.size());

    for(size_t i = 0 ; i < colors_.size() ; i++) {
        colors_[i] = Color(RandInt(0,255),RandInt(0,255),RandInt(0,255));
        //colors_[i] = Color(0,0,0);
    }
}

void Model::SetShaderColor(const int width, const int height) {
    auto face_num = planes_.size();
    auto light_color = Eigen::Vector3d(0.4, 0.4, 0.0);
    auto ambient_color = Eigen::Vector3d(0.1, 0.1, 0.0);
    auto light_position = Eigen::Vector3d(width / 2, height / 2, 10000.0f);

    auto clamp = [] (double x){
        return std::min(1.0, std::max(0.0, x));
    };

    for(int i = 0 ; i < planes_.size() ;i++) {
        Eigen::Vector3d color = Eigen::Vector3d(0.0, 0.0, 0.0);
        const Eigen::Vector3d& normal = planes_[i].Norm();
        int num_points = planes_[i].NumPoints();
        for (int j = 0; j < num_points ; j++) {
            Eigen::Vector3d vertex = planes_[i].Point(j);
            Eigen::Vector3d ray_direction = (light_position - vertex).normalized();
            float cosine = std::abs(ray_direction.dot(normal));
            color += cosine * light_color;
            color += ambient_color;
        }

        color /= num_points;
        color(0) = clamp(color(0));
        color(1) = clamp(color(1));
        color(2) = clamp(color(2));
        colors_[i] = Color(color(0) * 255, color(1) * 255, color(2) * 255);
    }
}

void Model::TransformationOntoScreen(uint width, uint height, Eigen::Vector3d& screen_model_center) {
    
    // collect the center and scale of model
    Eigen::Vector3d model_center = Eigen::Vector3d::Zero();
    double max_x = -GeometryUtils::MILLION, max_y = - GeometryUtils::MILLION;
    double min_x = GeometryUtils::MILLION, min_y = GeometryUtils::MILLION;
    size_t num_vertexes = 0;
    for(const auto plane_i : planes_) {
        // for(int j = 0 ; j < plane_i.NumPoints(); j++) {
        //     Eigen::Vector3d point_xyz =  plane_i.Point(j);
        //     model_center += point_xyz;
        //     max_x = max_x < point_xyz.x() ? point_xyz.x() : max_x;
        //     min_x = min_x > point_xyz.x() ? point_xyz.x() : min_x;
        //     max_y = max_y < point_xyz.y() ? point_xyz.y() : max_y;
        //     min_y = min_y > point_xyz.y() ? point_xyz.y() : min_y;
            
        //     num_vertexes++;
        // }
        for(const auto& point : plane_i.Points()) {

            model_center += point;

            max_x = max_x < point.x() ? point.x() : max_x;
            min_x = min_x > point.x() ? point.x() : min_x;
            max_y = max_y < point.y() ? point.y() : max_y;
            min_y = min_y > point.y() ? point.y() : min_y;

            num_vertexes++;
        }
    }
    model_center /= num_vertexes;
    double max_radius = 0.0;
    for(const auto plane_i : planes_) {
        for(const auto& point : plane_i.Points()) {
            double radius = (point - model_center).norm();
            max_radius = max_radius < radius ? radius : max_radius;
        }
    }

    // Align model to screen frame
    Eigen::Vector3d translation_3d(width / 2.0, height / 2.0, 0.0);

    double scale = std::min(width, height) * 0.5 / max_radius;

    for(size_t i = 0 ; i < planes_.size() ; i++) {
        std::vector<Eigen::Vector3d> new_vertexes(planes_[i].NumPoints());
        for(int j = 0 ; j < planes_[i].NumPoints() ; j++) {
            new_vertexes[j] = (planes_[i].Point(j) - model_center) * scale + translation_3d;  //scale
            new_vertexes[j](1) = height - new_vertexes[j](1); //flip the model
        }
        planes_[i] = Plane(new_vertexes);
    }

    screen_model_center = translation_3d;

}

// std::shared_ptr<std::vector<float>> Model::ConvertToTriangularMesh() {
//     std::shared_ptr<std::vector<float>> triangular_mesh;
    
//     //collect data size
//     for(int i = 0 ; i < planes_.size() ; i++) {
        
//     }

//     //reserve vector size

//     //push vertex and color data by order `p1 p2 p3 r g b`

// }
