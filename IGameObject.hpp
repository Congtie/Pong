#pragma once
#include <raylib.h>
#include <string>
#include <memory>
#include <vector>
#include <algorithm>

// ─── Abstract base for every renderable/updatable object ───────────────────
class IGameObject {
public:
    virtual ~IGameObject() = default;
    virtual void Draw()   const = 0;
    virtual void Update()       = 0;
    virtual bool IsAlive() const { return true; }
};

// ─── Template manager: owns a collection of any IGameObject subtype ────────
template <typename T>
class GameObjectManager {
    static_assert(std::is_base_of<IGameObject, T>::value,
                  "T must derive from IGameObject");
public:
    void Add(std::unique_ptr<T> obj) {
        objects_.push_back(std::move(obj));
    }

    void UpdateAll() {
        for (auto& obj : objects_) obj->Update();
        // Prune dead objects (e.g. cleared Tetris lines)
        objects_.erase(
            std::remove_if(objects_.begin(), objects_.end(),
                           [](const std::unique_ptr<T>& o){ return !o->IsAlive(); }),
            objects_.end());
    }

    void DrawAll() const {
        for (const auto& obj : objects_) obj->Draw();
    }

    std::vector<std::unique_ptr<T>>& Objects() { return objects_; }
    const std::vector<std::unique_ptr<T>>& Objects() const { return objects_; }
    std::size_t Count() const { return objects_.size(); }

private:
    std::vector<std::unique_ptr<T>> objects_;
};
