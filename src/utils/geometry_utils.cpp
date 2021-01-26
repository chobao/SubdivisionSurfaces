#include "geometry_utils.h"
#include <cmath>


namespace GeometryUtils {

    namespace Geometry2D{
        bool IsRayCross(double x, double y, const Line &l){
            double x1 = l.x1(),
                y1 = l.y1(),
                x2 = l.x2(),
                y2 = l.y2();
            if (y1 == y2 ||
                y > y1 && y > y2 ||
                y < y1 && y < y2) {
                return false;
            }
            double tx = (x2 - x1) * (y - y1) / (y2 - y1) + x1;
            return tx > x;
        }

        double SolveParameter(const double x, const double l1, const double l2) {
            if((x >= l1 && x <= l2) || (x <= l1 && x >= l2)) {
                return (x - l1) / (l2 - l1);
            } else {
                return -1.0;
            }
        }

        double IsInSegment(const double x, const double y, const Line& line) {
            //assert(line.x1() == line.x2() && line.y1() == line.y2());
            double t = -1.0;
            if(line.x1() != line.x2()) {
                t = SolveParameter(x, line.x1(),line.x2());
            } else {
                t = SolveParameter(y, line.y1(),line.y2());
            }
            return t;
        }

        bool IsPointInLine(const Line& line, const TrivialPoint& point) {
                double p1x = point.x() - line.x1();
                double p1y = point.y() - line.y1();
                double p2x = point.x() - line.x2();
                double p2y = point.y() - line.y2();
                double ans =p1x * p2y - p1y * p2x;
                return ans == 0;
        }

        bool IsPointInSegment(const Line& line, const TrivialPoint& point,
                                            double& t) {
            if(IsPointInLine(line, point)) {
                t = IsInSegment(point.x(), point.y(), line);
                if(t >= 0 && t <= 1) {
                    return true;
                }
            }
            return false;
        }


        bool GetIntersectionsOfSegments(const Line &line1, const Line &line2, double &x, double &y, double &d1, double &d2) {
            double a1 = line1.y1() - line1.y2(),
                b1 = line1.x2() - line1.x1(),
                c1 = line1.x2() * line1.y1() - line1.x1() * line1.y2();
            double a2 = line2.y1() - line2.y2(),
                b2 = line2.x2() - line2.x1(),
                c2 = line2.x2() * line2.y1() - line2.x1() * line2.y2();
            const double D = a1 * b2 - a2 * b1;
            if (D == 0) {
                return false;
            }
            x = (b2 * c1 - b1 * c2) / D;
            y = (a1 * c2 - a2 * c1) / D;
            d1 = IsInSegment(x,y,line1);
            d2 = IsInSegment(x,y,line2);
            if(d1 > 0 && d1 < 1 && d2 > 0 && d2 < 1) {  //Comment!(BCHO):ignore one vertex of two lines overlap(t == 0|| t == 1)
                return true;
            } else  {
                return false;
            }
        }

        bool IsInsidePolygon(const std::vector<TrivialPoint> &test_polygon, const std::vector<Line> &aux_polygon) {
            for (int i = 0, szi = test_polygon.size(); i < szi; ++i) {
                bool b_is_outside = true;
                double x = test_polygon[i].x(),
                    y = test_polygon[i].y();
                for (int j = 0, szj = aux_polygon.size(); j < szj; ++j) {
                    const Line &l = aux_polygon[j];
                    double t;
                    if (IsPointInSegment( l, test_polygon[i], t)) {
                        b_is_outside = false;
                        break;
                    }
                    if (IsRayCross(x, y, l)) {
                        b_is_outside = !b_is_outside;
                    }
                }
                if (b_is_outside) {
                    return false;
                }
            }
            return true;
        }
    }


    bool IsPointInsideTriangular(const Eigen::Vector2d& p0,const Eigen::Vector2d& p1,const Eigen::Vector2d& p2,const Eigen::Vector2d& p) {
        //p0,p1,p2 make up of a triangular
        Eigen::Vector2d line1 = p1 - p0;
        Eigen::Vector2d line2 = p2 - p0;
        Eigen::Vector2d line = p - p0;
        
        double l1l1 = line1.dot(line1);
        double l1l2 = line1.dot(line2);
        double l2l2 = line2.dot(line2);
        double ll1 = line.dot(line1);
        double ll2 = line.dot(line2);
        Eigen::Matrix2d A;
        A << l1l1,l1l2,
                l1l2,l2l2;
        // if(A.determinant() == 0.0) return false; //TODO(BCHO): when the p0,p1,p2 is a line, |A| = 0, Is there any other situation leading to |A|=0?
        double detA = A.determinant();
        if(SignOfDouble(detA) == 0) 
        {
            return false;
        }
        Eigen::Vector2d b(ll1,ll2);
        Eigen::Vector2d res = A.inverse() * b;

        return SignOfDouble(res(0))>=0 && SignOfDouble(res(1)) >=0 && SignOfDouble(res(0) + res(1) - 1.0) <= 0;
    }

    Eigen::Vector2d GetCentroidInConvexPointSet(const std::vector<Eigen::Vector2d>& points) {
            Eigen::Vector2d center(0.0,0.0);
            for (const auto& p : points) {
                center += p;
            }
            center /= points.size();
            return center;
    }

    Eigen::Vector3d GetCentroidInConvexPointSet(const std::vector<Eigen::Vector3d>& points) {
            Eigen::Vector3d center(0.0, 0.0, 0.0);
            for (const auto& p : points) {
                center += p;
            }
            center /= points.size();
            return center;
    }

    void SortConvexPointSetInClockWise(std::vector<Eigen::Vector2d>& points) {
        Eigen::Vector2d center = GetCentroidInConvexPointSet(points);
        auto cmp = [&] (const Eigen::Vector2d& p1, const Eigen::Vector2d& p2){
            double angle1 = std::atan2(p1(1) - center(1), p1(0) - center(0));
            double angle2 = std::atan2(p2(1) - center(1), p2(0) - center(0));
            return angle1 < angle2;
        };

        std::sort(points.begin(), points.end(), cmp);
        
    }

    void SortConvexPointIndexSetInClockWise(const std::vector<Eigen::Vector3d>& vertices, const Eigen::Vector3d& norm, std::vector<CommonUtils::index_t>& indexes) {
            std::vector<Eigen::Vector3d> points(indexes.size());
            for(int i = 0 ; i < indexes.size() ; i++) {
                points[i] = vertices[indexes[i]];
            }
            Eigen::Vector3d center = GetCentroidInConvexPointSet(points);
            
            auto cmp = [&] (const CommonUtils::index_t& p1, const CommonUtils::index_t& p2){
                Eigen::Vector3d n1 = vertices[p1] - center;
                Eigen::Vector3d n2 = vertices[p2] - center;
                return norm.dot(n1.cross(n2)) > 0;

            };
            std::sort(indexes.begin(), indexes.end(), cmp);
            
    }
}
