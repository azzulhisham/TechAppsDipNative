cmake_minimum_required (VERSION 3.8)

project(ENTD_HPC
	VERSION 1.0
	DESCRIPTION "HPC of Enhanced Network Tortuosity Decomposition")

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

target_link_libraries(${PROJECT_NAME} PRIVATE ${MPI_C_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE ENTD) #add ENTD library
target_link_libraries(${PROJECT_NAME} PRIVATE Segy) #add ENTD Segy

# Tell C++ compiler to use C++11 features
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

# target_include_directories(${PROJECT_NAME} PUBLIC $ENV{MKLROOT}/include)
# target_link_libraries(${PROJECT_NAME} "-DMKL_ILP64 -mkl=parallel -liomp5 -lpthread -lm -ldl -qopenmp -g") 
