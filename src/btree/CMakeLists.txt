file(GLOB_RECURSE BTREE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(btree ${BTREE_SOURCES})

target_include_directories(btree PUBLIC ${PROJECT_SOURCE_DIR}/include)