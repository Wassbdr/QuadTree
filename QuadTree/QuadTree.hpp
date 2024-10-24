#ifndef QUADTREE_H
#define QUADTREE_H

#include <array>
#include <memory>
#include <vector>
#include <queue>

struct QueueItem;

// Point structure representing a 2D point with x and y coordinates as float
struct Point {
    float x, y, payload; // Additional field for payload data

    explicit Point(const float x = 0.0f, const float y = 0.0f, const float payload = 0.0f)
        : x(x),
          y(y),
          payload(payload) {
    }

    bool operator==(const Point &other) const; // Check for equality of two points
    bool operator<(const Point &other) const; // Comparison based on coordinates
    bool operator!=(const Point &other) const; // Check for inequality of two points
};

// Rectangle (Rect) structure representing a boundary with x, y as the center, and w, h as half-width and half-height
struct Rect {
    float x, y, w, h;

    explicit Rect(const float x = 0.0f, const float y = 0.0f, const float w = 0.0f, const float h = 0.0f)
        : x(x),
          y(y),
          w(w),
          h(h) {
    }

    [[nodiscard]] bool contains(const Point &p) const; // Check if a point is within the rectangle
    [[nodiscard]] bool intersects(const Rect &range) const; // Check if two rectangles overlap
};

// Inline function to compute squared Euclidean distance between two points (avoids costly square root)
inline float distanceSquared(const Point &a, const Point &b) {
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

class QuadTree {
    static constexpr int CAPACITY = 4; // Max number of points before subdividing the node
    Rect boundary; // The boundary this QuadTree node represents
    std::array<Point, CAPACITY> points; // Array storing points within this node
    int point_count = 0; // Current number of points in the node
    bool divided = false; // Flag indicating if the node has been subdivided

    // Child QuadTree nodes (subdivisions)
    std::unique_ptr<QuadTree> northeast;
    std::unique_ptr<QuadTree> northwest;
    std::unique_ptr<QuadTree> southeast;
    std::unique_ptr<QuadTree> southwest;


    void print_quadtree_rec(int depth) const; // Helper function to recursively print the tree

    void subdivide(); // Subdivide the current node into four child nodes


public:
    explicit QuadTree(const Rect &boundary); // Constructor initializing QuadTree with a boundary

    [[nodiscard]] bool isDivided() const; // Check if this node is subdivided
    static int capacity();

    void print_quadtree(int depth = 0) const; // Print the QuadTree structure

    bool insert(const Point &point); // Insert a point into the QuadTree

    // Nearest neighbor search function with preallocated memory
    template<size_t N>
    void nearestNeighbors(const Point &target, std::array<Point, N> &nearest, float &maxDist,
                          std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>> &nodeQueue,
                          std::vector<std::pair<float, Point>> &nearestHeap) const;
};
struct QueueItem {
    const QuadTree* node;
    float distance;

    QueueItem(const QuadTree* n, const float d) : node(n), distance(d) {}

    bool operator>(const QueueItem& other) const {
        return distance > other.distance;
    }
};
#include "QuadTree.tpp"

#endif //QUADTREE_H
