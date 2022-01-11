cmake_minimum_required (VERSION 3.8)

project(DipJni
	VERSION 1.0
	DESCRIPTION "Dip Jni")

add_library(${PROJECT_NAME} SHARED DipJni.cpp)


# PUBLIC needed to make library available elsewhere in project
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11) # Tell C++ compiler to use C++11 features
target_link_libraries(${PROJECT_NAME} PRIVATE Dip) #add main library

find_package(JNI REQUIRED)
include_directories(SYSTEM ${JNI_INCLUDE_DIRS})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD	
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${PROJECT_NAME}.dll"
		"${CMAKE_CURRENT_SOURCE_DIR}/../../TechApps.Dip"
	COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/${PROJECT_NAME}.pdb"
		"${CMAKE_CURRENT_SOURCE_DIR}/../../TechApps.Dip"
)