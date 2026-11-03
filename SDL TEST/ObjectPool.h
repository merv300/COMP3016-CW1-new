#pragma once

#include <vector>
#include <memory>
#include <cassert>

namespace rtz {

// Fixed-capacity object pool with automatic expansion.
// Designed for cache-friendly allocation of homogeneous game objects
// (bullets, enemies, particles) where construction/destruction is frequent.
template <typename T>
class ObjectPool {
public:
    // capacityHint: initial reserve. Pool grows by 50% when exhausted.
    explicit ObjectPool(size_t capacityHint = 64)
        : nextIndex(0) {
        objects.reserve(capacityHint);
        for (size_t i = 0; i < capacityHint; ++i) {
            objects.emplace_back(std::make_unique<T>());
        }
    }

    // Returns a pointer to an inactive object, activating it.
    // The object is default-constructed (or reset via Activate() if provided).
    template <typename... Args>
    T* Acquire(Args&&... args) {
        // Find first inactive
        for (size_t i = 0; i < objects.size(); ++i) {
            if (!objects[i]->IsActive()) {
                T* obj = objects[i].get();
                obj->Activate(std::forward<Args>(args)...);
                return obj;
            }
        }

        // Pool exhausted - expand by 50%
        const size_t oldSize = objects.size();
        const size_t newSize = oldSize + (oldSize / 2) + 1;
        objects.reserve(newSize);
        for (size_t i = oldSize; i < newSize; ++i) {
            objects.emplace_back(std::make_unique<T>());
        }

        T* obj = objects[oldSize].get();
        obj->Activate(std::forward<Args>(args)...);
        return obj;
    }

    // Calls Deactivate() on all active objects.
    void ReleaseAll() {
        for (auto& obj : objects) {
            if (obj->IsActive()) {
                obj->Deactivate();
            }
        }
    }

    // Iterates active objects only. Use in update/render loops.
    template <typename Func>
    void ForEachActive(Func&& func) {
        for (auto& obj : objects) {
            if (obj->IsActive()) {
                func(*obj);
            }
        }
    }

    // Const version for read-only access.
    template <typename Func>
    void ForEachActive(Func&& func) const {
        for (const auto& obj : objects) {
            if (obj->IsActive()) {
                func(*obj);
            }
        }
    }

    size_t ActiveCount() const {
        size_t count = 0;
        for (const auto& obj : objects) {
            if (obj->IsActive()) ++count;
        }
        return count;
    }

    size_t Capacity() const { return objects.size(); }

private:
    std::vector<std::unique_ptr<T>> objects;
    size_t nextIndex; // unused but kept for potential round-robin strategy
};

} // namespace rtz