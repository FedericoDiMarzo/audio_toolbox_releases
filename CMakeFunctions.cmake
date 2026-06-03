# Used to log variables
function(log_variable VAR)
  message("${VAR} = ${${VAR}}")
endfunction()

# Used to get the value of an environment variable
# or use a default value if the environment variable is not set
function(set_from_env VAR_NAME DEFAULT_VALUE)
  if(DEFINED ENV{${VAR_NAME}})
    set(${VAR_NAME} $ENV{${VAR_NAME}} PARENT_SCOPE)
  elseif(DEFINED ${VAR_NAME})
    set(${VAR_NAME} ${VAR_NAME})
  else()
    set(${VAR_NAME} ${DEFAULT_VALUE} PARENT_SCOPE)
  endif()
endfunction()

# Used to set a definition in C++ from a boolean option
function(set_cpp_definitions_from_bool_opt OPT_NAME)
  if(${OPT_NAME})
    add_compile_definitions(${OPT_NAME}=1)
  else()
    add_compile_definitions(${OPT_NAME}=0)
  endif()
endfunction()

# Used to set a definition in C++ from a string option
function(set_cpp_definitions_from_string_opt OPT_NAME)
  if(DEFINED ${OPT_NAME})
    add_compile_definitions(${OPT_NAME}=${${OPT_NAME}})
  endif()
endfunction()
