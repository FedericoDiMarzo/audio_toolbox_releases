# Audio Toolbox

A C++ library to simplify the development of audio processing applications.

## Installation

Audio Toolbox should work on Windows, MacOS, and Linux.

```bash
git clone git@github.com:FedericoDiMarzo/audio_toolbox.git
cd audio_toolbox

# Ubuntu only dependencies are provided.
# For other platforms, you need to figure out the dependencies manually
./scripts/install_ubuntu_dependencies.sh
```

## Building the Project

We use cmake as a build system.

```bash
mkdir build && cd build
cmake ..
make -j
```

## CMake Options

You can pass these options by calling

```bash
cmake .. -D<OPTION_NAME>=<VALUE>
```

You can set the following options:

```bash
# Whether to build the tests and tools
AT_BUILD_TESTS=ON|OFF
AT_BUILD_TOOLS=ON|OFF

# Enables Miniaudio as audio backend
AT_ENABLE_MINIAUDIO=ON|OFF

# Enables RtMidi as MIDI backend
AT_ENABLE_RTMIDI=ON|OFF

# Enables approximate math functions (faster but less precise)
# Note: this is has not been tested extensively
AT_ENABLE_APPROX_MATH=ON|OFF

# Enables Intel optimizations 
AT_ENABLE_INTEL_OPTIMIZATIONS=ON|OFF

# Enables efficient random number generation
AT_ENABLE_EFFICIENT_RAND=ON|OFF

# Disable exceptions
AT_NO_EXCEPTIONS=ON|OFF

# Set the optimization flag (default is -O3)
AT_OPTIMIZATION_FLAG=<optimization_flag>

# Set the FFT implementation (0: pffft, 1: ARM)
AT_FFT_IMPLEMENTATION=0|1
```

## Running one of the tools

```bash
cmake .. -DAT_BUILD_TOOLS=ON
make -j
./bin/<name_of_the_tool>
```

## Running all the unit tests

```bash
cmake .. -DAT_BUILD_TESTS=ON
make -j
for x in ./bin/*_test; do $x || exit 1; done
```

## Aknowledgements

A special thanks to [Andrea Molgora](https://github.com/cagnolone) for the bitcrush effect implementation.
