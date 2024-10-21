#include "QuadTree/QuadTree.hpp"
#include <iostream>
#include <chrono>
#include <random>

int main() {
    constexpr int MAP_SIZE = 3600;
    constexpr int NUM_QUERIES = 1000000;
    constexpr int SCALE = 100000;

    const Rect boundary(MAP_SIZE / 2.0f, MAP_SIZE / 2.0f, MAP_SIZE / 2.0f, MAP_SIZE / 2.0f);
    QuadTree qt(boundary);

    auto start = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            float payload = static_cast<float>(x + y) / 2.0f;
            qt.insert(Point(static_cast<float>(x) / SCALE, static_cast<float>(y) / SCALE, payload));
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> insert_time = end - start;
    std::cout << "Insertion time: " << insert_time.count() << " seconds\n";

    std::random_device rd;
    std::seed_seq sd{rd(), rd(), rd(), rd()};
    auto gen = std::mt19937(sd);
    std::uniform_int_distribution dis(0, MAP_SIZE - 1);

    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_QUERIES; ++i) {
        Point qp(static_cast<float>(dis(gen)) / SCALE, static_cast<float>(dis(gen)) / SCALE);
        std::array<Point, 8> nearest;
        int maxDist = std::numeric_limits<int>::max();
        qt.nearestNeighbors(qp, nearest, maxDist);
    }
    end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> nn_search_time = end - start;

    std::cout << "Total nearest neighbor search time: " << nn_search_time.count() << " seconds\n";
    std::cout << "Average time per search: " << (nn_search_time.count() / NUM_QUERIES) << " seconds\n";

    return 0;
}
