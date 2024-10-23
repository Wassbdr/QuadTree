#include "QuadTree.hpp"

#include <iostream>

// Checks if two points are equal based on their coordinates
bool Point::operator==(const Point &other) const {
    return x == other.x && y == other.y;
}

// Defines the order for comparing points, using x as the primary key and y as the secondary key
bool Point::operator<(const Point &other) const {
    return x < other.x || (x == other.x && y < other.y);
}

// Checks if two points are not equal based on their coordinates
bool Point::operator!=(const Point &other) const {
    return x != other.x || y != other.y;
}

// This method includes points on the right and bottom edges when determining if a point is within the rectangle
bool Rect::contains(const Point &p) const {
    return p.x >= x - w && p.x <= x + w && p.y >= y - h && p.y <= y + h;

}

// Checks whether two rectangles overlap by comparing their boundaries
bool Rect::intersects(const Rect &range) const {
    return !(range.x - range.w > x + w || range.x + range.w < x - w ||
             range.y - range.h > y + h || range.y + range.h < y - h);
}

// Constructor for the QuadTree, initializes with a boundary rectangle
QuadTree::QuadTree(const Rect &boundary) : boundary(boundary) {}

// Subdivides the current QuadTree node into four child nodes
void QuadTree::subdivide() {
    if (point_count == 0) return; // No points to subdivide if none exist

    // Calculate the midpoints to divide the boundary into quadrants
    const float midX = boundary.x;
    const float midY = boundary.y;

    const float halfWidth = boundary.w / 2;
    const float halfHeight = boundary.h / 2;

    // Creates smaller boundary rectangles for each quadrant and initializes child QuadTrees
    northeast = std::make_unique<QuadTree>(Rect(midX + halfWidth, midY - halfHeight, halfWidth, halfHeight));
    northwest = std::make_unique<QuadTree>(Rect(midX - halfWidth, midY - halfHeight, halfWidth, halfHeight));
    southeast = std::make_unique<QuadTree>(Rect(midX + halfWidth, midY + halfHeight, halfWidth, halfHeight));
    southwest = std::make_unique<QuadTree>(Rect(midX - halfWidth, midY + halfHeight, halfWidth, halfHeight));

    // Redistribute points from the parent node into child nodes
    for (int i = 0; i < point_count; ++i) {
        if (!northeast->insert(points[i]) &&
            !northwest->insert(points[i]) &&
            !southeast->insert(points[i])) {
            southwest->insert(points[i]);
        }
    }



    point_count = 0; // Clear the points from this node after redistribution
    divided = true;  // Mark the node as subdivided
}


// Inserts a point into the QuadTree, subdividing if necessary
bool QuadTree::insert(const Point &point) {
    if (!boundary.contains(point)) {
        return false; // Point is outside the current boundary
    }

    if (point_count < CAPACITY && !divided) {
        points[point_count] = point; // Store point if within capacity and no subdivision
        point_count++;
        return true;
    }

    if (!divided) {
        subdivide(); // Subdivide if capacity is exceeded
    }

    // Try to insert the point into one of the child nodes
    if (northeast->insert(point)) return true;
    if (northwest->insert(point)) return true;
    if (southeast->insert(point)) return true;
    if (southwest->insert(point)) return true;

    return false;
}

// Helper method to check if the QuadTree node is subdivided
bool QuadTree::isDivided() const {
    return divided;
}

int QuadTree::capacity() {
    return CAPACITY;
}

// Prints the QuadTree structure starting from the root node, color-coded and indented by depth
void QuadTree::print_quadtree(const int depth) const {
    std::cout << "\033[1;35mLEVEL 0:\n"; // Color output for the top-level node
    print_quadtree_rec(depth);
    std::cout << "\033[1;32m"; // Color reset
}

// Recursive helper function to print the structure of the QuadTree
void QuadTree::print_quadtree_rec(const int depth) const {
    // Prints indentation corresponding to the current depth of recursion
    auto print_indent = [](const int d) {
        for (int i = 0; i < d; ++i) std::cout << "    "; // Indentation: 4 spaces per depth level
    };

    print_indent(depth);
    std::cout << "Boundary: (" << boundary.x << ", " << boundary.y << ", " << boundary.w << ", " << boundary.h << ")\n";

    print_indent(depth);
    std::cout << "Points: ";
    for (int i = 0; i < point_count; ++i) {
        std::cout << "(" << points[i].x << ", " << points[i].y << ", " << points[i].payload << ") "; // Prints the stored points with payload
    }
    std::cout << "\n";

    if (divided) { // If the node has been subdivided, recursively print each quadrant
        print_indent(depth);
        std::cout << "LEVEL " << depth + 1 << ":\n";

        if (northeast) {
            print_indent(depth);
            std::cout << "- NE:\n";
            northeast->print_quadtree_rec(depth + 1);
        }

        if (northwest) {
            print_indent(depth);
            std::cout << "- NW:\n";
            northwest->print_quadtree_rec(depth + 1);
        }

        if (southeast) {
            print_indent(depth);
            std::cout << "- SE:\n";
            southeast->print_quadtree_rec(depth + 1);
        }

        if (southwest) {
            print_indent(depth);
            std::cout << "- SW:\n";
            southwest->print_quadtree_rec(depth + 1);
        }
    }
}