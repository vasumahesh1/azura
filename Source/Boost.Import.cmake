if ("${BUILD_PLATFORM}" STREQUAL "Windows")
  if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Boost/boost_1_69_0/
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

elseif ("${BUILD_PLATFORM}" STREQUAL "MacOS")
  
  if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT /usr/local/Cellar/boost/1.69.0_2/
        CACHE STRING ""
        FORCE)
  endif()

  if ("${BUILD_ARCH}" STREQUAL "64")
    set(BOOST_LIBRARYDIR "${BOOST_ROOT}/" CACHE STRING "" FORCE)
  elseif("${BUILD_ARCH}" STREQUAL "32")
    message(FATAL_ERROR "Azura Darwin doesn't support 32 bit builds")
  endif()

endif()

# if(CMAKE_BUILD_TYPE MATCHES DEBUG)
#   set(Boost_USE_DEBUG_LIBS ON)
#   set(Boost_USE_RELEASE_LIBS OFF)
# elseif(CMAKE_BUILD_TYPE MATCHES RELEASE)
#   set(Boost_USE_DEBUG_LIBS OFF)
#   set(Boost_USE_RELEASE_LIBS ON)
# endif()

message(STATUS "BOOST_ROOT: ${BOOST_ROOT}")

set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost ${BOOST_VERSION} EXACT REQUIRED COMPONENTS log filesystem system)
