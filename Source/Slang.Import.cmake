if ("${BUILD_PLATFORM}" STREQUAL "Windows")
  if(NOT DEFINED SLANG_ROOT)
    set(SLANG_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/Imports/Windows/Slang/slang-0.10.35/
        CACHE STRING ""
        FORCE)
  endif()

  set(SLANG_COMPILER "${SLANG_ROOT}bin/windows-x64/release/slangc.exe" CACHE STRING "" FORCE)
endif()
