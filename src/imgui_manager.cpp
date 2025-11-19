#include <project/imgui_manager.hpp>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <raylib.h>

namespace project {

ImGuiManager::ImGuiManager() = default;

ImGuiManager::~ImGuiManager() {
    shutdown();
}

void ImGuiManager::initialize() {
    if (initialized) {
        return;
    }
    
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    
    // Enable keyboard and gamepad controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    // Setup ImGui style
    ImGui::StyleColorsDark();
    
    // Setup platform/renderer backends
    // raylib uses OpenGL 3.3, so we use the OpenGL3 backend
    // Note: raylib initializes OpenGL context, so we can use it directly
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // Setup input
    setupInput();
    
    initialized = true;
}

void ImGuiManager::beginFrame() {
    if (!initialized) {
        return;
    }
    
    // Update input
    updateInput();
    
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::endFrame() {
    if (!initialized) {
        return;
    }
    
    // Render ImGui
    ImGui::Render();
    
    // Get the draw data
    ImDrawData* drawData = ImGui::GetDrawData();
    if (drawData != nullptr) {
        // Render ImGui draw data
        ImGui_ImplOpenGL3_RenderDrawData(drawData);
    }
}

void ImGuiManager::shutdown() {
    if (!initialized) {
        return;
    }
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    
    initialized = false;
}

bool ImGuiManager::wantsCaptureMouse() const {
    if (!initialized) {
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool ImGuiManager::wantsCaptureKeyboard() const {
    if (!initialized) {
        return false;
    }
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

void ImGuiManager::setupInput() {
    ImGuiIO& io = ImGui::GetIO();
    
    // Setup display size
    io.DisplaySize = ImVec2(
        static_cast<float>(GetScreenWidth()),
        static_cast<float>(GetScreenHeight())
    );
    
    // Setup key map
    io.KeyMap[ImGuiKey_Tab] = KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = KEY_HOME;
    io.KeyMap[ImGuiKey_End] = KEY_END;
    io.KeyMap[ImGuiKey_Insert] = KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeypadEnter] = KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = KEY_A;
    io.KeyMap[ImGuiKey_C] = KEY_C;
    io.KeyMap[ImGuiKey_V] = KEY_V;
    io.KeyMap[ImGuiKey_X] = KEY_X;
    io.KeyMap[ImGuiKey_Y] = KEY_Y;
    io.KeyMap[ImGuiKey_Z] = KEY_Z;
}

void ImGuiManager::updateInput() {
    ImGuiIO& io = ImGui::GetIO();
    
    // Update display size
    io.DisplaySize = ImVec2(
        static_cast<float>(GetScreenWidth()),
        static_cast<float>(GetScreenHeight())
    );
    
    // Update delta time
    io.DeltaTime = GetFrameTime();
    if (io.DeltaTime <= 0.0F) {
        io.DeltaTime = 0.00001F;
    }
    
    // Update mouse position
    Vector2 mousePos = GetMousePosition();
    io.MousePos = ImVec2(mousePos.x, mousePos.y);
    
    // Update mouse buttons
    io.MouseDown[0] = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    io.MouseDown[1] = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    io.MouseDown[2] = IsMouseButtonDown(MOUSE_BUTTON_MIDDLE);
    
    // Update mouse wheel
    float wheel = GetMouseWheelMove();
    io.MouseWheel = wheel;
    
    // Update keyboard modifiers
    io.KeyCtrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    io.KeyShift = IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT);
    io.KeyAlt = IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT);
    io.KeySuper = IsKeyDown(KEY_LEFT_SUPER) || IsKeyDown(KEY_RIGHT_SUPER);
    
    // Update keyboard keys
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); ++i) {
        io.KeysDown[i] = IsKeyDown(i);
    }
    
    // Update text input
    int key = GetCharPressed();
    if (key > 0) {
        io.AddInputCharacter(static_cast<unsigned int>(key));
    }
}

} // namespace project

