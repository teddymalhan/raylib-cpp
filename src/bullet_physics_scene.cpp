#include <project/bullet_physics_scene.hpp>
#include <btBulletDynamicsCommon.h>
#include <raymath.h>
#include <cmath>
#include <iostream>

namespace project {

BulletPhysicsScene::BulletPhysicsScene() = default;

BulletPhysicsScene::~BulletPhysicsScene() {
    cleanup();
}

void BulletPhysicsScene::initialize() {
    if (isInitialized) {
        return;
    }
    
    setupPhysicsWorld();
    createGroundPlane();
    createCharacter();
    
    isInitialized = true;
}

void BulletPhysicsScene::cleanup() {
    if (!isInitialized) {
        return;
    }
    
    // Mark as uninitialized first to prevent re-entry
    isInitialized = false;
    
    // Cleanup all entities - components will handle their own cleanup
    // First remove all rigid bodies from the physics world
    auto physicsView = registry.view<PhysicsBody>();
    for (auto entity : physicsView) {
        const auto& physicsBody = physicsView.get<PhysicsBody>(entity);
        if (physicsBody.rigidBody != nullptr && dynamicsWorld != nullptr) {
            dynamicsWorld->removeRigidBody(physicsBody.rigidBody);
        }
    }
    
    // Clear the registry (this will destroy all entities and components)
    registry.clear();
    
    // Cleanup physics world
    cleanupPhysicsWorld();
}

void BulletPhysicsScene::setupPhysicsWorld() {
    // Create collision configuration
    collisionConfiguration = new btDefaultCollisionConfiguration();
    
    // Create dispatcher
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    // Create broadphase
    overlappingPairCache = new btDbvtBroadphase();
    
    // Create constraint solver
    constraintSolver = new btSequentialImpulseConstraintSolver();
    
    // Create dynamics world
    dynamicsWorld = new btDiscreteDynamicsWorld(
        dispatcher,
        overlappingPairCache,
        constraintSolver,
        collisionConfiguration
    );
    
    // Set gravity (Y-axis up in raylib, Y-axis up in Bullet by default)
    constexpr float kGravity = -9.8F;
    dynamicsWorld->setGravity(btVector3(0.0F, kGravity, 0.0F));
}

void BulletPhysicsScene::createGroundPlane() {
    // Create a large ground plane
    constexpr float kGroundHalfExtentsX = 20.0F;
    constexpr float kGroundHalfExtentsY = 0.5F;
    constexpr float kGroundHalfExtentsZ = 20.0F;
    constexpr float kGroundY = -0.5F;
    
    // Create collision shape
    btCollisionShape* groundShape = new btBoxShape(
        btVector3(kGroundHalfExtentsX, kGroundHalfExtentsY, kGroundHalfExtentsZ)
    );
    
    // Create ground model
    constexpr float kCubeSize = 1.0F;
    Mesh cubeMesh = GenMeshCube(kCubeSize, kCubeSize, kCubeSize);
    Model groundModel = LoadModelFromMesh(cubeMesh);
    const bool hasGroundModel = IsModelValid(groundModel);
    
    // Create ground entity
    const Vector3 groundPosition{0.0F, kGroundY, 0.0F};
    const Vector3 groundScale{40.0F, 1.0F, 40.0F};
    
    auto groundEntity = createPhysicsEntity(
        groundPosition,
        groundShape,
        0.0F,  // Static (mass = 0)
        hasGroundModel,
        groundModel,
        DARKGREEN,
        true  // isGround
    );
    
    // Set ground scale
    if (registry.all_of<Transform>(groundEntity)) {
        auto& transform = registry.get<Transform>(groundEntity);
        transform.scale = groundScale;
    }
}

void BulletPhysicsScene::createFallingBoxes() {
    constexpr int kBoxCount = 10;
    constexpr float kBoxSize = 0.5F;
    constexpr float kBoxMass = 1.0F;
    constexpr float kStartHeight = 5.0F;
    constexpr float kSpacing = 2.0F;
    
    // Create a cube mesh for the boxes (shared mesh)
    Mesh cubeMesh = GenMeshCube(kBoxSize * 2.0F, kBoxSize * 2.0F, kBoxSize * 2.0F);
    
    // Create boxes in a grid pattern
    const int kGridSize = static_cast<int>(std::sqrt(static_cast<float>(kBoxCount)));
    int boxIndex = 0;
    
    for (int i = 0; i < kGridSize && boxIndex < kBoxCount; ++i) {
        for (int j = 0; j < kGridSize && boxIndex < kBoxCount; ++j) {
            // Calculate position
            const float posX = (static_cast<float>(i) - static_cast<float>(kGridSize) / 2.0F) * kSpacing;
            const float posZ = (static_cast<float>(j) - static_cast<float>(kGridSize) / 2.0F) * kSpacing;
            
            // Create collision shape
            btCollisionShape* boxShape = new btBoxShape(
                btVector3(kBoxSize, kBoxSize, kBoxSize)
            );
            
            // Create model (each box gets its own model instance)
            Model boxModel = LoadModelFromMesh(cubeMesh);
            const bool hasBoxModel = IsModelValid(boxModel);
            
            // Vary colors
            const float hue = static_cast<float>(boxIndex) / static_cast<float>(kBoxCount);
            const Color boxColor = ColorFromHSV(hue * 360.0F, 0.8F, 0.9F);
            
            // Create box entity
            const Vector3 boxPosition{posX, kStartHeight, posZ};
            createPhysicsEntity(
                boxPosition,
                boxShape,
                kBoxMass,
                hasBoxModel,
                boxModel,
                boxColor,
                false  // not ground
            );
            
            ++boxIndex;
        }
    }
}

void BulletPhysicsScene::createCharacter() {
    constexpr const char* kCharacterPath = "assets/characters/character-a.glb";
    constexpr float kCharacterMass = 0.0F;  // Static (doesn't fall)
    constexpr float kCharacterDistance = 3.0F;  // Distance from origin (camera looks at origin)
    constexpr float kGroundY = -0.5F;  // Ground plane Y position (matches createGroundPlane)
    
    std::cout << "=== Creating Character ===" << std::endl;
    std::cout << "Character path: " << kCharacterPath << std::endl;
    
    // Check if model file exists
    if (!FileExists(kCharacterPath)) {
        std::cout << "ERROR: Character model not found at: " << kCharacterPath << std::endl;
        std::cout << "Current working directory check..." << std::endl;
        return;
    }
    
    std::cout << "Character file exists, loading model..." << std::endl;
    
    // Load the character model
    Model characterModel = LoadModel(kCharacterPath);
    const bool hasCharacterModel = IsModelValid(characterModel);
    
    std::cout << "Model loaded. IsValid: " << (hasCharacterModel ? "YES" : "NO") << std::endl;
    std::cout << "Model mesh count: " << characterModel.meshCount << std::endl;
    std::cout << "Model material count: " << characterModel.materialCount << std::endl;
    
    if (!hasCharacterModel) {
        std::cout << "ERROR: Failed to load character model from: " << kCharacterPath << std::endl;
        return;
    }
    
    // Get bounding box to determine collision shape size
    const BoundingBox boundingBox = GetModelBoundingBox(characterModel);
    const Vector3 boundingSize = Vector3Subtract(boundingBox.max, boundingBox.min);
    
    std::cout << "Character model loaded. Bounding box size: (" 
              << boundingSize.x << ", " << boundingSize.y << ", " << boundingSize.z << ")" << std::endl;
    
    // Calculate capsule dimensions from bounding box
    // Use the average of width and depth for radius, and height for capsule height
    const float capsuleRadius = std::max(boundingSize.x, boundingSize.z) * 0.5F;
    const float capsuleHeight = std::max(boundingSize.y - (capsuleRadius * 2.0F), 0.1F);
    
    // Create capsule collision shape (better for characters than boxes)
    // btCapsuleShape is Y-axis aligned by default (upright for characters)
    btCollisionShape* capsuleShape = new btCapsuleShape(
        static_cast<btScalar>(capsuleRadius),
        static_cast<btScalar>(capsuleHeight)
    );
    
    // Position character on the ground
    // Ground is at Y = -0.5, and the ground plane has a half-extent of 0.5, so the top of the ground is at Y = 0.0
    // We want the bottom of the character's bounding box to be at the top of the ground (Y = 0.0)
    // The bounding box min.y is relative to the model's origin, so we need to place the model origin
    // such that the bottom of the bounding box sits on the ground
    const float groundTopY = 0.0F;  // Top of ground plane (ground center Y = -0.5, half-extent = 0.5)
    const float characterBottomOffset = boundingBox.min.y;  // Offset from model origin to bottom of bounding box
    const float characterY = groundTopY - characterBottomOffset;  // Position so bottom sits on ground
    const Vector3 characterPosition{0.0F, characterY, 0.0F};  // Place at origin for visibility
    
    std::cout << "Ground top Y: " << groundTopY << std::endl;
    std::cout << "Character bounding box min Y: " << boundingBox.min.y << std::endl;
    std::cout << "Character Y position calculated: " << characterY << std::endl;
    
    std::cout << "Character positioned at: (" << characterPosition.x << ", " 
              << characterPosition.y << ", " << characterPosition.z << ")" << std::endl;
    std::cout << "Character bounding box min: (" << boundingBox.min.x << ", " 
              << boundingBox.min.y << ", " << boundingBox.min.z << ")" << std::endl;
    std::cout << "Character bounding box max: (" << boundingBox.max.x << ", " 
              << boundingBox.max.y << ", " << boundingBox.max.z << ")" << std::endl;
    
    const auto characterEntity = createPhysicsEntity(
        characterPosition,
        capsuleShape,
        kCharacterMass,
        hasCharacterModel,
        characterModel,
        WHITE,  // Use model's original colors
        false   // not ground
    );
    
    // Add Name component to identify the character
    registry.emplace<Name>(characterEntity, "character-a");
    
    // Set a visible scale for the character (GLB models might be very small or large)
    if (registry.all_of<Transform>(characterEntity)) {
        auto& transform = registry.get<Transform>(characterEntity);
        // Try a reasonable scale - adjust if needed
        const float modelScale = 1.0F / std::max({boundingSize.x, boundingSize.y, boundingSize.z, 1.0F});
        transform.scale = Vector3{1.0F, 1.0F, 1.0F};  // Start with 1:1 scale
        std::cout << "Character scale set to: (" << transform.scale.x << ", " 
                  << transform.scale.y << ", " << transform.scale.z << ")" << std::endl;
    }
    
    // Debug: Print model info
    std::cout << "Character entity created with " << characterModel.meshCount << " meshes" << std::endl;
    std::cout << "Character entity created with " << characterModel.materialCount << " materials" << std::endl;
}

entt::entity BulletPhysicsScene::createPhysicsEntity(
    const Vector3& position,
    btCollisionShape* collisionShape,
    float mass,
    bool hasModel,
    const Model& model,
    const Color& color,
    bool isGround
) {
    // Create entity
    const auto entity = registry.create();
    
    // Add Transform component
    auto& transform = registry.emplace<Transform>(entity);
    transform.position = position;
    transform.rotation = Quaternion{0.0F, 0.0F, 0.0F, 1.0F};  // Identity quaternion (x, y, z, w)
    transform.scale = Vector3{1.0F, 1.0F, 1.0F};
    
    // Setup Bullet Physics transform
    btTransform bulletTransform;
    bulletTransform.setIdentity();
    bulletTransform.setOrigin(btVector3(position.x, position.y, position.z));
    
    // Calculate inertia
    btVector3 localInertia(0.0, 0.0, 0.0);
    const bool isStatic = (mass == 0.0F);
    if (!isStatic) {
        collisionShape->calculateLocalInertia(mass, localInertia);
    }
    
    // Create motion state
    btDefaultMotionState* motionState = new btDefaultMotionState(bulletTransform);
    
    // Create rigid body
    btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
        mass,
        motionState,
        collisionShape,
        localInertia
    );
    
