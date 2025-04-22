# Add an executable
add_executable(OffsetMain Det.cpp)

target_link_libraries(OffsetMain PUBLIC manifold assimp)