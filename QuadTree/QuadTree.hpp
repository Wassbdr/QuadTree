#ifndef QUADTREE_H
#define QUADTREE_H

#include <array>
#include <memory>

// Point structure representing a 2D point with x and y coordinates as float
struct Point {
    int x, y;
    float payload;  // Additional field for payload data

    explicit Point(const float x = 0.0f, const float y = 0.0f, const float payload = 0.0f)
        : x(static_cast<int>(x * 100000)),
          y(static_cast<int>(y * 100000)),
            payload(payload) {}

    bool operator==(const Point& other) const;   // Check for equality of two points
    bool operator<(const Point& other) const;    // Comparison based on coordinates
    bool operator!=(const Point& other) const;   // Check for inequality of two points
};

// Rectangle (Rect) structure representing a boundary with x, y as the center, and w, h as half-width and half-height
struct Rect {
    int x, y, w, h;
    explicit Rect(const int x = 0.0, const int y = 0.0, const int w = 0.0f, const int h = 0.0f)
        : x(x * 100000),
          y(y * 100000),
          w(w * 100000),
          h(h * 100000) {}

    [[nodiscard]] bool contains(const Point& p) const;     // Check if a point is within the rectangle
    [[nodiscard]] bool intersects(const Rect& range) const; // Check if two rectangles overlap
};

// Inline function to compute squared Euclidean distance between two points (avoids costly square root)
inline int distanceSquared(const Point& a, const Point& b) {
    const int dx = a.x - b.x;
    const int dy = a.y - b.y;
    return dx * dx + dy * dy;
}

class QuadTree {
    static constexpr int CAPACITY = 4;      // Max number of points before subdividing the node
    Rect boundary;                           // The boundary this QuadTree node represents
    std::array<Point, CAPACITY> points;      // Array storing points within this node
    int point_count = 0;                     // Current number of points in the node
    bool divided = false;                    // Flag indicating if the node has been subdivided

    // Child QuadTree nodes (subdivisions)
    std::unique_ptr<QuadTree> northeast;
    std::unique_ptr<QuadTree> northwest;
    std::unique_ptr<QuadTree> southeast;
    std::unique_ptr<QuadTree> southwest;

    void print_quadtree_rec(int depth) const;  // Helper function to recursively print the tree
    void subdivide();                          // Subdivide the current node into four child nodes

public:
    explicit QuadTree(const Rect& boundary);   // Constructor initializing QuadTree with a boundary

    [[nodiscard]] bool isDivided() const;      // Check if this node is subdivided
    static int capacity();

    void print_quadtree(int depth = 0) const;  // Print the QuadTree structure

    bool insert(const Point& point);           // Insert a point into the QuadTree

    // Nearest Neighbor search: Find the N closest neighbors to the target point
    template<size_t N>
    void nearestNeighbors(const Point& target, std::array<Point, N>& nearest, int& maxDist) const;
};

#include "QuadTree.tpp"

#endif //QUADTREE_H
