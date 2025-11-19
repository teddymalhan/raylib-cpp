#include <project/tree_scene.hpp>
#include <iostream>

namespace project {

TreeScene::TreeScene(const std::string& modelPath) 
    : modelPath(modelPath) {
}

TreeScene::~TreeScene() {
    cleanup();
}

void TreeScene::initialize() {
    if (isInitialized) {
        return;
    }
    
    // Check if model file exists
    if (!FileExists(modelPath.c_str())) {
        std::cerr << "Model file not found: " << modelPath << '\n';
        return;
    }
    
    // Load the 3D model
    std::cout << "Loading model: " << modelPath << '\n';
    Model model = LoadModel(modelPath.c_str());
    
    if (!IsModelValid(model)) {
        std::cerr << "Failed to load model: " << modelPath << '\n';
        return;
    }
    
    std::cout << "Model loaded successfully!\n";
    
    // Add the model to the scene at the origin
    scene.addObject(model, Vector3{0.0F, 0.0F, 0.0F}, kModelScale, "tree-main");
    
    isInitialized = true;
}

void TreeScene::cleanup() {
    if (isInitialized) {
        scene.clear();
        isInitialized = false;
    }
}

void TreeScene::draw() const {
    if (isInitialized) {
        scene.draw();
    }
}

} // namespace project

