//
// Created by baochong on 2020/7.
// Modified in 2020/11.
//
#pragma once

#ifndef MY_GEOMETRY_UTILS_H
#define MY_GEOMETRY_UTILS_H

#include <cstdio>
#include <iomanip>
#include <vector>
#include <Eigen/Geometry>
#include <Eigen/Core>
#include <tuple>
#include <iostream>
//#define TRIANGULAR
#include "common_utils.h"



namespace GeometryUtils
{
    const double PI = 3.1415926;
    const double MILLION = 1000000;
    const double EPS = 1e-5;

    inline int SignOfDouble(double x) {
        if(x <= EPS && x >= -EPS) {
            return 0;
        }
        else if(x >= EPS) {
            return 1;
        }
        else {
            return -1;
        }
    }

    namespace Geometry2D {
        struct TrivialPoint {
            double x_coord,y_coord;
            double x() const {return x_coord;}
            double y() const {return y_coord;}
            TrivialPoint(){}
            TrivialPoint(const double xx, const double yy):x_coord(xx), y_coord(yy){}
            TrivialPoint(const Eigen::Vector2d& p1):x_coord(p1.x()), y_coord(p1.y()){}
        };

        struct Line {
            TrivialPoint p1,p2;
            Line(const TrivialPoint& p1i, const TrivialPoint& p2i):
                p1(p1i), p2(p2i){}

            double x1()const {return p1.x();}
            double y1()const {return p1.y();}
            double x2()const {return p2.x();}
            double y2()const {return p2.y();}
            std::tuple<double,double> LineVector() const{
                double l1x = p2.x() - p1.x();
                double l1y = p2.y() - p1.y();
                return std::make_tuple(l1x,l1y);
            }

            double Cross(const Line& line2) const{
                double l1x ,l1y;
                double l2x, l2y;
                std::tie(l1x, l1y) = this->LineVector();
                std::tie(l2x, l2y) = line2.LineVector();
                return l1x * l2y - l1y * l2x;
            }
        };
        bool IsRayCross(double x, double y, const Line &l);
        double SolveParameter(const double x, const double l1, const double l2);
        double IsInSegment(const double x, const double y, const Line& line);
        bool IsPointInLine(const Line& line, const TrivialPoint& point);
        bool IsPointInSegment(const Line& line, const TrivialPoint& point,
                                               double& t);
        bool GetIntersectionsOfSegments(const Line &line1, const Line &line2, double &x, double &y, double &d1, double &d2);
        bool IsInsidePolygon(const std::vector<TrivialPoint> &test_polygon, const std::vector<Line> &aux_polygon);

        inline bool IsCrossAtEnd(const Line& line1, const Line& line2) {
            return (line1.x2() - line1.x1()) * (line2.y1() - line1.y1()) == (line1.y2() - line1.y1()) * (line2.x1() - line1.x1()) ||
                (line1.x2() - line1.x1()) * (line2.y2() - line1.y1()) == (line1.y2() - line1.y1()) * (line2.x2() - line1.x1()) ||
                (line2.x2() - line2.x1()) * (line1.y1() - line2.y1()) == (line2.y2() - line2.y1()) * (line1.x1() - line2.x1()) ||
                (line2.x2() - line2.x1()) * (line1.y2() - line2.y1()) == (line2.y2() - line2.y1()) * (line1.x2() - line2.x1());
            }
    }

    

    struct ImplicitPlaneFunction
    {
        double a_,b_,c_,d_;
        ImplicitPlaneFunction(double aa = 0,double bb = 0, double cc = 0, double dd = 0):a_(aa),b_(bb),c_(cc),d_(dd){}
        double operator() (const Eigen::Vector3d& p)
        {
            return fabs(p(0) * a_ + p(1) * b_ + p(2) * c_ + d_);
        }

        Eigen::Vector3d unit_norm()
        {
            return Eigen::Vector3d(a_,b_,c_).normalized();
        }
    };



    class Plane
    {
    private:
        //EIGEN_MAKE_ALIGNED_OPERATOR_NEW
        std::vector<Eigen::Vector3d> points_;
        Eigen::Vector3d norm_;
        double d_;
        bool is_bordered_;
    public:
        Plane() {
            is_bordered_ = false;
            norm_ = Eigen::Vector3d(0,0,0);
            d_ = 0;
        }

        static double AverageD(const std::vector<Eigen::Vector3d>& points,const Eigen::Vector3d& norm, bool verbose = false) {
            double sum_d = 0.0;
            for(int i = 0 ; i < points.size() ; i++) {
                sum_d += - norm.dot(points[i]);
            }
            double d = sum_d / points.size();
            if(verbose) {
                double sum_error = 0.0;
                double max_error = 0.;
                for(int i = 0 ; i < points.size() ; i++) {
                    double error = norm.dot(points[i]) + d;
                    max_error = max_error > error ? max_error:error;
                    sum_error += error;
                }
                std::cout<<"max_error: "<< max_error<<"\n";
                std::cout<<"average_error: "<< sum_error / points.size()<<"\n";
            }
            return d;
        }

