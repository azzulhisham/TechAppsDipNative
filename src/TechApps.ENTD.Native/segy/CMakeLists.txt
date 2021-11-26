cmake_minimum_required(VERSION 3.8)

project(Segy
    VERSION 1.0
    DESCRIPTION "Pagosa Segy Library")


#set(BUILD_SHARED_LIBS ON)
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True)

file(GLOB SRC_FILES *.cpp)
add_library(${PROJECT_NAME} ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11) # Tell C++ compiler to use C++11 features

#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
#  COMMAND "${CMAKE_COMMAND}" -E copy 
#     "$<TARGET_FILE:${PROJECT_NAME}>"
#     "../tests/$<CONFIGURATION>/$<TARGET_FILE_NAME:${PROJECT_NAME}>" 
#  COMMENT "Copying to output directory")
