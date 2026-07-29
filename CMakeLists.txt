cmake_minimum_required(VERSION 3.22)
project(audio_toolbox)

set(CMAKE_CXX_STANDARD 20)
enable_language(C ASM)

# Set the output directory for the executables and libraries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/bin)

# Include the functions
include(CMakeFunctions.cmake)

# Used to fetch external libraries
include(FetchContent)

# Testing suite
include(GoogleTest)

set_property(DIRECTORY PROPERTY DEFAULT_CONFIG audio_toolbox)

# -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# = = = = = = = = = = = = = = = = =
# Options
# = = = = = = = = = = = = = = = = =

# Boolean options
option(AT_BUILD_TESTS "Build audio_toolbox tests" OFF)
option(AT_ENABLE_MINIAUDIO "Enable miniaudio" OFF)
option(AT_ENABLE_RTMIDI "Enable RtMidi" OFF)
option(AT_ENABLE_APPROX_MATH "Enable approximate math functions" OFF)
option(AT_ENABLE_INTEL_OPTIMIZATIONS "Enable Intel-specific optimizations" OFF)
option(AT_ENABLE_EFFICIENT_RAND "Enable efficient random number generation" OFF)
option(AT_NO_EXCEPTIONS "Disable exception handling" OFF)

# Non-boolean options
set(AT_OPTIMIZATION_FLAG "-O3" CACHE STRING "Set optimization flag")
set(AT_FFT_IMPLEMENTATION "0" CACHE STRING "Set FFT implementation (0: pffft, 1: arm_cmsis)")
set_property(CACHE AT_FFT_IMPLEMENTATION PROPERTY STRINGS 0 1)

if (AT_BUILD_TESTS)
    message(INFO "Tests require miniaudio. Enabling miniaudio.")
    message(INFO "Tests require RtMidi. Enabling RtMidi.")
    set(AT_ENABLE_MINIAUDIO ON CACHE BOOL "Enable miniaudio" FORCE)
    set(AT_ENABLE_RTMIDI ON CACHE BOOL "Enable RtMidi" FORCE)
endif()

if(NOT(AT_FFT_IMPLEMENTATION STREQUAL "0" OR AT_FFT_IMPLEMENTATION STREQUAL "1"))
  message(FATAL_ERROR "Invalid FFT option: ${AT_FFT_IMPLEMENTATION}")
endif()

# Check if the target architecture is x86_64 and we are not on Apple Silicon
if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64" AND NOT APPLE)
    set(AT_ENABLE_INTEL_OPTIMIZATIONS ON CACHE BOOL "Enable Intel-specific optimizations" FORCE)
endif()

# = = = = = = = = = = = = = = = = =
# C++ Definitions
# = = = = = = = = = = = = = = = = =
set_cpp_definitions_from_bool_opt(AT_ENABLE_MINIAUDIO)
set_cpp_definitions_from_bool_opt(AT_ENABLE_RTMIDI)
set_cpp_definitions_from_bool_opt(AT_ENABLE_APPROX_MATH)
set_cpp_definitions_from_bool_opt(AT_ENABLE_EFFICIENT_RAND)
set_cpp_definitions_from_bool_opt(AT_NO_EXCEPTIONS)
set_cpp_definitions_from_string_opt(AT_FFT_IMPLEMENTATION)

# = = = = = = = = = = = = = = = = =
# Flags
# = = = = = = = = = = = = = = = = =
set(_OPTIMIZATION ${AT_OPTIMIZATION_FLAG})

# We have different configurations for Windows, Linux, and Mac

# Windows = = = = = = = = = = = = =
if(MSVC)
  # There must be a better way to do this
  add_compile_definitions(
    M_PI=3.14159265358979323846
    M_SQRT2=1.41421356237309504880
    M_PI_2=1.57079632679489661923
  )
  set(_FLAGS "")
endif()

