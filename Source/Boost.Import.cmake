if ("${BUILD_PLATFORM}" STREQUAL "Windows")
  if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Boost/boost_1_67_0/
        CACHE STRING ""
        FORCE)
  endif()

  set(LIB64_BOOST "lib64-msvc-14.1")
  set(LIB32_BOOST "lib32-msvc-14.1")

  if ("${BUILD_ARCH}" STREQUAL "64")
    set(BOOST_LIBRARYDIR "${BOOST_ROOT}/${LIB64_BOOST}/" CACHE STRING "" FORCE)
  elseif("${BUILD_ARCH}" STREQUAL "32")
    set(BOOST_LIBRARYDIR "${BOOST_ROOT}/${LIB32_BOOST}/" CACHE STRING "" FORCE)
  endif()
endif()

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost ${BOOST_VERSION} EXACT REQUIRED COMPONENTS log filesystem system)

message(STATUS "BOOST_ROOT: ${BOOST_ROOT}")