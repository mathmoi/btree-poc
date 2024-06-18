# Create the target
file(GLOB_RECURSE BTREE_DEMO_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(btree-demo ${BTREE_DEMO_SOURCES})

# compile statically
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# Add link libraries
target_link_libraries(btree-demo btree)
add_dependencies(btree-demo btree)