macro(AzuraSilenceClangTidy TargetName)
  set_target_properties(${TargetName} PROPERTIES CXX_CLANG_TIDY "")
endmacro(AzuraSilenceClangTidy)

macro(AzuraSilenceEnumGuard TargetName)
  set_target_properties(${TargetName}
                        PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} /wd4061")
endmacro(AzuraSilenceEnumGuard)

macro(AzuraDisableWarningsAsErrors)
  if(MSVC)
    string(REPLACE " /WX"
                   ""
                   CMAKE_CXX_FLAGS
                   "${CMAKE_CXX_FLAGS}")
  elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    string(REPLACE " -Werror"
                   ""
                   CMAKE_CXX_FLAGS
                   "${CMAKE_CXX_FLAGS}")
  endif()
endmacro(AzuraDisableWarningsAsErrors)

macro(AzuraEnableWarningsAsErrors)
  if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
  elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
  endif()
endmacro(AzuraEnableWarningsAsErrors)

macro(AzuraAddBuildFlags TargetName)
  if(CMAKE_BUILD_TYPE MATCHES DEBUG)
    target_compile_definitions(${TargetName} PUBLIC BUILD_DEBUG=1)
  elseif(CMAKE_BUILD_TYPE MATCHES RELEASE)
    target_compile_definitions(${TargetName} PUBLIC BUILD_RELEASE=1)
  endif()
endmacro(AzuraAddBuildFlags)

macro(AzuraAddUnitTest TargetName)
  add_test(UnitTests ${TargetName})
  target_compile_definitions(${TargetName} PUBLIC BUILD_UNIT_TEST=1)
endmacro(AzuraAddUnitTest)

macro(AzuraSetGLFWForTarget TargetName)
  set_target_properties(${TargetName} PROPERTIES LINK_FLAGS "/NODEFAULTLIB:MSVCRT")
endmacro(AzuraSetGLFWForTarget)
