if ("${BUILD_PLATFORM}" STREQUAL "MacOS")
	if ("${BUILD_ARCH}" STREQUAL "64")
	elseif("${BUILD_ARCH}" STREQUAL "32")
	endif()
endif()


