#include <gtest/gtest.h>
#include "QuadTree.hpp"

class QuadTreeTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a QuadTree covering a 100x100 area centered at (0,0)
        tree = std::make_unique<QuadTree>(Rect(0.0f, 0.0f, 50.0f, 50.0f));
    }

    std::unique_ptr<QuadTree> tree;
};

// Test inserting a point into the QuadTree
TEST_F(QuadTreeTest, InsertPoint) {
    EXPECT_TRUE(tree->insert(Point(10.0f, 10.0f)));   // Point inside
    EXPECT_TRUE(tree->insert(Point(-10.0f, -10.0f))); // Point inside
    EXPECT_TRUE(tree->insert(Point(0.0f, 0.0f)));     // Point at the center
}

// Test inserting a point outside the QuadTree bounds
TEST_F(QuadTreeTest, InsertOutOfBounds) {
    EXPECT_FALSE(tree->insert(Point(60.0f, 60.0f)));  // Point outside
    EXPECT_FALSE(tree->insert(Point(-60.0f, -60.0f))); // Point outside
}

// Test QuadTree subdivision
TEST_F(QuadTreeTest, Subdivision) {
    for (int i = 0; i < QuadTree::capacity(); ++i) {
        EXPECT_TRUE(tree->insert(Point(static_cast<float>(i), static_cast<float>(i))));
    }
    EXPECT_FALSE(tree->isDivided());  // Should not be divided yet.

    EXPECT_TRUE(tree->insert(Point(QuadTree::capacity(), QuadTree::capacity())));
    EXPECT_TRUE(tree->isDivided());  // Tree should now be subdivided.
}

// Test Rect contains method
TEST_F(QuadTreeTest, RectContains) {
    const Rect rect(0.0f, 0.0f, 10.0f, 10.0f);
    EXPECT_TRUE(rect.contains(Point(0.0f, 0.0f)));    // Point at center
    EXPECT_TRUE(rect.contains(Point(10.0f, 10.0f)));  // Point at edge
    EXPECT_TRUE(rect.contains(Point(-10.0f, -10.0f))); // Point at negative edge
    EXPECT_FALSE(rect.contains(Point(11.0f, 11.0f)));  // Point outside
}

// Test Rect intersects method
TEST_F(QuadTreeTest, RectIntersects) {
    const Rect rect1(0.0f, 0.0f, 10.0f, 10.0f);
    const Rect rect2(5.0f, 5.0f, 10.0f, 10.0f);  // Overlaps with rect1
    const Rect rect3(20.0f, 20.0f, 5.0f, 5.0f);  // Does not overlap with rect1

    EXPECT_TRUE(rect1.intersects(rect2));  // Should intersect
    EXPECT_FALSE(rect1.intersects(rect3)); // Should not intersect
}

// Test Point equality operator
TEST_F(QuadTreeTest, PointEquality) {
    const Point p1(1.0f, 2.0f);
    const Point p2(1.0f, 2.0f);
    const Point p3(2.0f, 1.0f);

    EXPECT_EQ(p1, p2);  // p1 and p2 should be equal
    EXPECT_NE(p1, p3);  // p1 and p3 should not be equal
}

// Test Point comparison operator
TEST_F(QuadTreeTest, PointComparison) {
    const Point p1(1.0f, 2.0f);
    const Point p2(2.0f, 1.0f);
    const Point p3(1.0f, 3.0f);

    EXPECT_LT(p1, p2);  // p1 is less than p2
    EXPECT_LT(p1, p3);  // p1 is less than p3
}

// Test KNN search with a small set of points
TEST_F(QuadTreeTest, NearestNeighborsSmallSet) {
    tree->insert(Point(10.0f, 10.0f));
    tree->insert(Point(-10.0f, -10.0f));
    tree->insert(Point(20.0f, 20.0f));
    tree->insert(Point(30.0f, 30.0f));
    tree->insert(Point(40.0f, 40.0f));

    tree->print_quadtree();  // Print the QuadTree structure
    std::array<Point, 3> nearest;
    long maxDist = std::numeric_limits<long>::max();
    tree->nearestNeighbors<3>(Point(0.0f, 0.0f), nearest, maxDist);

    EXPECT_EQ(nearest[0], Point(10.0f, 10.0f));  // Closest point
    EXPECT_EQ(nearest[1], Point(-10.0f, -10.0f)); // Second closest point
    EXPECT_EQ(nearest[2], Point(20.0f, 20.0f));  // Third closest point
}

// Test KNN search with a large set of points
TEST_F(QuadTreeTest, NearestNeighborsLargeSet) {
    for (int i = -40; i <= 40; i += 10) {
        for (int j = -40; j <= 40; j += 10) {
            tree->insert(Point(static_cast<float>(i), static_cast<float>(j)));
        }
    }

    std::array<Point, 5> nearest;
    long maxDist = std::numeric_limits<long>::max();
    tree->nearestNeighbors<5>(Point(15.0f, 15.0f), nearest, maxDist);

    std::vector<Point> expected = {Point(10.0f, 10.0f), Point(20.0f, 10.0f), Point(10.0f, 20.0f), Point(20.0f, 20.0f), Point(0, 20.0f)};
    for (const auto& point : nearest) {
        EXPECT_TRUE(std::find(expected.begin(), expected.end(), point) != expected.end());
    }
}

