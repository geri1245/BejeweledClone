#pragma once

#include <compare>

struct Point {
    int x;
    int y;

    std::strong_ordering operator<=>(const Point& other) const = default;
};

int DistanceSquared(const Point& lhs, const Point& rhs);
Point Lerp(const Point& lhs, const Point& rhs, double progress);

Point operator+(const Point& lhs, const Point& rhs);
Point operator*(const Point& point, int multiplier);
Point operator/(const Point& point, int dividend);
Point operator-(const Point& lhs, const Point& rhs);

template <>
struct std::hash<Point> {
    std::size_t operator()(const Point& p) const noexcept
    {
        std::size_t h1 = std::hash<int> {}(p.x);
        std::size_t h2 = std::hash<int> {}(p.y);
        return h1 ^ (h2 << 1); // or use boost::hash_combine
    }
};
