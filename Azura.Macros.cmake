macro(AzuraSilenceClangTidy TargetName)
	set_target_properties(
	    ${TargetName}
	    PROPERTIES
	         CXX_CLANG_TIDY ""
	)
endmacro(AzuraSilenceClangTidy)


macro(AzuraAddUnitTest TargetName)
	add_test(UnitTests ${TargetName})

	target_compile_definitions(${TargetName}
	    PUBLIC
	        BUILD_UNIT_TEST=1
	)

endmacro(AzuraAddUnitTest)
