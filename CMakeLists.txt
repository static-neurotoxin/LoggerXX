CMAKE_MINIMUM_REQUIRED (VERSION 3.2.0)
PROJECT(LoggerXX CXX)

FIND_PACKAGE(Boost REQUIRED COMPONENTS ${BOOST_REQUIRED_PACKAGES})

SET(SOURCES
    log.cpp
    logger.cpp
)

SET(HEADERS
    log.h
    logger.h
)

ADD_LIBRARY(LoggerXX STATIC ${SOURCES} ${HEADERS})


TARGET_LINK_LIBRARIES(LoggerXX
	${Boost_LIBRARIES}
 )

#ensure that externals are built before core.
#ADD_DEPENDENCIES(LoggerXX externals)

# Enable C++14
SET_PROPERTY(TARGET LoggerXX PROPERTY CXX_STANDARD 14)
SET_PROPERTY(TARGET LoggerXX PROPERTY CXX_STANDARD_REQUIRED TRUE)
IF(UNIX)
    SET_PROPERTY(TARGET LoggerXX PROPERTY CXX_EXTENSIONS FALSE)
ENDIF()

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
TARGET_LINK_LIBRARIES(LogTest LoggerXX ${Boost_LIBRARIES} Threads::Threads)

SET_PROPERTY(TARGET LogTest PROPERTY CXX_STANDARD 14)
SET_PROPERTY(TARGET LogTest PROPERTY CXX_STANDARD_REQUIRED TRUE)
IF(UNIX)
   SET_PROPERTY(TARGET LogTest PROPERTY CXX_EXTENSIONS FALSE)
ENDIF()
