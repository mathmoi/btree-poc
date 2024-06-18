# Get Catch2
include(FetchContent)
FetchContent_Declare(Catch2
                     GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                     GIT_TAG v3.6.0)
FetchContent_MakeAvailable(Catch2)

# Create the target
file(GLOB_RECURSE BTREE_TESTS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(btree-tests ${BTREE_TESTS_SOURCES})

# Link libraries
target_link_libraries(btree-tests PRIVATE Catch2::Catch2WithMain
                                          btree)
add_dependencies(btree-tests btree)

# Configure CTest
include(CTest)
include(Catch)
catch_discover_tests(btree-tests)