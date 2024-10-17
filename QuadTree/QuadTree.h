#ifndef QUADTREE_H
#define QUADTREE_H

#include <array>
#include <memory>
#include <vector>

struct Point {
    int x, y;
    explicit Point(const int x = 0, const int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const;
    bool operator<(const Point& other) const;
};

struct Rect {
    int x, y, w, h;
    explicit Rect(const int x = 0, const int y = 0, const int w = 0, const int h = 0) : x(x), y(y), w(w), h(h) {}

    [[nodiscard]] bool contains(const Point& p) const;
    [[nodiscard]] bool intersects(const Rect& range) const;
};
// Inline distanceSquared function to avoid repeated function calls
inline int distanceSquared(const Point& a, const Point& b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return dx * dx + dy * dy;
}

class QuadTree {
    static constexpr int CAPACITY = 16;
    Rect boundary;
    std::array<Point, CAPACITY> points;
    int point_count = 0;
    bool divided = false;

    std::unique_ptr<QuadTree> northeast;
    std::unique_ptr<QuadTree> northwest;
    std::unique_ptr<QuadTree> southeast;
    std::unique_ptr<QuadTree> southwest;
    void print_quadtree_rec(int depth) const;
    void subdivide();

public:
    explicit QuadTree(const Rect& boundary);

    void redistributePoints();

    [[nodiscard]] bool isDivided() const;

    void print_quadtree(int depth = 0) const;


    bool insert(const Point& point);

    template<size_t N>
    void nearestNeighbors(const Point& target, std::array<Point, N>& nearest, int& maxDist) const;
};


#include "QuadTree.tpp"

#endif //QUADTREE_H
