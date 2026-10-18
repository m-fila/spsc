# Provides:
#   - function: spsc_target_compile_options(<target>)

function(spsc_target_compile_options target_name)
  target_compile_options(
    ${target_name}
    PRIVATE -Wall -Werror -Wextra -Wno-interference-size
  )
endfunction()