# Linux = = = = = = = = = = = = = =
if(NOT MSVC AND NOT APPLE)
  set(_FLAGS "-Wall -Werror -Wextra")

  # Set Intel-specific flags
  if(AT_ENABLE_INTEL_OPTIMIZATIONS)
    set(_FLAGS "${_FLAGS} -march=native -mtune=native -msse2")
  endif()
endif()

# Mac = = = = = = = = = = = = = = =
if(APPLE)
  set(_FLAGS "-Wall -Werror -Wextra")
endif()

# = = = = = = = = = = = = = = = = =
# Build-specific options
# = = = = = = = = = = = = = = = = =
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message("Debug build.")
  set(_OPTIMIZATION "-O0")
  set(_FLAGS "${_FLAGS} -g -fno-inline")

elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  message("Release build.")

elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  message("Release build with debug info.")
  set(_FLAGS "${_FLAGS} -g")
endif()

# This is the variable that will be used to set the targets flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${_FLAGS} ${_OPTIMIZATION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${_FLAGS} ${_OPTIMIZATION}")

# = = = = = = = = = = = = = = = = =
# External Libraries
# = = = = = = = = = = = = = = = = =
file(GLOB_RECURSE AT_SOURCES "src/*.cpp")
set(AT_EXT_LIBS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

# FFT implementations
if(${AT_FFT_IMPLEMENTATION} EQUAL 0)
  # Add pffft library
  add_library(
    pffft
    STATIC
    ${AT_EXT_LIBS_DIR}/pffft/pffft.c
  )

  if(NOT MSVC)
    set_source_files_properties(
      ${AT_EXT_LIBS_DIR}/pffft/pffft.c
      PROPERTIES
      COMPILE_FLAGS
      "-Wno-error=pointer-to-int-cast"
    )
  endif()

  target_include_directories(
    pffft
    PRIVATE
    ${AT_EXT_LIBS_DIR}
  )
endif()

if(${AT_FFT_IMPLEMENTATION} EQUAL 1)
  # Add arm_cmsis library
  set(
    ARM_CMSIS_SOURCES
    ${AT_EXT_LIBS_DIR}/arm/cmsis_fft/stm32h7xx.h
    ${AT_EXT_LIBS_DIR}/arm/cmsis_fft/arm_cfft_f32.c
    ${AT_EXT_LIBS_DIR}/arm/cmsis_fft/arm_bitreversal.c
    ${AT_EXT_LIBS_DIR}/arm/cmsis_fft/arm_common_tables.c
    ${AT_EXT_LIBS_DIR}/arm/cmsis_fft/arm_rfft_fast_f32.c
    ${AT_EXT_LIBS_DIR}/arm/cmsis_fft/arm_cfft_radix8_f32.c
    ${AT_EXT_LIBS_DIR}/arm/cmsis_fft/arm_rfft_fast_init_f32.c
  )
  add_library(
    arm_cmsis_fft
    STATIC
    ${ARM_CMSIS_SOURCES}
  )
  target_include_directories(
    arm_cmsis_fft
    PRIVATE
    ${AT_EXT_LIBS_DIR}
  )
endif()

# MIDI support
if(AT_ENABLE_RTMIDI)
  list(APPEND AT_EXT_LIBS "rtmidi")
  add_library(
    rtmidi
    STATIC
    ${AT_EXT_LIBS_DIR}/rtmidi/RtMidi.cpp
  )
  target_include_directories(
    rtmidi
    PRIVATE
    ${AT_EXT_LIBS_DIR}
  )
endif()

# FFT implementations
if(${AT_FFT_IMPLEMENTATION} EQUAL 0)
  list(APPEND AT_EXT_LIBS pffft)
elseif(${AT_FFT_IMPLEMENTATION} EQUAL 1)
  list(APPEND AT_EXT_LIBS arm_cmsis_fft)
endif()

# Audio backend for desktop applications
if(AT_ENABLE_MINIAUDIO)
  list(APPEND AT_EXT_LIBS miniaudio)
  add_library(
    miniaudio
    STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/miniaudio_instance.cpp
  )
  target_include_directories(
    miniaudio
    PRIVATE
    ${AT_EXT_LIBS_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
  )

else()
  list(REMOVE_ITEM AT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/core/miniaudio_instance.cpp")
  list(REMOVE_ITEM AT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/core/audio_interface.cpp")
  list(REMOVE_ITEM LIB_TESTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/core/audio_interface_test.cpp")
endif()

# = = = = = = = = = = = = = = = = =
# Compile audio_toolbox library
# = = = = = = = = = = = = = = = = =
add_library(
  audio_toolbox
  STATIC
  ${AT_SOURCES}
)
target_link_libraries(
  audio_toolbox
  PRIVATE
  ${AT_EXT_LIBS}
)

target_include_directories(
  audio_toolbox
  PUBLIC
  ${AT_EXT_LIBS_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

# = = = = = = = = = = = = = = = = =
# Compiling tests
# = = = = = = = = = = = = = = = = =

if(AT_BUILD_TESTS)
  file(GLOB_RECURSE LIB_TESTS "tests/*.cpp")
  add_subdirectory(${AT_EXT_LIBS_DIR}/googletest)
  enable_testing()
  set(TEST_EXECUTABLES)

  foreach(LT ${LIB_TESTS})
    # Skip audio_interface_test.cpp on macOS
    if(APPLE AND LT MATCHES "/audio_interface_test\\.cpp$")
        continue()
    endif()

    get_filename_component(NAME ${LT} NAME_WE)
    set(NAME "at_${NAME}")
    add_executable(${NAME} ${LT})
    target_link_libraries(
      ${NAME}
      PRIVATE
      audio_toolbox
      gtest_main
      gtest
    )
    target_include_directories(
      ${NAME}
      PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/tests
      ${CMAKE_CURRENT_SOURCE_DIR}/inc
      ${AT_EXT_LIBS_DIR}
    )

    list(APPEND TEST_EXECUTABLES ${NAME})
    gtest_discover_tests(${NAME})
  endforeach()

  # Compile all tests in one go
  add_custom_target(
    at_tests
    DEPENDS
    ${TEST_EXECUTABLES}
  )
endif()

# = = = = = = = = = = = = = = = = =
# Log configuration
# = = = = = = = = = = = = = = = = =
log_variable(AT_ENABLE_APPROX_MATH)
log_variable(AT_ENABLE_MINIAUDIO)
log_variable(AT_ENABLE_RTMIDI)
log_variable(AT_ENABLE_INTEL_OPTIMIZATIONS)
log_variable(AT_ENABLE_EFFICIENT_RAND)
log_variable(AT_NO_EXCEPTIONS)
log_variable(AT_OPTIMIZATION_FLAG)
log_variable(AT_FFT_IMPLEMENTATION)
log_variable(AT_BUILD_TESTS)

# Convert the flags to a list
string(REPLACE " " ";" CXX_FLAGS_LIST "${CMAKE_CXX_FLAGS}")
string(REPLACE " " ";" C_FLAGS_LIST "${CMAKE_C_FLAGS}")

# Print each C++ flag
message("CMAKE_CXX_FLAGS:")

foreach(FLAG ${CXX_FLAGS_LIST})
  message("  ${FLAG}")
endforeach()

# Print each C flag
message("CMAKE_C_FLAGS:")

foreach(FLAG ${C_FLAGS_LIST})
  message("  ${FLAG}")
endforeach()

# Print each source file
message("AT_SOURCE_FILES:")

foreach(SOURCE ${AT_SOURCES})
  message("  ${SOURCE}")
endforeach()

# Print each test file
message("AT_TEST_FILES:")
foreach(TEST ${LIB_TESTS})
  message("  ${TEST}")
endforeach()

# Print libs files
message("AT_LIBS:")

foreach(LIB ${AT_EXT_LIBS})
  message("  ${LIB}")
endforeach()

# Print each header file
message("AT_HEADER_FILES:")

foreach(HEADER ${LIB_HEADERS})
  message("  ${HEADER}")
endforeach()
