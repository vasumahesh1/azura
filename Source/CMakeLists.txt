# TODO: Add Platform Support for Variables like these
set(VULKAN_1_1_77_0_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Vulkan/1.1.77.0/Include/ CACHE STRING "" FORCE)
set(VULKAN_1_1_77_0_LIB32_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Vulkan/1.1.77.0/Lib32/ CACHE STRING "" FORCE)
set(VULKAN_1_1_77_0_LIB64_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Vulkan/1.1.77.0/Lib/ CACHE STRING "" FORCE)
set(BOOST_VERSION 1.68.0 CACHE STRING "" FORCE)
set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Boost/boost_1_68_0/ CACHE STRING "" FORCE)

add_subdirectory(Common)
add_subdirectory(Azura)
