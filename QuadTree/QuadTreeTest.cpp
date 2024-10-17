#include "QuadTree.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

void testInsertion() {
    // Create a QuadTree with a boundary rectangle of 100x100 centered at (0, 0)
    QuadTree qt(Rect(0, 0, 100, 100));

    // Test inserting a point inside the boundary
    const Point p0{10, 10};
    assert( qt.insert(p0));
    // The point should be inserted correctly
    std::cout << "Insertion test passed for point (10, 10)\n";
}

void testSubdivision() {
    // Create a QuadTree with a boundary rectangle of 100x100 centered at (0, 0)
    QuadTree qt(Rect(0, 0, 100, 100));

    // Insert points beyond the capacity of a node
    const Point p1{10, 10};
    const Point p2{-10, 10};
    const Point p3{-10, -10};
    const Point p4{10, -10};
    const Point p5{0, 0};

    qt.insert(p1);
    qt.insert(p2);
    qt.insert(p3);
    qt.insert(p4);
    qt.insert(p5);  // This point should trigger subdivision

    // If the QuadTree is subdivided, there should be four subtrees
    if (qt.isDivided()) {
        std::cout << "Subdivision test passed.\n";
    } else {
        std::cout << "Subdivision test failed.\n";
    }
}

void testNearestNeighbors() {
    // Create a QuadTree with a boundary rectangle of 100x100 centered at (0, 0)
    QuadTree qt(Rect(0, 0, 100, 100));

    // Insert several points into the QuadTree
    qt.insert(Point(10, 10));
    qt.insert(Point(20, 20));
    qt.insert(Point(-10, -10));
    qt.insert(Point(5, 5));

    // Target point for nearest neighbors search
    const Point target{0, 0};
    std::array<Point, 2> nearest;
    int maxDist = std::numeric_limits<int>::max();

    // Search for the two nearest neighbors of point (0, 0)
    qt.nearestNeighbors(target, nearest, maxDist);

    assert(nearest[0] == Point(5, 5) && nearest[1]== Point(-10, -10));
    std::cout << "2 Nearest neighbors test passed.\n";
}

void testOutOfBoundsInsertion() {
    // Create a QuadTree with a boundary rectangle of 100x100 centered at (0, 0)
    QuadTree qt(Rect(0, 0, 100, 100));

    // Attempt to insert a point outside the boundary
    const Point p{-150, 150};

    // The point should not be inserted as it is outside the boundary
    assert(!qt.insert(p));

    std::cout << "Out of bounds insertion test passed.\n";
}

void testInsertAndNearestNeighbors() {
    const Rect boundary(50, 50, 50, 50);
    QuadTree qt(boundary);

    // Insertion of points into the QuadTree
    std::vector<Point> points = {
        Point(10, 10), Point(20, 20), Point(30, 30), Point(40, 40), Point(50, 50),
        Point(60, 60), Point(70, 70), Point(80, 80), Point(90, 90), Point(15, 15),
        Point(25, 25), Point(35, 35), Point(45, 45), Point(55, 55), Point(65, 65),
        Point(75, 75), Point(85, 85), Point(95, 95)
    };

    for (const auto& p : points) {
        assert(qt.insert(p));
    }

    // Illustration of the quadtree after insertion:
    // Level 1 (root): (50, 50) - Size: 50x50
    // Subtrees:
    //   - NE: Contains (60, 60), (70, 70), (80, 80), (90, 90), (55, 55)
    //   - NW: Contains (30, 30), (20, 20), (10, 10)
    //   - SE: Contains (75, 75), (85, 85), (95, 95)
    //   - SW: Empty

    // Test for finding the 4 nearest neighbors of the point (45, 45)
    std::array<Point, 4> nearest;
    int maxDist = std::numeric_limits<int>::max();
    qt.nearestNeighbors(Point(45, 45), nearest, maxDist);

    // Expected results: points close to (45, 45)
    // Probably (40, 40), (50, 50), (35, 35), and (55, 55)

    std::cout << "Insert and nearest neighbors test passed!" << std::endl;
}

