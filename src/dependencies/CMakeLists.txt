include(FetchContent)

FetchContent_Declare(
    sfml
    GIT_REPOSITORY "https://github.com/SFML/SFML"
    GIT_TAG 2f11710abc5aa478503a7ff3f9e654bd2078ebab
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui"
    GIT_TAG 530134d1253c97bd33268b5932b6c67d0ad47d09
)

FetchContent_Declare(
  imgui-sfml
  GIT_REPOSITORY "https://github.com/SFML/imgui-sfml"
  GIT_TAG ea890431d2c47e02f84b08d1467a881b88732102
)

add_subdirectory(sfml)
FetchContent_MakeAvailable(imgui)
add_subdirectory(imgui-sfml)
