CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

project (arduino2fs)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pthread")

#SET(LIBRARY_TYPE SHARED)
SET(LIBRARY_TYPE STATIC)
set(CMAKE_BUILD_TYPE "Release")

find_package( Boost 1.39.0 REQUIRED system thread )
LIST(APPEND PROJECT_LIBRARIES Boost Boost-system Boost-thread)

FOREACH(DEPENDENT ${PROJECT_LIBRARIES} )
	INCLUDE_DIRECTORIES( ${${DEPENDENT}_INCLUDE_DIRS} )
	LINK_DIRECTORIES( ${${DEPENDENT}_LIBRARY_DIRS} )
	LIST(APPEND PROJECT_BUILD_LIBRARIES ${${DEPENDENT}_LIBRARIES} )
ENDFOREACH(DEPENDENT )


INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )

# arduino2fs
SET ( PROJECT_SOURCE_FILES
	src/main.cpp
	src/comm/myserialport.cpp
	src/comm/serialprotocol.cpp
	src/comm/lanserialbridge.cpp
	src/comm/impl/siocbridge.cpp
)

add_executable(arduino2fs ${PROJECT_SOURCE_FILES})
target_link_libraries (arduino2fs  ${PROJECT_BUILD_LIBRARIES})

# arduino2fs_tests
SET ( PROJECT_TEST_SOURCE_FILES
	test/src/main.cpp
	test/src/comm/serialprotocol.cpp
	test/src/comm/lanserialbridge.cpp
	src/comm/serialprotocol.cpp
	src/comm/lanserialbridge.cpp
	src/comm/impl/siocbridge.cpp
)
LIST(APPEND PROJECT_BUILD_LIBRARIES gtest)

add_executable(arduino2fs_test ${PROJECT_TEST_SOURCE_FILES})
target_link_libraries (arduino2fs_test  ${PROJECT_BUILD_LIBRARIES})

