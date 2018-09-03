function(AzuraAddSPIRVShadersToTarget TARGET)
  foreach(SHADER_TUPLE_STR ${ARGN})

    string(REPLACE "|" ";" SHADER_TUPLE ${SHADER_TUPLE_STR})

    list(GET SHADER_TUPLE 0 SHADER_INPUT_FILE)
    list(GET SHADER_TUPLE 1 SHADER_ARGUMENTS)

    get_filename_component(SHADER_FILE_NAME ${SHADER_INPUT_FILE} NAME)
    set(OUTPUT_FILE "${PROJECT_BINARY_DIR}/Shaders/Vulkan/${SHADER_FILE_NAME}.spv")
    separate_arguments(SHADER_ARGUMENTS_LIST NATIVE_COMMAND "${SHADER_ARGUMENTS}")
    add_custom_command(OUTPUT ${OUTPUT_FILE}
                       COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/Shaders/Vulkan/"
                       COMMAND ${GLSL_VALIDATOR} -V ${SHADER_INPUT_FILE} -o ${OUTPUT_FILE} ${SHADER_ARGUMENTS_LIST}
                       DEPENDS ${SHADER_INPUT_FILE})
    list(APPEND SPIRV_BINARY_FILES ${OUTPUT_FILE})
  endforeach(SHADER_TUPLE_STR)

  add_custom_target(${TARGET}_VulkanShaders DEPENDS ${SPIRV_BINARY_FILES})
  add_dependencies(${TARGET} ${TARGET}_VulkanShaders)
endfunction()