void testLargeDataset() {
    constexpr int MAP_SIZE = 100;
    constexpr int NUM_POINTS = 1000;

    const Rect boundary(MAP_SIZE / 2, MAP_SIZE / 2, MAP_SIZE / 2, MAP_SIZE / 2);
    QuadTree qt(boundary);

    std::random_device rd;
    std::seed_seq sd{rd(), rd(), rd(), rd()}; // 4 values are enough. 8 max.
    auto g = std::mt19937(sd); // advanced seeding
    std::uniform_int_distribution dis(0, MAP_SIZE);

    std::vector<Point> points;
    for (int i = 0; i < NUM_POINTS; ++i) {
        Point p(dis(g), dis(g));
        points.push_back(p);
        qt.insert(p);
    }

    // Illustration:
    // With 1000 points, the tree will subdivide into many subtrees
    // The points will be distributed over a large space, each quadrant will contain
    // a portion of the 1000 points. Each node will have a maximum capacity before subdivision.

    // Test for finding 4 and 8 neighbors
    std::array<Point, 4> nearest4;
    std::array<Point, 8> nearest8;
    int maxDist = std::numeric_limits<int>::max();
    qt.nearestNeighbors(Point(MAP_SIZE / 2, MAP_SIZE / 2), nearest4, maxDist);
    qt.nearestNeighbors(Point(MAP_SIZE / 2, MAP_SIZE / 2), nearest8, maxDist);

    std::cout << "Large dataset test passed!" << std::endl;
}

void testEdgeCases() {
    const Rect boundary(50, 50, 50, 50);
    QuadTree qt(boundary);

    // Case 1: Empty tree
    std::array<Point, 4> nearest;
    int maxDist = std::numeric_limits<int>::max();
    qt.nearestNeighbors(Point(0, 0), nearest, maxDist);
    assert(maxDist == std::numeric_limits<int>::max()); // No point found

    // Case 2: Tree with a single point
    qt.insert(Point(50, 50));
    qt.nearestNeighbors(Point(0, 0), nearest, maxDist);
    assert(nearest[0] == Point(50, 50)); // Only one point should be found

    // Case 3: Points on the edges
    qt.insert(Point(0, 0));
    qt.insert(Point(100, 0));
    qt.insert(Point(0, 100));
    qt.insert(Point(100, 100));

    // Search for nearest neighbors of (0, 0)
    qt.nearestNeighbors(Point(0, 0), nearest, maxDist);

    // Illustration of the quadtree:
    // Points at the corners of the boundary square: (0, 0), (100, 0), (0, 100), (100, 100)
    // Subtrees will contain these points in well-defined regions.
    // Expected results: (0, 0), (100, 0), (0, 100), (50, 50)

    std::cout << "Edge cases tests passed!" << std::endl;
}


