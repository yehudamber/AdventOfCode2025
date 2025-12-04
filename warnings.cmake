# Provide a function to enable a sensible set of compiler warnings for a target.
# Usage: enable_cxx_warnings(<target>)
function(enable_cxx_warnings target)
	if(NOT TARGET ${target})
		message(FATAL_ERROR "enable_cxx_warnings: target '${target}' does not exist")
	endif()

	# Apply compiler-specific warning flags per-target using generator expressions.
	# This ensures the flags are attached based on the compiler used for the target.
	target_compile_options(${target} PRIVATE
		# GCC
		$<$<CXX_COMPILER_ID:GNU>:-Wall;-Wextra;-Wpedantic;-Wcast-align;-Wshadow;-Wformat=2;-Wlogical-op;-Wdouble-promotion;-Wconversion>
		# Clang
		$<$<CXX_COMPILER_ID:Clang>:-Wall;-Wextra;-Wpedantic;-Wshadow;-Wformat=2;-Wconversion;-Wdouble-promotion;-Wimplicit-fallthrough>
		# MSVC
		$<$<CXX_COMPILER_ID:MSVC>:/W4>
	)
endfunction()
