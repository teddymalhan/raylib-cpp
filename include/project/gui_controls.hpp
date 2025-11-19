#pragma once

#include <project/scene_manager.hpp>
#include <raylib.h>

namespace project {

/// GUI controls and panels for the application
/// Provides UI for scene management, camera controls, and debug information
class GuiControls {
public:
    GuiControls() = default;
    ~GuiControls() = default;
    
    // Rule of Five: disable copy, allow move
    GuiControls(const GuiControls&) = delete;
    GuiControls& operator=(const GuiControls&) = delete;
    GuiControls(GuiControls&&) noexcept = default;
    GuiControls& operator=(GuiControls&&) noexcept = default;
    
    /// Render the main control panel
    /// @param sceneManager Reference to scene manager for scene switching
    /// @param camera Reference to camera for controls
    void renderControlPanel(SceneManager& sceneManager, Camera3D& camera);
    
    /// Render debug information panel
    void renderDebugPanel();
    
    /// Render scene information panel
    /// @param sceneManager Reference to scene manager
    void renderSceneInfo(SceneManager& sceneManager);
    
    /// Show demo window (for testing ImGui integration)
    void showDemoWindow();
    
    /// Get show demo flag
    [[nodiscard]] bool& getShowDemo() noexcept { return showDemo; }

private:
    // Control panel state
    bool showControlPanel{true};
    bool showDebugPanel{true};
    bool showSceneInfo{true};
    bool showDemo{false};
    
    // Camera controls
    static constexpr float kDefaultCameraSpeed = 2.0F;
    static constexpr float kDefaultCameraSensitivity = 0.003F;
    float cameraSpeed{kDefaultCameraSpeed};
    float cameraSensitivity{kDefaultCameraSensitivity};
};

} // namespace project

