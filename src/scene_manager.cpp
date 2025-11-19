#include <project/scene_manager.hpp>
#include <stdexcept>

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
    
    const size_t nextIndex = (currentSceneIndex + 1) % scenes.size();
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
    
    // Cleanup current scene
    if (currentSceneIndex < scenes.size() && scenes[currentSceneIndex]) {
        scenes[currentSceneIndex]->cleanup();
    }
    
    // Update index
    currentSceneIndex = index;
    
    // Initialize new scene
    if (scenes[currentSceneIndex]) {
        scenes[currentSceneIndex]->initialize();
    }
}

} // namespace project

