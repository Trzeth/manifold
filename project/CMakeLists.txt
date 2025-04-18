# Add an executable
add_executable(OffsetMain Fillet.cpp)

target_link_libraries(OffsetMain PUBLIC manifold assimp)