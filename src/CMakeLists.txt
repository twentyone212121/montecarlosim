add_executable(imgui-sfml-example
  main.cpp
        MonteCarlo.cpp MonteCarlo.h)

# Yep, that's it!
target_link_libraries(imgui-sfml-example
  PUBLIC
    ImGui-SFML::ImGui-SFML
)

include(Install.cmake)
