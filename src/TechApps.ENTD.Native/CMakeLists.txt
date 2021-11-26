cmake_minimum_required (VERSION 3.11)

project (TechApps.ENTD.Native
    VERSION 0.1
    DESCRIPTION "Enhanced NTD")

add_subdirectory (${PROJECT_SOURCE_DIR}/src)
add_subdirectory (${PROJECT_SOURCE_DIR}/segy)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
    add_subdirectory (${PROJECT_SOURCE_DIR}/mpi)
else()
#enable_testing()
    add_subdirectory(tests)
endif()
