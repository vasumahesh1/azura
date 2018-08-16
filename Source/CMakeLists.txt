set(BOOST_VERSION 1.67.0 CACHE STRING "" FORCE)

# TODO: Add Platform Support for Variables like these
if (NOT DEFINED VULKAN_1_1_77_0_INCLUDE_DIR)
	set(VULKAN_1_1_77_0_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Vulkan/1.1.77.0/Include/ CACHE STRING "" FORCE)
endif()

if (NOT DEFINED VULKAN_1_1_77_0_LIB32_DIR)
	set(VULKAN_1_1_77_0_LIB32_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Vulkan/1.1.77.0/Lib32/ CACHE STRING "" FORCE)
endif()

if (NOT DEFINED VULKAN_1_1_77_0_LIB64_DIR)
	set(VULKAN_1_1_77_0_LIB64_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Vulkan/1.1.77.0/Lib/ CACHE STRING "" FORCE)
endif()

if (NOT DEFINED BOOST_ROOT)
	set(BOOST_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Boost/boost_1_67_0/ CACHE STRING "" FORCE)
endif()

set(BOOST_LIBRARYDIR ${BOOST_ROOT}/stage/lib/ CACHE STRING "" FORCE)

message("-- Using VULKAN_1_1_77_0_INCLUDE_DIR => ${VULKAN_1_1_77_0_INCLUDE_DIR}")
message("-- Using VULKAN_1_1_77_0_LIB32_DIR => ${VULKAN_1_1_77_0_LIB32_DIR}")
message("-- Using VULKAN_1_1_77_0_LIB64_DIR => ${VULKAN_1_1_77_0_LIB64_DIR}")
message("-- Using BOOST_ROOT => ${BOOST_ROOT}")

add_subdirectory(Common)
add_subdirectory(Azura)
# add_subdirectory(Sandbox)
