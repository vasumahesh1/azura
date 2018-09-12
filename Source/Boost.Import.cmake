if ("${BUILD_PLATFORM}" STREQUAL "Windows")
  if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Boost/boost_1_67_0/
        CACHE STRING ""
        FORCE)
  endif()
endif()


set(Boost_USE_STATIC_LIBS ON)

find_package(Boost ${BOOST_VERSION} EXACT REQUIRED COMPONENTS log filesystem system)

set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib/ CACHE STRING "" FORCE)

message(STATUS "BOOST_ROOT: ${BOOST_ROOT}")