set(sources
    src/tmp.cpp
    src/scene.cpp
    src/scene_manager.cpp
    src/tree_scene.cpp
    src/geometric_scene.cpp
    src/bullet_physics_scene.cpp
    src/imgui_manager.cpp
    src/gui_controls.cpp
)

set(exe_sources
		src/main.cpp
		${sources}
)

set(headers
    include/project/tmp.hpp
    include/project/scene.hpp
    include/project/scene_strategy.hpp
    include/project/scene_manager.hpp
    include/project/tree_scene.hpp
    include/project/geometric_scene.hpp
    include/project/bullet_physics_scene.hpp
    include/project/imgui_manager.hpp
    include/project/gui_controls.hpp
)

set(test_sources
  src/tmp_test.cpp
)
