#pragma once

#include <raylib.h>

namespace project {

/// Abstract base class for scene rendering strategies (Strategy pattern)
/// This allows different scene implementations to be swapped at runtime
class SceneStrategy {
public:
    virtual ~SceneStrategy() = default;
    
    // Rule of Five: disable copy, allow move
    SceneStrategy(const SceneStrategy&) = delete;
    SceneStrategy& operator=(const SceneStrategy&) = delete;
    SceneStrategy(SceneStrategy&&) noexcept = default;
    SceneStrategy& operator=(SceneStrategy&&) noexcept = default;
    
    /// Get the name/identifier of this scene
    [[nodiscard]] virtual const char* getName() const = 0;
    
    /// Update the scene (called each frame before drawing)
    virtual void update() {}
    
    /// Draw the scene (called each frame)
    virtual void draw() const = 0;
    
    /// Initialize the scene (called when scene becomes active)
    virtual void initialize() {}
    
    /// Cleanup the scene (called when scene becomes inactive)
    virtual void cleanup() {}
    
protected:
    SceneStrategy() = default;
};

} // namespace project

