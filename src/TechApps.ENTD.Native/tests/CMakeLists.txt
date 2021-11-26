cmake_minimum_required (VERSION 3.8)

project(tests)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PUBLIC ENTD) #add ENTD library
target_link_libraries(${PROJECT_NAME} PUBLIC Segy) #add ENTD Segy

# Tell C++ compiler to use C++11 features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

find_package(OpenMP)

if(OPENMP_FOUND)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

# include(GoogleTest)
# include(CTest)
# include(FetchContent)

# enable_testing()

# FetchContent_Declare(
#   googletest
#   GIT_REPOSITORY https://github.com/google/googletest.git
#   GIT_TAG        release-1.10.0
# )

# FetchContent_GetProperties(googletest)
# if(NOT googletest_POPULATED)
#   FetchContent_Populate(googletest)
#   add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
# endif()

# mark_as_advanced(
#     BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
#     gmock_build_tests gtest_build_samples gtest_build_tests
#     gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
# )

# gtest_discover_tests(${PROJECT_NAME}
#   WORKING_DIRECTORY ${PROJECT_DIR}
#   PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
# )

# set_target_properties(gtest PROPERTIES FOLDER extern)
# set_target_properties(gtest_main PROPERTIES FOLDER extern)
# set_target_properties(gmock PROPERTIES FOLDER extern)
# set_target_properties(gmock_main PROPERTIES FOLDER extern)
# set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER tests)

# target_link_libraries(${PROJECT_NAME} PUBLIC gtest)
# target_link_libraries(${PROJECT_NAME} PUBLIC gmock)
# target_link_libraries(${PROJECT_NAME} PUBLIC gtest_main)

# if (MSVC)
#   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
#   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
# endif()

# message("${PROJECT_DIR}")