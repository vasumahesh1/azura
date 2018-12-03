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

function(AzuraDeployFolder TARGET FOLDER)
  add_custom_target(${TARGET}_Textures COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/${FOLDER} ${CMAKE_CURRENT_BINARY_DIR}/${FOLDER})
  add_dependencies(${TARGET} ${TARGET}_Textures)
endfunction()

function(AzuraAddFilesAsDependency TARGET KEY)
  add_custom_target(${TARGET}_${KEY})

  foreach(FILE_PATH ${ARGN})

    add_custom_command(
      TARGET ${TARGET}_${KEY}
      COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${FILE_PATH}
      DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${FILE_PATH}"
      COMMENT "Copying for target ${TARGET}: ${KEY} File Dependency ..."
      )

  endforeach(FILE_PATH)

  add_dependencies(${TARGET} ${TARGET}_${KEY})
endfunction()

function(AzuraAddSlangShaders TARGET API)
  foreach(SHADER_TUPLE_STR ${ARGN})
    string(REPLACE "|" ";" SHADER_TUPLE ${SHADER_TUPLE_STR})

    list(GET SHADER_TUPLE 0 SHADER_INPUT_FILE)
    list(GET SHADER_TUPLE 1 SHADER_TYPE)

    set(SHADER_ENTRY_POINT "main")

    list(LENGTH SHADER_TUPLE TUPLE_LENGTH)

    string(TOLOWER ${SHADER_TYPE}_5_0 SLANG_PROFILE)

    set(OUTPUT_FILE "")
    set(OUTPUT_DIRECTORY "")

    get_filename_component(SHADER_FILE_NAME ${SHADER_INPUT_FILE} NAME)
    string(REGEX REPLACE "\\.[^.]*$" "" SHADER_FILE_WITHOUT_EXT ${SHADER_FILE_NAME})


    if (TUPLE_LENGTH GREATER 2)
      string(TOLOWER ${SHADER_TYPE} SHADER_TYPE_LOWER)
      list(GET SHADER_TUPLE 2 SHADER_ENTRY_POINT)
      set(SHADER_FILE_WITHOUT_EXT ${SHADER_FILE_WITHOUT_EXT}.${SHADER_TYPE_LOWER})
    endif()

    if ("${API}" STREQUAL "VULKAN")
      set(OUTPUT_FILE "${PROJECT_BINARY_DIR}/Shaders/Vulkan/${SHADER_FILE_WITHOUT_EXT}.spv")
      set(OUTPUT_DIRECTORY "Shaders/Vulkan/")
    elseif ("${API}" STREQUAL "GLSL")
      set(OUTPUT_FILE "${PROJECT_BINARY_DIR}/Shaders/Vulkan/${SHADER_FILE_WITHOUT_EXT}.glsl")
      set(OUTPUT_DIRECTORY "Shaders/Vulkan/")
    elseif ("${API}" STREQUAL "HLSL")
      set(OUTPUT_FILE "${PROJECT_BINARY_DIR}/Shaders/D3D12/${SHADER_FILE_WITHOUT_EXT}.hlsl")
      set(OUTPUT_DIRECTORY "Shaders/D3D12/")
    elseif ("${API}" STREQUAL "DXBC")
      set(OUTPUT_FILE "${PROJECT_BINARY_DIR}/Shaders/D3D12/${SHADER_FILE_WITHOUT_EXT}.dxbc")
      set(OUTPUT_DIRECTORY "Shaders/D3D12/")
    endif()

    add_custom_command(OUTPUT ${OUTPUT_FILE}
                       COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/${OUTPUT_DIRECTORY}"
                       COMMAND ${SLANG_COMPILER} ${SHADER_INPUT_FILE} -profile ${SLANG_PROFILE} -entry ${SHADER_ENTRY_POINT} -o ${OUTPUT_FILE}
                       DEPENDS ${SHADER_INPUT_FILE})
    list(APPEND SPIRV_BINARY_FILES ${OUTPUT_FILE})
  endforeach(SHADER_TUPLE_STR)

  set(SHADERS_TARGET ${TARGET}_Shaders_${API})

  add_custom_target(${SHADERS_TARGET} DEPENDS ${SPIRV_BINARY_FILES})
  add_dependencies(${TARGET} ${SHADERS_TARGET})
endfunction()



function(AzuraDeployTarget TARGET)
  install(TARGETS ${TARGET}
        RUNTIME DESTINATION ${TARGET}/ COMPONENT AZURA_DEPLOY_COMPONENT)
  install(DIRECTORY ${DEPLOY_CONFIG_FOLDER}
          DESTINATION ${TARGET}/config/ COMPONENT AZURA_DEPLOY_COMPONENT)
endfunction()

macro(AzuraDeployTargetFolder TARGET FOLDER_SRC FOLDER_DEST)
  install(DIRECTORY ${FOLDER_SRC}/
        DESTINATION ${TARGET}/${FOLDER_DEST}/ COMPONENT AZURA_DEPLOY_COMPONENT)
endmacro()
