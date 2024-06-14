file(GLOB_RECURSE MKVDB_PAGER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(project-adder ${MKVDB_PAGER_SOURCES})

target_include_directories(project-adder PUBLIC ${PROJECT_SOURCE_DIR}/include)