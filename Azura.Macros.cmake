macro(AzuraSilenceClangTidy TargetName)
  set_target_properties(${TargetName} PROPERTIES CXX_CLANG_TIDY "")
endmacro(AzuraSilenceClangTidy)

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
