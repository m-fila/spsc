# Provides:
#   - cache variable: SPSC_SANITIZER (none|address|undefined|thread)
#   - function: spsc_apply_sanitizer(<target>)

include_guard(GLOBAL)

set(
  SPSC_SANITIZER
  "none"
  CACHE STRING
  "Enable one sanitizer: none;address;undefined;thread"
)
set_property(
  CACHE SPSC_SANITIZER
  PROPERTY STRINGS none address undefined thread
)

function(spsc_apply_sanitizer target_name)
  if(NOT TARGET ${target_name})
    message(
      FATAL_ERROR
      "spsc_apply_sanitizer: target '${target_name}' does not exist"
    )
  endif()

  if(SPSC_SANITIZER STREQUAL "none")
    return()
  endif()

  if(SPSC_SANITIZER STREQUAL "address")
    set(_spsc_sanitize_flag "address")
  elseif(SPSC_SANITIZER STREQUAL "undefined")
    set(_spsc_sanitize_flag "undefined")
  elseif(SPSC_SANITIZER STREQUAL "thread")
    set(_spsc_sanitize_flag "thread")
  else()
    message(
      FATAL_ERROR
      "Invalid SPSC_SANITIZER='${SPSC_SANITIZER}'. Use one of: none, address, undefined, thread."
    )
  endif()

  get_target_property(_spsc_target_type ${target_name} TYPE)
  if(_spsc_target_type STREQUAL "INTERFACE_LIBRARY")
    set(_spsc_scope "INTERFACE")
  else()
    set(_spsc_scope "PUBLIC")
  endif()

  target_compile_options(
    ${target_name}
    ${_spsc_scope}
    -fsanitize=${_spsc_sanitize_flag}
    -fno-omit-frame-pointer
  )
  target_link_options(
    ${target_name}
    ${_spsc_scope}
    -fsanitize=${_spsc_sanitize_flag}
  )

  if(SPSC_SANITIZER STREQUAL "undefined")
    target_compile_options(
      ${target_name}
      ${_spsc_scope}
      -fno-sanitize-recover=all
    )
  endif()

  unset(_spsc_sanitize_flag)
  unset(_spsc_target_type)
  unset(_spsc_scope)
endfunction()