// Test KNN search when the target is on an existing point
TEST_F(QuadTreeTest, NearestNeighborsTargetOnExistingPoint) {
    tree->insert(Point(0.0f, 0.0f));
    tree->insert(Point(10.0f, 10.0f));
    tree->insert(Point(-10.0f, -10.0f));
    tree->insert(Point(20.0f, 20.0f));

    std::array<Point, 3> nearest;
    long maxDist = std::numeric_limits<long>::max();
    tree->nearestNeighbors<3>(Point(0.0f, 0.0f), nearest, maxDist);

    EXPECT_EQ(nearest[0], Point(10.0f, 10.0f));  // Nearest point
    EXPECT_EQ(nearest[1], Point(-10.0f, -10.0f)); // Second closest
    EXPECT_EQ(nearest[2], Point(20.0f, 20.0f));  // Third closest
}

// Test KNN search with fewer points than requested
TEST_F(QuadTreeTest, NearestNeighborsFewerPoints) {
    tree->insert(Point(10.0f, 10.0f));
    tree->insert(Point(20.0f, 20.0f));

    std::array<Point, 3> nearest;
    long maxDist = std::numeric_limits<long>::max();
    tree->nearestNeighbors<3>(Point(0.0f, 0.0f), nearest, maxDist);

    EXPECT_EQ(nearest[0], Point(10.0f, 10.0f));  // First closest
    EXPECT_EQ(nearest[1], Point(20.0f, 20.0f));  // Second closest
    EXPECT_EQ(nearest[2], Point());              // No third point, should return default
}

// Test KNN search in an empty QuadTree
TEST_F(QuadTreeTest, NearestNeighborsEmptyTree) {
    std::array<Point, 3> nearest;
    long maxDist = std::numeric_limits<long>::max();
    tree->nearestNeighbors<3>(Point(0.0f, 0.0f), nearest, maxDist);

    for (const auto& point : nearest) {
        EXPECT_EQ(point, Point());  // All should be default points since the tree is empty
    }
}

// Test KNN search with exactly 8 points in the QuadTree
TEST_F(QuadTreeTest, NearestNeighborsExact8Points) {
    tree->insert(Point(10.0f, 10.0f));
    tree->insert(Point(-10.0f, -10.0f));
    tree->insert(Point(20.0f, 20.0f));
    tree->insert(Point(-20.0f, -20.0f));
    tree->insert(Point(30.0f, 30.0f));
    tree->insert(Point(-30.0f, -30.0f));
    tree->insert(Point(40.0f, 40.0f));
    tree->insert(Point(-40.0f, -40.0f));

    std::array<Point, 8> nearest;
    long maxDist = std::numeric_limits<long>::max();
    tree->nearestNeighbors<8>(Point(0.0f, 0.0f), nearest, maxDist);

    std::vector<Point> expected = {
        Point(10.0f, 10.0f), Point(-10.0f, -10.0f), Point(20.0f, 20.0f), Point(-20.0f, -20.0f),
        Point(30.0f, 30.0f), Point(-30.0f, -30.0f), Point(40.0f, 40.0f), Point(-40.0f, -40.0f)
    };
    for (int i = 0; i < 8; ++i) {
        EXPECT_EQ(nearest[i], expected[i]);
    }
}

// Test KNN search requesting 8 neighbors with more than 8 points in the QuadTree
TEST_F(QuadTreeTest, NearestNeighborsMoreThan8Points) {
    for (int i = -40; i <= 40; i += 10) {
        for (int j = -40; j <= 40; j += 10) {
            tree->insert(Point(static_cast<float>(i), static_cast<float>(j)));
        }
    }

    std::array<Point, 8> nearest;
    long maxDist = std::numeric_limits<long>::max();
    tree->nearestNeighbors<8>(Point(0.0f, 0.0f), nearest, maxDist);

    std::vector<Point> expected = {
        Point(-10.0f, 10.0f), Point(10.0f, -10.0f), Point(10.0f, 10.0f), Point(0.0f, 10.0f),
        Point(0.0f, -10.0f), Point(10.0f, 10.0f), Point(-10.0f, -10.0f), Point(10.0f, -10.0f)
    };

    for (const auto& point : nearest) {
        EXPECT_TRUE(std::find(expected.begin(), expected.end(), point) != expected.end());
    }
}


// Test KNN search requesting 8 neighbors but with fewer than 8 points in the QuadTree
TEST_F(QuadTreeTest, NearestNeighborsRequest8FewerPoints) {
    tree->insert(Point(10.0f, 10.0f));
    tree->insert(Point(-10.0f, -10.0f));
    tree->insert(Point(20.0f, 20.0f));
    tree->insert(Point(-20.0f, -20.0f));

    std::array<Point, 8> nearest;
    long maxDist = std::numeric_limits<long>::max();
    tree->nearestNeighbors<8>(Point(0.0f, 0.0f), nearest, maxDist);

    EXPECT_EQ(nearest[0], Point(10.0f, 10.0f));
    EXPECT_EQ(nearest[1], Point(-10.0f, -10.0f));
    EXPECT_EQ(nearest[2], Point(20.0f, 20.0f));
    EXPECT_EQ(nearest[3], Point(-20.0f, -20.0f));
    EXPECT_EQ(nearest[4], Point());  // Default value since there are no more points
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
