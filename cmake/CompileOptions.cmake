# Provides:
#   - function: spsc_target_compile_options(<target>)

function(spsc_target_compile_options target_name)
  set(compile_options -Wall -Werror -Wextra)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    list(APPEND compile_options -Wno-interference-size)
  endif()

  target_compile_options(${target_name} PRIVATE ${compile_options})
endfunction()
