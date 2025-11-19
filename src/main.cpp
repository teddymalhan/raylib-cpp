#include <raylib.h>
#include <raymath.h>
#include <project/scene_manager.hpp>
#include <project/tree_scene.hpp>
#include <project/geometric_scene.hpp>
#include <project/bullet_physics_scene.hpp>
#include <project/imgui_manager.hpp>
#include <project/gui_controls.hpp>
#include <iostream>
#include <memory>

namespace {
    constexpr int kWindowWidth = 800;
    constexpr int kWindowHeight = 600;
    constexpr const char* kWindowTitle = "Raylib 3D Scene Example";
    
    constexpr float kCameraPositionX = 2.0F;
    constexpr float kCameraPositionY = 1.5F;
    constexpr float kCameraPositionZ = 2.0F;
    constexpr float kCameraFovy = 45.0F;
    constexpr int kTargetFPS = 60;
    constexpr int kGridSlices = 10;
    constexpr float kGridSpacing = 1.0F;
    constexpr int kFpsPosX = 10;
    constexpr int kFpsPosY = 10;

    Camera3D createCamera() {
        Camera3D camera{};
        camera.position = Vector3{ kCameraPositionX, kCameraPositionY, kCameraPositionZ };
        camera.target = Vector3{ 0.0F, 0.0F, 0.0F };
        camera.up = Vector3{ 0.0F, 1.0F, 0.0F };
        camera.fovy = kCameraFovy;
        camera.projection = CAMERA_PERSPECTIVE;
        return camera;
    }

    void runGameLoop(Camera3D& camera, project::SceneManager& sceneManager, 
                     project::ImGuiManager& imguiManager, project::GuiControls& guiControls) {
        SetTargetFPS(kTargetFPS);
        
        // Camera tracking state
        bool cameraTrackingEnabled = true;
        bool cursorEnabled = false;
        
        // Start with cursor disabled (camera tracking enabled)
        DisableCursor();
        
        #pragma unroll
        while (!WindowShouldClose()) {
            // Begin ImGui frame
            imguiManager.beginFrame();
            
            // Check if ImGui wants input
            const bool imguiWantsMouse = imguiManager.wantsCaptureMouse();
            const bool imguiWantsKeyboard = imguiManager.wantsCaptureKeyboard();
            
            // Handle F4 key to toggle camera tracking
            if (IsKeyPressed(KEY_F4)) {
                cameraTrackingEnabled = !cameraTrackingEnabled;
            }
            
            // Handle clicking on non-ImGui area to re-enable camera tracking
            if (!cameraTrackingEnabled && !imguiWantsMouse) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
                    cameraTrackingEnabled = true;
                }
            }
            
            // Update cursor state based on camera tracking and ImGui state
            if (!cameraTrackingEnabled || imguiWantsMouse) {
                // Show cursor when camera tracking is disabled or ImGui wants mouse
                if (!cursorEnabled) {
                    EnableCursor();
                    cursorEnabled = true;
                }
            } else {
                // Hide cursor when camera tracking is enabled and ImGui doesn't want mouse
                if (cursorEnabled) {
                    DisableCursor();
                    cursorEnabled = false;
                }
            }
            
            // Update first-person camera only if tracking is enabled and ImGui doesn't want input
            if (cameraTrackingEnabled && !imguiWantsMouse && !imguiWantsKeyboard) {
            UpdateCamera(&camera, CAMERA_FIRST_PERSON);
            }
            
            // Update current scene
            sceneManager.update();
            
            // Draw
            BeginDrawing();
            ClearBackground(SKYBLUE);
            
            BeginMode3D(camera);
            
            // Draw the current scene
            sceneManager.draw();
            
            // Draw a grid for reference
            DrawGrid(kGridSlices, kGridSpacing);
            
            EndMode3D();
            
            // Draw raylib UI (FPS counter)
            DrawFPS(kFpsPosX, kFpsPosY);
            
            // Render ImGui GUI
            guiControls.renderControlPanel(sceneManager, camera);
            guiControls.renderDebugPanel();
            guiControls.renderSceneInfo(sceneManager);
            guiControls.showDemoWindow();
            
            // End ImGui frame (renders ImGui)
            imguiManager.endFrame();
            
            EndDrawing();
        }
        
        // Restore cursor state
        if (cursorEnabled) {
            DisableCursor();
        }
    }
}

int main() {
    InitWindow(kWindowWidth, kWindowHeight, kWindowTitle);
    
    if (!IsWindowReady()) {
        std::cerr << "Failed to initialize window\n";
        return 1;
    }
    
    // Initialize ImGui
    project::ImGuiManager imguiManager;
    imguiManager.initialize();
    
    // Create GUI controls
    project::GuiControls guiControls;
    
    // Set up 3D camera
    Camera3D camera = createCamera();
    
    // Create scene manager to handle multiple scenes (Strategy pattern)
    project::SceneManager sceneManager;
    
    // Register only the Bullet Physics scene
    auto bulletPhysicsScene = std::make_unique<project::BulletPhysicsScene>();
    sceneManager.registerScene(std::move(bulletPhysicsScene));
    std::cout << "Registered Bullet Physics Scene\n";
    
    runGameLoop(camera, sceneManager, imguiManager, guiControls);
    
    // Cleanup
    CloseWindow();
    return 0;
}

