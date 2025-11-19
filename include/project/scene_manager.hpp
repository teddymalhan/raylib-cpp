#pragma once

#include <project/scene_strategy.hpp>
#include <memory>
#include <vector>

namespace project {

/// Manages scene switching using the Strategy pattern
/// Allows switching between different scene implementations at runtime
class SceneManager {
public:
    SceneManager();
    ~SceneManager() = default;
    
    // Rule of Five: disable copy, allow move
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;
    SceneManager(SceneManager&&) noexcept = default;
    SceneManager& operator=(SceneManager&&) noexcept = default;
    
    /// Register a scene strategy (takes ownership)
    void registerScene(std::unique_ptr<SceneStrategy> scene);
    
    /// Switch to a scene by index
    void switchToScene(size_t index);
    
    /// Switch to the next scene (cycles through)
    void switchToNextScene();
    
    /// Switch to the previous scene (cycles through)
    void switchToPreviousScene();
    
    /// Get the current scene index
    [[nodiscard]] size_t getCurrentSceneIndex() const noexcept { return currentSceneIndex; }
    
    /// Get the total number of registered scenes
    [[nodiscard]] size_t getSceneCount() const noexcept { return scenes.size(); }
    
    /// Get the current scene (nullptr if no scenes registered)
    [[nodiscard]] SceneStrategy* getCurrentScene() noexcept;
    [[nodiscard]] const SceneStrategy* getCurrentScene() const noexcept;
    
    /// Update the current scene
    void update();
    
    /// Draw the current scene
    void draw() const;

private:
    std::vector<std::unique_ptr<SceneStrategy>> scenes;
    size_t currentSceneIndex{0};
    
    void activateScene(size_t index);
};

} // namespace project

