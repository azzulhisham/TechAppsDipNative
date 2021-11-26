cmake_minimum_required (VERSION 3.8)

project(DIP
	VERSION 1.0
	DESCRIPTION "DIP")

add_executable(${PROJECT_NAME} DipSlopePWD.cpp)

# PUBLIC needed to make library available elsewhere in project
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})


if (WIN32)
	find_package(MKL CONFIG)	
	if (NOT MKL_ROOT)
		set(MKL_ROOT $ENV{MKL_ROOT})	
		message("MKL_ROOT $ENV{MKL_ROOT}")
	endif()
	target_include_directories(${PROJECT_NAME} PUBLIC ${MKL_ROOT}/include)
	target_link_libraries(${PROJECT_NAME}
	 	${MKL_ROOT}/lib/intel64/mkl_core.lib
	 	${MKL_ROOT}/lib/intel64/mkl_intel_lp64.lib
	 	${MKL_ROOT}/lib/intel64/mkl_sequential.lib
	 	#${MKL_ROOT}/lib/intel64/mkl_intel_thread.lib
	 	#${MKL_ROOT}/../../compiler/latest/windows/compiler/lib/intel64_win/libiomp5md.lib
	)	
else()
	target_include_directories(${PROJECT_NAME} PUBLIC $ENV{MKLROOT}/include)
	#target_link_libraries(${PROJECT_NAME} "-DMKL_ILP64 -mkl=parallel -liomp5 -lpthread -lm -ldl -qopenmp -g") 
	target_link_libraries(${PROJECT_NAME} "-lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl -qopenmp") 
endif()

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11) # Tell C++ compiler to use C++11 features
