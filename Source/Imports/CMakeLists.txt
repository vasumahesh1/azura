# include(ExternalProject)

# list(APPEND CMAKE_ARGS "-DYAML_CPP_BUILD_TESTS=OFF")

# externalproject_add(YamlCPP
#                     GIT_REPOSITORY
#                     https://github.com/jbeder/yaml-cpp.git
#                     # GIT_TAG
#                     # yaml-cpp-0.6.2
#                     CMAKE_ARGS
#                     ${CMAKE_ARGS}

#                     BUILD_COMMAND
#                     ""
#                     INSTALL_COMMAND
#                     ""
#                     TEST_COMMAND
#                     "")


# ExternalProject_Get_property(YamlCPP SOURCE_DIR)
add_subdirectory(Common)

# set(YAMP_CPP_INCLUDE
#       ${SOURCE_DIR}/include
#       CACHE STRING ""
#       FORCE)

# ExternalProject_Get_property(YamlCPP BINARY_DIR)

# set(YAMP_CPP_LIB
#       ${BINARY_DIR}/libyaml-cppmdd.lib
#       CACHE STRING ""
#       FORCE)
