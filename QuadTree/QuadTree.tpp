#include <queue>
#include <limits>
#include <algorithm>

// Finds the nearest neighbors to a target point within the QuadTree
template<size_t N>
void QuadTree::nearestNeighbors(const Point& target, std::array<Point, N>& nearest, int& maxDist) const {
    struct QueueItem {
        const QuadTree* node; // Pointer to the QuadTree node
        int distance; // Distance from the target point

        // Constructor for QueueItem to initialize the node and distance
        QueueItem(const QuadTree* n, const int d) : node(n), distance(d) {}

        // Comparison operator for priority queue
        bool operator>(const QueueItem& other) const {
            return distance > other.distance;
        }
    };

    // Priority queue to explore nodes based on their distance to the target point
    std::priority_queue<QueueItem, std::vector<QueueItem>, std::greater<>> pq;
    pq.emplace(this, 0); // Start with the root QuadTree node

    int nearest_count = 0; // Count of nearest neighbors found
    int furthest_index = -1; // Index of the furthest neighbor found

    while (!pq.empty()) {
        const QuadTree* current = pq.top().node; // Get the current node
        const int nodeDistance = pq.top().distance; // Get the distance of the current node
        pq.pop();

        // Early exit if the node's min distance exceeds the max distance for KNN
        if (nearest_count == N && nodeDistance > maxDist) {
            break; // Stop searching further nodes if max distance exceeded
        }

        // Check all points in the current node for nearest neighbors
        for (int i = 0; i < current->point_count; ++i) {
            const Point& candidate = current->points[i];
            if (candidate == target) continue; // Skip the target point itself

            int dist = distanceSquared(target, candidate); // Calculate distance squared to the candidate point
            if (nearest_count < N) {
                nearest[nearest_count++] = candidate; // Add candidate to nearest list if not full
                if (nearest_count == N) {
                    // Compute the maximum distance after filling the array
                    furthest_index = std::distance(nearest.begin(), std::max_element(nearest.begin(), nearest.end(), [&](const Point& a, const Point& b) {
                        return distanceSquared(target, a) < distanceSquared(target, b);
                    }));
                    maxDist = distanceSquared(target, nearest[furthest_index]); // Update maxDist
                }
            } else if (dist < maxDist) {
                // Replace the furthest point if the candidate is closer
                nearest[furthest_index] = candidate;
                furthest_index = std::distance(nearest.begin(), std::max_element(nearest.begin(), nearest.end(), [&](const Point& a, const Point& b) {
                    return distanceSquared(target, a) < distanceSquared(target, b);
                }));
                maxDist = distanceSquared(target, nearest[furthest_index]); // Update maxDist
            }
        }

        // Enqueue child nodes if the current node is divided
        if (current->divided) {
            auto enqueue_if_closer = [&](const QuadTree* child) {
                if (!child) return; // Exit if the child node does not exist

                // Calculate the minimum distance from the target to the boundary of this child
                int dx = std::max(0, std::abs(target.x - child->boundary.x) - child->boundary.w);
                int dy = std::max(0, std::abs(target.y - child->boundary.y) - child->boundary.h);
                int minDist = dx * dx + dy * dy; // Minimum squared distance to the child

                // Only explore this child if it could contain a closer neighbor
                if (minDist <= maxDist || nearest_count < N) {
                    pq.emplace(child, minDist); // Add the child to the queue for exploration
                }
            };

            // Check all four child nodes (northeast, northwest, southeast, southwest)
            enqueue_if_closer(current->northeast.get());
            enqueue_if_closer(current->northwest.get());
            enqueue_if_closer(current->southeast.get());
            enqueue_if_closer(current->southwest.get());
        }
    }
}
