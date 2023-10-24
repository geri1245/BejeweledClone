#include "Point.h"

#include <cmath>

int DistanceSquared(const Point& lhs, const Point& rhs)
{
    return int(std::pow(lhs.x - rhs.x, 2)) + int(std::pow(lhs.y - rhs.y, 2));
}

Point operator+(const Point& lhs, const Point& rhs)
{
    return Point { lhs.x + rhs.x, lhs.y + rhs.y };
}

Point operator*(const Point& point, int multiplier)
{
    return Point { point.x * multiplier, point.y * multiplier };
}

Point operator/(const Point& point, int dividend)
{
    return Point { point.x / dividend, point.y / dividend };
}

Point operator-(const Point& lhs, const Point& rhs)
{
    return lhs + rhs * -1;
}
//
// std::strong_ordering operator<=>(const Point& lhs, const Point& rhs)
//{
//    if (lhs.x < rhs.x) {
//        return std::strong_ordering::less;
//    } else if (lhs.x > rhs.x) {
//        return std::strong_ordering::greater;
//    } else {
//        if (lhs.y < rhs.y) {
//            return std::strong_ordering::less;
//        } else if (lhs.y > rhs.y) {
//            return std::strong_ordering::greater;
//        } else {
//            return std::strong_ordering::equal;
//        }
//    }
//}
