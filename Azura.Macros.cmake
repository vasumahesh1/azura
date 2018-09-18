macro(AzuraSilenceClangTidy TargetName)
  set_target_properties(${TargetName} PROPERTIES CXX_CLANG_TIDY "")
endmacro(AzuraSilenceClangTidy)

macro(AzuraSilenceEnumGuard TargetName)
  set_target_properties(${TargetName}
                        PROPERTIES COMPILE_FLAGS "${CMAKE_CXX_FLAGS} /wd4061")
endmacro(AzuraSilenceEnumGuard)

macro(AzuraGlobalSilenceEnumGuard)
  if(MSVC)
    string(REPLACE " /we4061"
                   ""
                   CMAKE_CXX_FLAGS
                   "${CMAKE_CXX_FLAGS}")

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4061")
  elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    string(REPLACE " -Wswitch-enum"
                   ""
                   CMAKE_CXX_FLAGS
                   "${CMAKE_CXX_FLAGS}")
  endif()
endmacro(AzuraGlobalSilenceEnumGuard)

macro(AzuraSilenceWarningsForMathfu TargetName)
  if(MSVC)
    target_compile_options(${TargetName}
      PUBLIC
      "/wd4201"
    )
  endif()
endmacro(AzuraSilenceWarningsForMathfu)

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

macro(AzuraSilenceCRTSecureWarnings TargetName)
  target_compile_definitions(${TargetName} PUBLIC _CRT_SECURE_NO_WARNINGS)
endmacro(AzuraSilenceCRTSecureWarnings)

macro(AzuraAddUnitTest TargetName)
  add_test(UnitTests ${TargetName})
  target_compile_definitions(${TargetName} PUBLIC BUILD_UNIT_TEST=1 BUILD_TEST=1)
endmacro(AzuraAddUnitTest)

macro(AzuraAddGraphicsTest TargetName)
  if ((NOT DEFINED CI_MACHINE))
  add_test(AutoGraphicsTests ${TargetName})
  target_compile_definitions(${TargetName} PUBLIC BUILD_GRAPHICS_TEST=1 BUILD_TEST=1)
  endif()
endmacro(AzuraAddGraphicsTest)

macro(AzuraAddLoggingSupport TargetName Level)
  target_compile_definitions(${TargetName} PUBLIC LOG_LEVEL=${Level})
endmacro(AzuraAddLoggingSupport)
