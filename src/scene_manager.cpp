#include <project/scene_manager.hpp>
#include <stdexcept>
#include <iostream>

namespace project {

SceneManager::SceneManager() = default;

void SceneManager::registerScene(std::unique_ptr<SceneStrategy> scene) {
    if (!scene) {
        return;
    }
    
    scenes.push_back(std::move(scene));
    
    // If this is the first scene, activate it
    if (scenes.size() == 1) {
        activateScene(0);
    }
}

void SceneManager::switchToScene(size_t index) {
    if (index >= scenes.size()) {
        return;
    }
    
    activateScene(index);
}

void SceneManager::switchToNextScene() {
    if (scenes.empty()) {
        return;
    }
    
    // Calculate next index with proper wrapping
    // When at the last scene (index scenes.size() - 1), this should wrap to 0
    const size_t nextIndex = (currentSceneIndex + 1) % scenes.size();
    
    // Debug output (can be removed later)
    std::cout << "Switching from scene " << currentSceneIndex 
              << " to scene " << nextIndex 
              << " (total: " << scenes.size() << ")\n";
    
    activateScene(nextIndex);
}

void SceneManager::switchToPreviousScene() {
    if (scenes.empty()) {
        return;
    }
    
    const size_t prevIndex = (currentSceneIndex == 0) 
        ? scenes.size() - 1 
        : currentSceneIndex - 1;
    activateScene(prevIndex);
}

SceneStrategy* SceneManager::getCurrentScene() noexcept {
    if (currentSceneIndex >= scenes.size()) {
        return nullptr;
    }
    return scenes[currentSceneIndex].get();
}

const SceneStrategy* SceneManager::getCurrentScene() const noexcept {
    if (currentSceneIndex >= scenes.size()) {
        return nullptr;
    }
    return scenes[currentSceneIndex].get();
}

void SceneManager::update() {
    if (auto* scene = getCurrentScene()) {
        scene->update();
    }
}

void SceneManager::draw() const {
    if (const auto* scene = getCurrentScene()) {
        scene->draw();
    }
}

void SceneManager::activateScene(size_t index) {
    if (index >= scenes.size()) {
        return;
    }
    
    // Only cleanup if we have a valid current scene and it's different from the target
    if (currentSceneIndex < scenes.size() && 
        currentSceneIndex != index && 
        scenes[currentSceneIndex]) {
        try {
            scenes[currentSceneIndex]->cleanup();
        } catch (...) {
            // If cleanup throws, continue anyway to prevent crash
            // This can happen if Bullet Physics cleanup fails
        }
    }
    
    // Update index
    currentSceneIndex = index;
    
    // Initialize new scene
    if (scenes[currentSceneIndex]) {
        try {
            scenes[currentSceneIndex]->initialize();
        } catch (...) {
            // If initialization throws, log but continue
            // The scene will just be in an uninitialized state
        }
    }
}

} // namespace project

