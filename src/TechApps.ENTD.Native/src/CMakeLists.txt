cmake_minimum_required (VERSION 3.8)

project (ENTD)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS True)

add_library (${PROJECT_NAME} ArrayToBit.cpp Segment.cpp Entd.cpp cubing.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
 COMMAND "${CMAKE_COMMAND}" -E copy 
    "$<TARGET_FILE:${PROJECT_NAME}>"
    "../tests/$<CONFIGURATION>/$<TARGET_FILE_NAME:${PROJECT_NAME}>" 
 COMMENT "Copying to output directory")
#
#if (WIN32)
#	set(MKL_ROOT "C:/Program Files (x86)/IntelSWTools/compilers_and_libraries_2018/windows")
#	set(MKL_INCLUDES "${MKL_ROOT}/mkl/include")
#	set(MKL_LIBRARIES "${MKL_ROOT}/mkl/lib/intel64")
#	set(MKL_COMPILERS "${MKL_ROOT}/compiler/lib/intel64_win")
#
#	target_include_directories(${PROJECT_NAME} PUBLIC ${MKL_INCLUDES})
#	target_link_libraries(${PROJECT_NAME}
#		${MKL_LIBRARIES}/mkl_core.lib
#		${MKL_LIBRARIES}/mkl_intel_lp64.lib
#		${MKL_LIBRARIES}/mkl_intel_thread.lib
#		${MKL_COMPILERS}/libiomp5md.lib)
#else()
#	target_include_directories(${PROJECT_NAME} PUBLIC $ENV{MKLROOT}/include)
#	target_link_libraries(${PROJECT_NAME} "-DMKL_ILP64 -mkl=parallel -liomp5 -lpthread -lm -ldl -qopenmp -g") 
#endif()