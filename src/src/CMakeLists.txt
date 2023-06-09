add_executable(trees WIN32 config.cpp auxillary.cpp scc.cpp trees.cpp app.cpp main.cpp)
target_link_libraries(trees PRIVATE sfml-graphics)
target_link_libraries(trees PRIVATE ImGui-SFML::ImGui-SFML)
