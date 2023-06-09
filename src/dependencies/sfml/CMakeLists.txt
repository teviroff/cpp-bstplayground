message(STATUS "Fetching SFML")

set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(sfml)
