if ("${BUILD_PLATFORM}" STREQUAL "Windows")

  if(NOT DEFINED SLANG_ROOT)
    set(SLANG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Slang/slang-0.11.16/
        CACHE STRING ""
        FORCE)
  endif()

  set(SLANG_COMPILER "${SLANG_ROOT}bin/windows-x64/release/slangc.exe" CACHE STRING "" FORCE)

elseif ("${BUILD_PLATFORM}" STREQUAL "MacOS")

  if(NOT DEFINED SLANG_ROOT)
    set(SLANG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Darwin/Slang/slang-0.11.16/
        CACHE STRING ""
        FORCE)
  endif()

  set(SLANG_COMPILER "${SLANG_ROOT}bin/linux-x86_64/release/slangc" CACHE STRING "" FORCE)

endif()