        Plane(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, const Eigen::Vector3d& p3) {
            points_.push_back(p1);
            points_.push_back(p2);
            points_.push_back(p3);
            norm_ = (p2-p1).cross((p3-p1));
            norm_ = norm_.normalized();
            d_ = AverageD(points_, norm_, false);
            is_bordered_ = true;
        }

        Plane(const std::vector<Eigen::Vector3d>& points) {
            assert(points.size() >= 3);
            points_.assign(points.begin(), points.end());
            norm_ = (points[1]-points[0]).cross((points[2]-points[0])); 
            norm_ = norm_.normalized();
            d_ = AverageD(points_, norm_, false); //(Note)BC: perform a least square for minimum error 'd'
            is_bordered_ = true;
        }

        Eigen::Vector3d Norm() {return norm_;}

        double operator() (const Eigen::Vector3d& p) const
        {
            return norm_.dot(p) + d_;
        }

        bool IsPerpendicularToPlaneXOY() const  {
            return SignOfDouble(norm_(2)) == 0;
        }

        double SolveZ(const double x, const double y) const {
            // if(norm_(2) >= -EPS && norm_(2) <= EPS) {
            if(SignOfDouble(norm_(2)) == 0) {
                if(is_bordered_ == false) {
                    return -GeometryUtils::MILLION;
                }
                else {
                    //At the situation that plane is parallel to ray (x,y), z value is set to the
                    //nearest z (largest) of all points for lower cost on the premise ray (x,y) 
                    //should intersect to polygon
                    //TODO(BCHO): adapt to the case that z value is set according (x,y)
                    double max_z = -GeometryUtils::MILLION;
                    for(const auto& point: points_) {
                        max_z = max_z > point(2) ? max_z: point(2);
                    }
                    return max_z;
                }
                
            } else {
                return (- d_ - norm_(0) * x - norm_(1) * y) / norm_(2);
            }
        }

        std::vector<Eigen::Vector2d> GetXOYPlane() const {
            std::vector<Eigen::Vector2d> plane_2d(points_.size());
            for(int i = 0 ; i < points_.size() ; i++) {
                plane_2d[i] = points_[i].head(2);
            }
            return plane_2d;
        }

        // bool IsDegenerateOnPlaneXOY() {
        //     bool b_degenerate = true;
        //     for(int j = 0 , num_points = points_.size(); j < num_points;j++) {
        //         int pre = (j - 1 + num_points) % num_points;
        //         int nxt = (j + 1) % num_points;
        //         Geomtry2D::Line line1();
        //         Eigen::Vector2d v1 = (points_[pre] - points_[j]).head(2);
        //         Eigen::Vector2d v2 = (points_[nxt] - points_[j]).head(2);
        //         if(SignOfDouble(v1.x() * v2.y() - v1.y() * v2.x()) != 0) {
        //             b_degenerate = false;
        //             break;
        //         }

        //     }
        //     return b_degenerate;
            
        // }


        inline const std::vector<Eigen::Vector3d>& Points() const { return points_;}
        inline  size_t NumPoints() const { return points_.size();}
        inline const Eigen::Vector3d& PointElement(size_t i) const { return points_[i];}

        inline const Eigen::Vector3d& Norm() const { return norm_;}


    };

    bool IsPointInsideTriangular(const Eigen::Vector2d& p0,const Eigen::Vector2d& p1,const Eigen::Vector2d& p2,const Eigen::Vector2d& p);

    template<typename T>
    inline bool IsBetweenInterval(const std::pair<T, T>& interval1, const T& value) {
        return value >= interval1.first && value <= interval1.second;
    }

    template<typename T>
    bool IsIntervalIntersection(const std::pair<T, T>& interval1, const std::pair<T, T>& interval2) {
        return IsBetweenInterval(interval1, interval2.first) || 
                IsBetweenInterval(interval1, interval2.second) ||
                IsBetweenInterval(interval2, interval1.first) ||
                IsBetweenInterval(interval2, interval1.second);
    }

    void SortConvexPointSetInClockWise(std::vector<Eigen::Vector2d>& points);
    void SortConvexPointIndexSetInClockWise(const std::vector<Eigen::Vector3d>& vertices, const Eigen::Vector3d& norm, std::vector<CommonUtils::index_t>& indexes);
    Eigen::Vector2d GetCentroidInConvexPointSet(const std::vector<Eigen::Vector2d>& points);
    Eigen::Vector3d GetCentroidInConvexPointSet(const std::vector<Eigen::Vector3d>& points);

    
}



#endif //VISUAL_MASK_FOR_WALL_GEOMETRY_UTILS_H
