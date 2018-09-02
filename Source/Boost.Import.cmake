add_library(VULKAN_STATIC INTERFACE IMPORTED GLOBAL)

if ("${BUILD_PLATFORM}" STREQUAL "Windows")
  if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Boost/boost_1_67_0/
        CACHE STRING ""
        FORCE)
  endif()
endif()

set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib/ CACHE STRING "" FORCE)
