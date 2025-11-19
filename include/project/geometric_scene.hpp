#pragma once

#include <project/scene_strategy.hpp>
#include <vector>
#include <raylib.h>

namespace project {

/// Concrete scene implementation featuring geometric shapes
/// Demonstrates a different scene using the Strategy pattern
class GeometricScene : public SceneStrategy {
public:
    GeometricScene();
    ~GeometricScene() override;
    
    // Rule of Five: disable copy, allow move
    GeometricScene(const GeometricScene&) = delete;
    GeometricScene& operator=(const GeometricScene&) = delete;
    GeometricScene(GeometricScene&&) noexcept = default;
    GeometricScene& operator=(GeometricScene&&) noexcept = default;
    
    void update() override;
    void draw() const override;
    [[nodiscard]] const char* getName() const override { return "Geometric Scene"; }
    void initialize() override;
    void cleanup() override;

private:
    struct GeometricObject {
        Vector3 position{0.0F, 0.0F, 0.0F};
        Vector3 rotation{0.0F, 0.0F, 0.0F};
        Vector3 size{1.0F, 1.0F, 1.0F};
        Color color{WHITE};
        float rotationSpeed{0.0F};
    };
    
    std::vector<GeometricObject> objects;
    Model cubeModel{};
    bool hasCubeModel{false};
    float time{0.0F};
    bool isInitialized{false};
    
    void createGeometricObjects();
};

} // namespace project

