cmake_minimum_required(VERSION 3.30.3)

project(SFMLflappybird LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# get SFML if not have n stuff
find_package(SFML 3 QUIET COMPONENTS Graphics Audio)
if(NOT SFML_FOUND)
	message(WARNING "SFML3 local installation not found")
	message("downloading SFML3 from github")
	include(FetchContent)
	FetchContent_Declare(
		SFML
		GIT_REPOSITORY https://github.com/SFML/SFML.git
		GIT_TAG 3.0.x
	)
	FetchContent_MakeAvailable(SFML)
endif()

# build bin and stuff
add_executable(${PROJECT_NAME} src/main.cpp src/gamefunctions.cpp src/missile.cpp src/pipe.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE SFML::Graphics SFML::Audio)
