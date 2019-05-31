project(Azura)

set(BOOST_VERSION 1.69.0 CACHE STRING "" FORCE)

message(STATUS "Build Target: ${BUILD_TARGET}")
message(STATUS "Build Platform: ${BUILD_PLATFORM}")
message(STATUS "Build Arch: ${BUILD_ARCH}")

include("./Windows.Settings.cmake")
include("./Vulkan.Import.cmake")
include("./Boost.Import.cmake")
include("./Slang.Import.cmake")

add_subdirectory(Imports)

add_subdirectory(AssetManager)
add_subdirectory(Common)
add_subdirectory(Containers)
add_subdirectory(Platform)
add_subdirectory(Core)
add_subdirectory(Azura)
add_subdirectory(Samples)
add_subdirectory(Log)
add_subdirectory(Math)
add_subdirectory(ModFactory)
add_subdirectory(Camera)
add_subdirectory(PowerBench)
add_subdirectory(Physics)

if(DEFINED FORCE_INCLUDE_SUB_DIRECTORIES)
	foreach(dir ${FORCE_INCLUDE_SUB_DIRECTORIES})
		add_subdirectory(${dir})
	endforeach()
endif()
