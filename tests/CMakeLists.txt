# Get Catch2
include(FetchContent)
FetchContent_Declare(Catch2
                     GIT_REPOSITORY https://github.com/catchorg/Catch2.git
                     GIT_TAG v3.6.0)
FetchContent_MakeAvailable(Catch2)

# Create the target
file(GLOB_RECURSE PROJECT_TESTS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(project-tests ${PROJECT_TESTS_SOURCES})

# Link libraries
target_link_libraries(project-tests PRIVATE Catch2::Catch2WithMain
                                            project-adder)
add_dependencies(project-tests project-adder)

# Configure CTest
include(CTest)
include(Catch)
catch_discover_tests(project-tests)