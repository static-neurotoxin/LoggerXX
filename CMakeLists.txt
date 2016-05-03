CMAKE_MINIMUM_REQUIRED (VERSION 3.2.0)
PROJECT(LoggerXX CXX)

FIND_PACKAGE(Boost REQUIRED COMPONENTS filesystem system)

SET(SOURCES
    log_manager.cpp
    log_message.cpp
    log_target.cpp
)

SET(HEADERS
    log_manager.h
    log_message.h
    log_target.h
    log_hash.h
)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})

ADD_LIBRARY(LoggerXX STATIC ${SOURCES} ${HEADERS})

TARGET_LINK_LIBRARIES(LoggerXX
	${Boost_LIBRARIES}
 )

# Enable C++14
SET_PROPERTY(TARGET LoggerXX PROPERTY CXX_STANDARD 14)
SET_PROPERTY(TARGET LoggerXX PROPERTY CXX_STANDARD_REQUIRED TRUE)
SET_PROPERTY(TARGET LoggerXX PROPERTY CXX_EXTENSIONS FALSE)

TARGET_COMPILE_FEATURES(LoggerXX PRIVATE
    cxx_auto_type
    cxx_deleted_functions
    cxx_lambdas
    cxx_nullptr
    cxx_range_for
    cxx_return_type_deduction
    cxx_right_angle_brackets
    cxx_variadic_templates
    )

ADD_EXECUTABLE(LogTest LogTest.cpp ${HEADERS})
TARGET_LINK_LIBRARIES(LogTest LoggerXX ${Boost_LIBRARIES})


SET_PROPERTY(TARGET LogTest PROPERTY CXX_STANDARD 14)
SET_PROPERTY(TARGET LogTest PROPERTY CXX_STANDARD_REQUIRED TRUE)
SET_PROPERTY(TARGET LogTest PROPERTY CXX_EXTENSIONS FALSE)

add_executable(TreeTest treetest.cpp)
TARGET_LINK_LIBRARIES(TreeTest LoggerXX ${Boost_LIBRARIES})

SET_PROPERTY(TARGET TreeTest PROPERTY CXX_STANDARD 14)
SET_PROPERTY(TARGET TreeTest PROPERTY CXX_STANDARD_REQUIRED TRUE)
SET_PROPERTY(TARGET TreeTest PROPERTY CXX_EXTENSIONS FALSE)
