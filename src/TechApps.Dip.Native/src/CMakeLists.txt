cmake_minimum_required (VERSION 3.8)

project(Dip
	VERSION 1.0
	DESCRIPTION "Dip Guide Implementation")

add_library(${PROJECT_NAME} Dip.cpp)

# PUBLIC needed to make library available elsewhere in project
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
# Tell C++ compiler to use C++11 features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

if (WIN32)
	find_package(MKL CONFIG)	
	target_include_directories(${PROJECT_NAME} PUBLIC ${MKL_ROOT}/include)
	target_link_libraries(${PROJECT_NAME}
	 	${MKL_ROOT}/lib/intel64/mkl_core.lib
	 	${MKL_ROOT}/lib/intel64/mkl_intel_lp64.lib
	 	${MKL_ROOT}/lib/intel64/mkl_sequential.lib	 	
	)	
else()
	target_include_directories(${PROJECT_NAME} PUBLIC $ENV{MKLROOT}/include)	
	target_link_libraries(${PROJECT_NAME} "-lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl -qopenmp") 
endif()


