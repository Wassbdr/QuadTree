#include "QuadTree/QuadTree.h"
#include <iostream>
#include <chrono>
#include <random>

int main() {
    constexpr int MAP_SIZE = 3600;
    constexpr int NUM_QUERIES = 1000000;

    const Rect boundary(MAP_SIZE / 2, MAP_SIZE / 2, MAP_SIZE / 2, MAP_SIZE / 2);
    QuadTree qt(boundary);

    // Insert points into the quadtree
    auto start = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            qt.insert(Point(x, y));
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> insert_time = end - start;
    std::cout << "Insertion time: " << insert_time.count() << " seconds\n";

    std::random_device rd;
    std::seed_seq sd{rd(), rd(), rd(), rd()}; // 4 values are enough. 8 max.
    auto gen = std::mt19937(sd); // advanced seeding
    std::uniform_int_distribution dis(0, MAP_SIZE - 1);

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_QUERIES; ++i) {
        Point qp(dis(gen), dis(gen));
        std::array<Point, 8> nearest;
        int maxDist = std::numeric_limits<int>::max();
        qt.nearestNeighbors(qp, nearest, maxDist);
    }
    end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> nn_search_time = end - start;

    std::cout << "First Total nearest neighbor search time: " << nn_search_time.count() << " seconds\n";
    std::cout << "First Average time per search: " << (nn_search_time.count() / NUM_QUERIES) << " seconds\n";

    return 0;
}