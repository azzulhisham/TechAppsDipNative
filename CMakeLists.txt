cmake_minimum_required (VERSION 3.11)

project (TechApps.Dip
    VERSION 0.1
    DESCRIPTION "Dip Guide")

add_subdirectory (${PROJECT_SOURCE_DIR}/src/TechApps.Dip.Native/src)
add_subdirectory (${PROJECT_SOURCE_DIR}/src/TechApps.Dip.Native/jni)