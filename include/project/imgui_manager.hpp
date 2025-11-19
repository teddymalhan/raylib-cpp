#pragma once

#include <raylib.h>
#include <string>
#include <functional>

namespace project {

/// Manages Dear ImGui integration with raylib
/// Handles initialization, frame updates, and rendering
class ImGuiManager {
public:
    ImGuiManager();
    ~ImGuiManager();
    
    // Rule of Five: disable copy, allow move
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;
    ImGuiManager(ImGuiManager&&) noexcept = default;
    ImGuiManager& operator=(ImGuiManager&&) noexcept = default;
    
    /// Initialize ImGui with raylib window
    /// Must be called after raylib window is initialized
    void initialize();
    
    /// Begin a new ImGui frame (call at start of each frame)
    void beginFrame();
    
    /// End ImGui frame and render (call at end of each frame)
    void endFrame();
    
    /// Shutdown ImGui (called automatically in destructor)
    void shutdown();
    
    /// Check if ImGui wants to capture mouse input
    [[nodiscard]] bool wantsCaptureMouse() const;
    
    /// Check if ImGui wants to capture keyboard input
    [[nodiscard]] bool wantsCaptureKeyboard() const;
    
    /// Check if ImGui is initialized
    [[nodiscard]] bool isInitialized() const noexcept { return initialized; }

private:
    bool initialized{false};
    
    void setupInput();
    void updateInput();
};

} // namespace project

