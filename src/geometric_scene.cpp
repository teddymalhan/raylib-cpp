#include <project/geometric_scene.hpp>
#include <raymath.h>
#include <cmath>
#include <numbers>

namespace project {

GeometricScene::GeometricScene() = default;

GeometricScene::~GeometricScene() {
    cleanup();
}

void GeometricScene::initialize() {
    if (isInitialized) {
        return;
    }
    
    // Create a simple cube model for drawing
    constexpr float kCubeSize = 1.0F;
    Mesh cubeMesh = GenMeshCube(kCubeSize, kCubeSize, kCubeSize);
    cubeModel = LoadModelFromMesh(cubeMesh);
    hasCubeModel = IsModelValid(cubeModel);
    
    createGeometricObjects();
    time = 0.0F;
    isInitialized = true;
}

void GeometricScene::cleanup() {
    if (isInitialized) {
        if (hasCubeModel) {
            UnloadModel(cubeModel);
            hasCubeModel = false;
        }
        objects.clear();
        isInitialized = false;
    }
}

void GeometricScene::createGeometricObjects() {
    objects.clear();
    
    // Create a pattern of geometric shapes in a circle
    constexpr int kObjectCount = 8;
    constexpr float kRadius = 3.0F;
    constexpr float kHeight = 1.0F;
    constexpr float kObjectSize = 0.8F;
    constexpr float kBaseRotationSpeed = 30.0F;
    constexpr float kRotationSpeedIncrement = 10.0F;
    constexpr float kColorSaturation = 0.8F;
    constexpr float kColorValue = 0.9F;
    constexpr float kMaxHue = 360.0F;
    constexpr float kCenterHeight = 0.5F;
    constexpr float kCenterSize = 1.5F;
    constexpr float kCenterRotationSpeed = 20.0F;
    
    for (int i = 0; i < kObjectCount; ++i) {
        const float angle = (static_cast<float>(i) / static_cast<float>(kObjectCount)) * 2.0F * std::numbers::pi_v<float>;
        const float posX = kRadius * std::cos(angle);
        const float posZ = kRadius * std::sin(angle);
        
        GeometricObject obj;
        obj.position = Vector3{posX, kHeight, posZ};
        obj.size = Vector3{kObjectSize, kObjectSize, kObjectSize};
        obj.rotationSpeed = kBaseRotationSpeed + static_cast<float>(i) * kRotationSpeedIncrement;
        
        // Vary colors
        const float hue = static_cast<float>(i) / static_cast<float>(kObjectCount);
        obj.color = ColorFromHSV(hue * kMaxHue, kColorSaturation, kColorValue);
        
        objects.push_back(obj);
    }
    
    // Add a central larger object
    GeometricObject centerObj;
    centerObj.position = Vector3{0.0F, kCenterHeight, 0.0F};
    centerObj.size = Vector3{kCenterSize, kCenterSize, kCenterSize};
    centerObj.color = GOLD;
    centerObj.rotationSpeed = kCenterRotationSpeed;
    objects.push_back(centerObj);
}

void GeometricScene::update() {
    if (!isInitialized) {
        return;
    }
    
    time += GetFrameTime();
    
    constexpr float kMaxRotationDegrees = 360.0F;
    
    // Update rotations
    for (auto& obj : objects) {
        obj.rotation.y = std::fmod(time * obj.rotationSpeed, kMaxRotationDegrees);
    }
}

void GeometricScene::draw() const {
    if (!isInitialized || !hasCubeModel) {
        return;
    }
    
    constexpr float kMaxRotationDegrees = 360.0F;
    const Vector3 kRotationAxis = Vector3{0.0F, 1.0F, 0.0F}; // Y-axis rotation
    
    for (const auto& obj : objects) {
        // Use DrawModelEx for rotation support
        const float rotationAngle = std::fmod(obj.rotation.y, kMaxRotationDegrees) * DEG2RAD;
        DrawModelEx(cubeModel, obj.position, kRotationAxis, rotationAngle, obj.size, obj.color);
        
        // Draw wireframe (simplified - just draw cube wires at position)
        // Note: DrawModelEx doesn't have a wireframe version, so we draw a simple wireframe cube
        DrawCubeWiresV(obj.position, obj.size, DARKGRAY);
    }
}

} // namespace project

