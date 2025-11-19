#include <project/gui_controls.hpp>
#include <imgui.h>
#include <cmath>

namespace project {

void GuiControls::renderControlPanel(SceneManager& sceneManager, Camera3D& camera) {
    if (!showControlPanel) {
        return;
    }
    
    ImGui::Begin("Control Panel", &showControlPanel);
    
    // Scene selection
    if (ImGui::CollapsingHeader("Scene Management", ImGuiTreeNodeFlags_DefaultOpen)) {
        const size_t sceneCount = sceneManager.getSceneCount();
        const size_t currentIndex = sceneManager.getCurrentSceneIndex();
        
        if (sceneCount > 0) {
            // Show current scene info
            const auto* currentScene = sceneManager.getCurrentScene();
            if (currentScene != nullptr) {
                ImGui::Text("Current: %s", currentScene->getName());
            }
            ImGui::Text("Scene %zu of %zu", currentIndex + 1, sceneCount);
            
            ImGui::Spacing();
            
            ImGui::Spacing();
            
            // Navigation buttons
            if (ImGui::Button("Previous Scene")) {
                sceneManager.switchToPreviousScene();
            }
            ImGui::SameLine();
            if (ImGui::Button("Next Scene")) {
                sceneManager.switchToNextScene();
            }
        } else {
            ImGui::Text("No scenes registered");
        }
    }
    
    // Camera controls
    if (ImGui::CollapsingHeader("Camera Controls", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Camera position
        float pos[3] = {camera.position.x, camera.position.y, camera.position.z};
        if (ImGui::DragFloat3("Position", pos, 0.1F, -100.0F, 100.0F, "%.2f")) {
            camera.position.x = pos[0];
            camera.position.y = pos[1];
            camera.position.z = pos[2];
        }
        
        // Camera target
        float target[3] = {camera.target.x, camera.target.y, camera.target.z};
        if (ImGui::DragFloat3("Target", target, 0.1F, -100.0F, 100.0F, "%.2f")) {
            camera.target.x = target[0];
            camera.target.y = target[1];
            camera.target.z = target[2];
        }
        
        // Camera FOV
        if (ImGui::SliderFloat("FOV", &camera.fovy, 10.0F, 120.0F, "%.1f")) {
            // FOV updated
        }
        
        // Camera speed
        ImGui::SliderFloat("Camera Speed", &cameraSpeed, 0.1F, 10.0F, "%.2f");
        ImGui::SliderFloat("Camera Sensitivity", &cameraSensitivity, 0.001F, 0.01F, "%.4f");
    }
    
    // Application info
    if (ImGui::CollapsingHeader("Application Info")) {
        ImGui::Text("FPS: %d", GetFPS());
        ImGui::Text("Frame Time: %.3f ms", GetFrameTime() * 1000.0F);
        ImGui::Text("Screen Size: %d x %d", GetScreenWidth(), GetScreenHeight());
        ImGui::Spacing();
        ImGui::Text("Controls:");
        ImGui::BulletText("F4: Toggle camera tracking");
        ImGui::BulletText("Click non-GUI area: Re-enable camera");
        ImGui::BulletText("WASD: Move camera");
        ImGui::BulletText("Mouse: Look around");
        ImGui::BulletText("ESC: Exit application");
    }
    
    ImGui::End();
}

void GuiControls::renderDebugPanel() {
    if (!showDebugPanel) {
        return;
    }
    
    ImGui::Begin("Debug Info", &showDebugPanel);
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                1000.0F / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate);
    
    ImGui::Spacing();
    
    if (ImGui::CollapsingHeader("ImGui Metrics")) {
        ImGui::Text("Active Windows: %d", ImGui::GetIO().MetricsRenderWindows);
    }
    
    ImGui::End();
}

void GuiControls::renderSceneInfo(SceneManager& sceneManager) {
    if (!showSceneInfo) {
        return;
    }
    
    ImGui::Begin("Scene Information", &showSceneInfo);
    
    const auto* currentScene = sceneManager.getCurrentScene();
    if (currentScene != nullptr) {
        ImGui::Text("Current Scene: %s", currentScene->getName());
        ImGui::Text("Scene Index: %zu / %zu",
                    sceneManager.getCurrentSceneIndex() + 1,
                    sceneManager.getSceneCount());
    } else {
        ImGui::Text("No active scene");
    }
    
    ImGui::Spacing();
    
    if (ImGui::Button("Show ImGui Demo")) {
        showDemo = !showDemo;
    }
    
    ImGui::End();
}

void GuiControls::showDemoWindow() {
    if (showDemo) {
        ImGui::ShowDemoWindow(&showDemo);
    }
}

} // namespace project