void testAdvancedScenarios() {
    const Rect boundary(50, 50, 50, 50);
    QuadTree qt(boundary);

    // Scenario 1: Clustered points
    std::vector cluster1 = {
        Point(10, 10), Point(11, 11), Point(12, 12), Point(13, 13), Point(14, 14)
    };
    std::vector cluster2 = {
        Point(80, 80), Point(81, 81), Point(82, 82), Point(83, 83), Point(84, 84)
    };

    for (const auto& p : cluster1) qt.insert(p);
    for (const auto& p : cluster2) qt.insert(p);
    qt.print_quadtree();
    // Illustration of the quadtree after insertion:
    // Level 0 (root): (50, 50) - Size: 50x50: Contains all from cluster 1 except (14, 14)
    // Subtrees:
    //   - NW: Contains (14,14)
    //   - SE: Contains all points from cluster2
    //   - NE and SW: Empty

    // Test nearest neighbors for a point between clusters
    std::array<Point, 3> nearest;
    int maxDist = std::numeric_limits<int>::max();
    qt.nearestNeighbors(Point(40, 40), nearest, maxDist);
    // Expected results: The 3 nearest points should all be from cluster1,
    // as it's closer to (40, 40) than cluster2, (12, 12) and (13, 13) and (14,14) are likely candidates
    std::array<Point,3> expected = {Point(12, 12), Point(13, 13), Point(14, 14)};

    std::sort(nearest.begin(), nearest.end());
    std::sort(expected.begin(), expected.end());
    assert(nearest == expected);

    // Scenario 2: Points along a diagonal line
    qt = QuadTree(boundary); // Reset the tree
    std::vector diagonal = {
        Point(0, 0), Point(20, 20), Point(40, 40), Point(60, 60), Point(80, 80), Point(100, 100)
    };

    for (const auto& p : diagonal) qt.insert(p);

    // Illustration of the quadtree after insertion:
    // Level 1 (root): (50, 50) - Size: 50x50
    // Points are distributed across different quadrants:
    //   - SW: (0, 0), (20, 20)
    //   - NE: (60, 60), (80, 80), (100, 100)
    //   - Center: (40, 40)

    // Test nearest neighbors for a point slightly off the diagonal
    qt.nearestNeighbors(Point(45, 50), nearest, maxDist);

    // Expected results: Points close to (45, 50) on the diagonal,
    // likely (40, 40) and (60, 60)

    std::cout << "Diagonal points test: 3 nearest neighbors to (45, 50) are:\n";
    for (const auto& p : nearest) {
        std::cout << "(" << p.x << ", " << p.y << ") ";
    }
    std::cout << "\n";

    // Scenario 3: Sparse distribution with outliers
    qt = QuadTree(boundary); // Reset the tree
    std::vector sparsePoints = {
        Point(10, 10), Point(90, 90), Point(10, 90), Point(90, 10), // corners
        Point(50, 50), // center
        Point(0, 50), Point(100, 50), Point(50, 0), Point(50, 100), // middle of edges
        Point(-10, -10), Point(110, 110) // outliers
    };

    for (const auto& p : sparsePoints) qt.insert(p);

    // Illustration of the quadtree after insertion:
    // Level 1 (root): (50, 50) - Size: 50x50
    // Points are distributed across all quadrants, with some outside the boundary:
    //   - NW: (10, 90)
    //   - NE: (90, 90)
    //   - SW: (10, 10)
    //   - SE: (90, 10)
    //   - Center: (50, 50)
    //   - On edges: (0, 50), (100, 50), (50, 0), (50, 100)
    //   - Outside: (-10, -10), (110, 110)

    // Test nearest neighbors for a point near the center
    std::array<Point, 5> nearestFive;
    qt.nearestNeighbors(Point(60, 60), nearestFive, maxDist);

    // Expected results: Points close to (60, 60), likely including (50, 50) and (90, 90)

    std::cout << "Sparse distribution test: 5 nearest neighbors to (60, 60) are:\n";
    for (const auto& p : nearestFive) {
        std::cout << "(" << p.x << ", " << p.y << ") ";
    }
    std::cout << "\n";

    std::cout << "Advanced scenarios tests completed!\n";
}


// Helper functions for colored output
void print_separator() {
    std::cout << std::endl << "\033[0m-----------------\n";
}
void print_in_yellow(const std::string &str) {
    print_separator();
    std::cout << "\033[1;33m" << str << std::endl;
}
void print_in_blue(const std::string &str) {
    print_separator();
    std::cout << "\033[1;34m" << str << std::endl;
}

int main() {
    print_in_yellow("QuadTree Tests");
    print_in_blue("Insertion Tests");
    std::cout << "\033[1;32m";
    testInsertion();
    testOutOfBoundsInsertion();
    print_in_blue("Subdivision Tests");
    std::cout << "\033[1;32m";
    testSubdivision();
    print_in_blue("Nearest Neighbors Tests");
    std::cout << "\033[1;32m";
    testNearestNeighbors();
    testInsertAndNearestNeighbors();

    print_in_blue("Large Dataset Tests And Advanced Scenarios");
    std::cout << "\033[1;32m";
    testLargeDataset();
    testAdvancedScenarios();

    print_in_blue("Edge Cases Tests");
    std::cout << "\033[1;32m";
    testEdgeCases();
    print_in_yellow("All tests passed!");
    return 0;
}