function(add_solution name)
    if(NOT IS_READABLE "${CMAKE_CURRENT_SOURCE_DIR}/${name}.cpp")
        message(FATAL_ERROR "Solution source file not found: ${name}.cpp")
    endif()

    add_executable(${name} "${name}.cpp")
    target_link_libraries(${name} PRIVATE getInput)
    enable_cxx_warnings(${name})
endfunction()

add_solution(day1-1)
target_link_libraries(day1-1 PRIVATE Utils)
