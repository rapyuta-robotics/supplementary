cmake_minimum_required(VERSION 3.5.1)
project(autodiff)

## Use c++ 11x std
set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
if (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")
	set(CMAKE_CXX_COMPILER_ARG1 "-std=c++11" CACHE STRING "C++ version for eclipse" FORCE)
	set(CMAKE_ECLIPSE_VERSION "4.5" CACHE STRING "Eclipse version" FORCE)
	add_definitions (-DCMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT=TRUE)
endif (${CMAKE_EXTRA_GENERATOR} MATCHES "Eclipse CDT4")


set(CMAKE_CXX_FLAGS "-Werror-implicit-function-declaration -Werror=return-type -Wno-reorder ${CMAKE_CXX_FLAGS}") #-Wall 

set(OPTIMIZATION "-O2 -finline-functions -funroll-loops -fomit-frame-pointer")
#--ffast-math
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG ${CPP_STD} ${OPTIMIZATION}")



#set(CMAKE_CXX_FLAGS "-g -fnon-call-exceptions -ggdb -Werror-implicit-function-declaration -Werror=return-type -Wall -Wno-reorder ${CMAKE_CXX_FLAGS}")


find_package(catkin REQUIRED COMPONENTS
 fsystem
 alica_engine
)

catkin_package(
 INCLUDE_DIRS include
 LIBRARIES autodiff alica_engine
 CATKIN_DEPENDS fsystem
)

include_directories(include ${catkin_INCLUDE_DIRS})

add_library(autodiff
  src/Abs.cpp
  src/And.cpp
  src/Atan2.cpp
  src/Constant.cpp
  src/ConstPower.cpp
  src/ConstraintBuilder.cpp
  src/ConstraintUtility.cpp
  src/Cos.cpp
  src/Exp.cpp
  src/LinSigmoid.cpp
  src/Log.cpp
  src/LTConstraint.cpp
  src/LTEConstraint.cpp
  src/Max.cpp
  src/Min.cpp
  src/Or.cpp
  src/Product.cpp
  src/Reification.cpp
  src/Sigmoid.cpp
  src/Sin.cpp
  src/Sum.cpp
  src/Tape.cpp
  src/Term.cpp
  src/TermBuilder.cpp
  src/TermHolder.cpp
  src/TermPower.cpp
  src/Variable.cpp
)

target_link_libraries(autodiff ${catkin_LIBRARIES})

add_executable(test_tape test/test_tape.cpp)

target_link_libraries(test_tape ${PROJECT_NAME} ${catkin_LIBRARIES})


add_executable(sine_benchmark test/test_benchmark.cpp)

target_link_libraries(sine_benchmark ${PROJECT_NAME} ${catkin_LIBRARIES})

if(CATKIN_ENABLE_TESTING)
    catkin_add_gtest(${PROJECT_NAME}-test test/test_autodiff.cpp)
    if(TARGET ${PROJECT_NAME}-test)
    target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME} ${catkin_LIBRARIES})
    endif()
else(CATKIN_ENABLE_TESTING)
    # AUTODIFF TESTS
    find_package(GTest REQUIRED)
    include_directories(BEFORE
    ${GTEST_INCLUDE_DIRS}
    )
    add_executable(${PROJECT_NAME}-tests test/test_autodiff.cpp)
    target_link_libraries(${PROJECT_NAME}-tests ${PROJECT_NAME} pthread ${GTEST_LIBRARIES})
endif(CATKIN_ENABLE_TESTING)
