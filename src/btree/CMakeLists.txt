#file(GLOB_RECURSE BTREE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
#add_library(btree ${BTREE_SOURCES})
add_library(btree INTERFACE)

file(GLOB_RECURSE BTREE_HEADERS ${PROJECT_SOURCE_DIR}/include/btree/*.hpp)
target_sources(btree INTERFACE ${BTREE_HEADERS})

target_include_directories(btree INTERFACE ${PROJECT_SOURCE_DIR}/include)