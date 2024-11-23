cmake_minimum_required(VERSION 3.30.3)

project(SFMLflappybird LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# get SFML if not have n stuff
include(FetchContent)
FetchContent_Declare(
	SFML
	GIT_REPOSITORY https://github.com/SFML/SFML.git
	GIT_TAG 2.6.x
)
FetchContent_MakeAvailable(SFML)

# build bin and stuff
add_executable(${PROJECT_NAME} src/main.cpp src/gamefunctions.cpp src/missile.cpp src/pipe.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE sfml-window sfml-system sfml-graphics sfml-audio)
