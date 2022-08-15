cmake_minimum_required(VERSION 3.6.0)
project(NesEmu)
set(CMAKE_CONFIGURATION_TYPES "Release" CACHE STRING "limited configs" FORCE)
include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/src
)
file(GLOB SRCS
	"${PROJECT_SOURCE_DIR}/include/*"
	"${PROJECT_SOURCE_DIR}/src/*"
)
add_executable(NesEmu ${SRCS})
find_package(SDL2 REQUIRED)
target_link_libraries(NesEmu ${SDL2_LIBRARIES})
find_package(SDL2_ttf REQUIRED)
target_link_libraries(NesEmu ${SDL2_TTF_LIBRARIES})