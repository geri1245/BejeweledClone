#pragma once

#include <compare>

struct Point {
    int x;
    int y;

    std::strong_ordering operator<=>(const Point& other) const = default;
};

int DistanceSquared(const Point& lhs, const Point& rhs);

Point operator+(const Point& lhs, const Point& rhs);
Point operator*(const Point& point, int multiplier);
Point operator/(const Point& point, int dividend);
Point operator-(const Point& lhs, const Point& rhs);
