#include "QuadTree/QuadTree.hpp"
#include <iostream>
#include <chrono>
#include <random>
#include <queue>

int main() {
    constexpr int MAP_SIZE = 3600;
    constexpr int NUM_QUERIES = 1000000
    ;

    const Rect boundary(MAP_SIZE / 2.0f, MAP_SIZE / 2.0f, MAP_SIZE / 2.0f, MAP_SIZE / 2.0f);
    QuadTree qt(boundary);

    // Measure insertion time
    auto start = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < MAP_SIZE; ++x) {
        for (int y = 0; y < MAP_SIZE; ++y) {
            const float payload = static_cast<float>(x + y) / 2.0f;
            qt.insert(Point(static_cast<float>(x), static_cast<float>(y), payload));
        }
    }
    auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> insert_time = end - start;
    std::cout << "Insertion time: " << insert_time.count() << " seconds\n";

    // Setup random number generation for queries
    std::random_device rd;
    std::seed_seq sd{rd(), rd(), rd(), rd()};
    auto gen = std::mt19937(sd);
    std::uniform_int_distribution dis(0, MAP_SIZE - 1);

    // Preallocate nearest neighbors array and heap
    std::array<Point, 8> nearest;
    float maxDist = std::numeric_limits<float>::max();
    std::vector<std::pair<float, Point>> nearestHeap;
    nearestHeap.reserve(8); // Reserve space for nearest neighbors heap
    std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>> nodeQueue;

    // Measure nearest neighbor search time
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_QUERIES; ++i) {
        Point target(static_cast<float>(dis(gen)), static_cast<float>(dis(gen)));

        // Reset heap and queue for each query
        nearestHeap.clear();
        while (!nodeQueue.empty()) nodeQueue.pop();  // Clear the priority queue

        // Perform nearest neighbor search
        qt.nearestNeighbors<8>(target, nearest, maxDist, nodeQueue, nearestHeap);
    }
    end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double> nn_search_time = end - start;

    std::cout << "Total nearest neighbor search time: " << nn_search_time.count() << " seconds\n";
    std::cout << "Average time per search: " << (nn_search_time.count() / NUM_QUERIES) << " seconds\n";

    return 0;
}