    btRigidBody* rigidBody = new btRigidBody(rigidBodyCI);
    
    // Add rigid body to physics world
    if (dynamicsWorld != nullptr) {
        dynamicsWorld->addRigidBody(rigidBody);
    }
    
    // Add PhysicsBody component
    auto& physicsBody = registry.emplace<PhysicsBody>(entity);
    physicsBody.rigidBody = rigidBody;
    physicsBody.collisionShape = collisionShape;
    physicsBody.motionState = motionState;
    physicsBody.mass = mass;
    physicsBody.isStatic = isStatic;
    
    // Add Renderable component if model is provided
    if (hasModel) {
        auto& renderable = registry.emplace<Renderable>(entity);
        renderable.model = model;
        renderable.color = color;
        renderable.hasModel = IsModelValid(model);
    }
    
    // Add Ground tag if this is ground
    if (isGround) {
        registry.emplace<Ground>(entity);
    }
    
    return entity;
}

void BulletPhysicsScene::update() {
    if (!isInitialized || dynamicsWorld == nullptr) {
        return;
    }
    
    // Update physics system (steps simulation and syncs transforms)
    const float deltaTime = GetFrameTime();
    PhysicsSystem::update(registry, dynamicsWorld, deltaTime);
}

void BulletPhysicsScene::draw() const {
    if (!isInitialized) {
        return;
    }
    
    // Draw ground entities first
    RenderSystem::drawGround(registry);
    
    // Draw all other renderable entities
    RenderSystem::draw(registry);
    
    // Draw debug markers and direct rendering for characters
    auto characterView = registry.view<const Transform, const Renderable, const Name>();
    size_t characterCount = 0;
    bool hasCharacters = false;
    
    for (auto entity : characterView) {
        hasCharacters = true;
        characterCount++;
        const auto& transform = characterView.get<Transform>(entity);
        const auto& renderable = characterView.get<Renderable>(entity);
        
        // Draw a small debug cube at character position (optional - can be removed)
        // DrawCube(transform.position, 0.2F, 0.2F, 0.2F, BLUE);
        
        if (renderable.hasModel) {
            // Draw bounding box for character
            const BoundingBox bbox = GetModelBoundingBox(renderable.model);
            const Vector3 bboxSize = Vector3Subtract(bbox.max, bbox.min);
            const Vector3 bboxCenter = Vector3Add(bbox.min, Vector3Scale(bboxSize, 0.5F));
            const Vector3 worldPos = Vector3Add(transform.position, bboxCenter);
            
            // Draw bounding box
            DrawBoundingBox(BoundingBox{
                Vector3Subtract(worldPos, Vector3Scale(bboxSize, 0.5F)),
                Vector3Add(worldPos, Vector3Scale(bboxSize, 0.5F))
            }, YELLOW);
            
            // Draw a small debug sphere at character position (optional - can be removed)
            // DrawSphere(transform.position, 0.1F, RED);
            
            // Draw model with normal scale
            // Use the transform scale if valid, otherwise use 1.0
            const Vector3 drawScale = (transform.scale.x > 0.0F && transform.scale.y > 0.0F && transform.scale.z > 0.0F)
                ? transform.scale
                : Vector3{1.0F, 1.0F, 1.0F};
            
            // Draw the model once with proper scale
            DrawModelEx(
                renderable.model,
                transform.position,
                Vector3{0.0F, 1.0F, 0.0F},  // Y-axis
                0.0F,  // No rotation
                drawScale,
                WHITE
            );
        } else {
            // Model is invalid - draw a warning marker
            DrawSphere(transform.position, 0.5F, ORANGE);
        }
    }
    
    if (!hasCharacters) {
        // No characters found - draw a test cube at origin to verify rendering works
        DrawCube(Vector3{0.0F, 0.0F, 0.0F}, 1.0F, 1.0F, 1.0F, MAGENTA);
        DrawCubeWires(Vector3{0.0F, 0.0F, 0.0F}, 1.0F, 1.0F, 1.0F, RED);
    }
    
    // Draw wireframes for physics objects (optional visual aid)
    // Skip characters (entities with Name component) to avoid obscuring the model
    auto view = registry.view<const Transform, const PhysicsBody>(entt::exclude<Ground>);
    for (auto entity : view) {
        // Skip entities with Name component (characters)
        if (registry.all_of<Name>(entity)) {
            continue;
        }
        
        const auto& transform = view.get<Transform>(entity);
        constexpr float kBoxSize = 0.5F;
        DrawCubeWiresV(
            transform.position,
            Vector3{kBoxSize * 2.0F, kBoxSize * 2.0F, kBoxSize * 2.0F},
            DARKGRAY
        );
    }
}

void BulletPhysicsScene::cleanupPhysicsWorld() {
    // Delete dynamics world components
    if (dynamicsWorld != nullptr) {
        delete dynamicsWorld;
        dynamicsWorld = nullptr;
    }
    
    if (constraintSolver != nullptr) {
        delete constraintSolver;
        constraintSolver = nullptr;
    }
    
    if (overlappingPairCache != nullptr) {
        delete overlappingPairCache;
        overlappingPairCache = nullptr;
    }
    
    if (dispatcher != nullptr) {
        delete dispatcher;
        dispatcher = nullptr;
    }
    
    if (collisionConfiguration != nullptr) {
        delete collisionConfiguration;
        collisionConfiguration = nullptr;
    }
}

} // namespace project
