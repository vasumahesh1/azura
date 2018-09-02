set(BOOST_VERSION 1.67.0 CACHE STRING "" FORCE)

message("CMake: Build Target: ${BUILD_TARGET}")
message("CMake: Build Platform: ${BUILD_PLATFORM}")
message("CMake: Build Arch: ${BUILD_ARCH}")

include("./GLFW.Import.cmake")
include("./Vulkan.Import.cmake")
include("./Boost.Import.cmake")

add_subdirectory(Imports)

add_subdirectory(Common)
add_subdirectory(Azura)
add_subdirectory(Log)

if(DEFINED FORCE_INCLUDE_SUB_DIRECTORIES)
  foreach(dir ${FORCE_INCLUDE_SUB_DIRECTORIES})
    add_subdirectory(${dir})
  endforeach()
endif()
