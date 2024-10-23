#ifndef QUADTREE_TPP
#define QUADTREE_TPP

#include <queue>
#include <limits>
#include <algorithm>
#include <vector>
#include <array>

// Optimized nearest neighbor search in QuadTree
template<size_t N>
void QuadTree::nearestNeighbors(const Point &target, std::array<Point, N> &nearest, float &maxDist,
                                std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>>& nodeQueue,
                                std::vector<std::pair<float, Point>> &nearestHeap) const {

    nodeQueue.emplace(this, 0.0f);
    while (!nodeQueue.empty()) {
        const QuadTree* current = nodeQueue.top().node;
        const float currentDistance = nodeQueue.top().distance;
        nodeQueue.pop();

        // Stop if current distance is larger than the farthest point in nearestHeap
        if (nearestHeap.size() == N && currentDistance > maxDist) {
            break;  // Early exit
        }

        // Check all points in the current node
        for (int i = 0; i < current->point_count; ++i) {
            const Point& candidate = current->points[i];
            if (candidate == target) continue;

            float dist = distanceSquared(target, candidate);

            // Add to heap if we haven't found N points yet
            if (nearestHeap.size() < N) {
                nearestHeap.emplace_back(dist, candidate);
                if (nearestHeap.size() == N) {
                    std::ranges::make_heap(nearestHeap.begin(), nearestHeap.end()); // Build heap
                    maxDist = nearestHeap.front().first; // Update maxDist after heap is filled
                }
            }
            // Otherwise, only replace if the new point is closer
            else if (dist < nearestHeap.front().first) {
                std::ranges::pop_heap(nearestHeap.begin(), nearestHeap.end());
                nearestHeap.back() = std::make_pair(dist, candidate);
                std::ranges::push_heap(nearestHeap.begin(), nearestHeap.end());
                maxDist = nearestHeap.front().first; // Update maxDist
            }
        }

        // Traverse the child nodes
        if (current->divided) {
            for (const QuadTree* child : { current->northeast.get(), current->northwest.get(), current->southeast.get(), current->southwest.get() }) {
                if (!child) continue;

                // Calculate the minimum distance from the target to the boundary of the child node
                float dx = std::max(0.0f, std::abs(target.x - child->boundary.x) - child->boundary.w);
                float dy = std::max(0.0f, std::abs(target.y - child->boundary.y) - child->boundary.h);
                float minDist = dx * dx + dy * dy;

                // Only traverse if minDist is smaller than maxDist, or we haven't found enough neighbors
                if (minDist <= maxDist || nearestHeap.size() < N) {
                    // Use the array of representative points to prune further
                    for (const auto& rep : child->points) {
                        float repDist = distanceSquared(target, rep);

                        // Check if representative point is within maxDist, and if so, enqueue the child
                        if (repDist < maxDist) {
                            nodeQueue.emplace(child, minDist);  // Enqueue child node for further exploration
                            break; // If one rep point qualifies, no need to check others
                        }
                    }
                }
            }
        }
    }

    // Populate the nearest array
    for (unsigned int i = 0; i < N && !nearestHeap.empty(); ++i) {
        std::ranges::pop_heap(nearestHeap.begin(), nearestHeap.end());
        nearest[i] = nearestHeap.back().second;
        nearestHeap.pop_back();
    }
}

#endif // QUADTREE_TPP
