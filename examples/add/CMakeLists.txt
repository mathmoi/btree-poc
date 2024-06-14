# Create the target
file(GLOB_RECURSE ADD_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(add ${ADD_SOURCES})

# compile statically
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")

# Add link libraries
target_link_libraries(add project-adder)
add_dependencies(add project-adder)