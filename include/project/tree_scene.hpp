#pragma once

#include <project/scene_strategy.hpp>
#include <project/scene.hpp>
#include <string>

namespace project {

/// Concrete scene implementation featuring a tree model
/// This is the original scene from the codebase
class TreeScene : public SceneStrategy {
public:
    explicit TreeScene(const std::string& modelPath);
    ~TreeScene() override;
    
    // Rule of Five: disable copy, allow move
    TreeScene(const TreeScene&) = delete;
    TreeScene& operator=(const TreeScene&) = delete;
    TreeScene(TreeScene&&) noexcept = default;
    TreeScene& operator=(TreeScene&&) noexcept = default;
    
    void draw() const override;
    [[nodiscard]] const char* getName() const override { return "Tree Scene"; }
    void initialize() override;
    void cleanup() override;
    
    [[nodiscard]] size_t getObjectCount() const { return scene.getObjectCount(); }

private:
    Scene scene;
    std::string modelPath;
    bool isInitialized{false};
    
    static constexpr float kModelScale = 2.0F;
};

} // namespace project

