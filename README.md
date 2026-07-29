# Audio Toolbox

A C++ library to simplify the development of audio processing applications.

## Installation

Audio Toolbox should work on Windows, MacOS, and Linux. Check out the dependecy list inside `linux_dependencies.txt` for more details.

```bash
git clone git@github.com:FedericoDiMarzo/audio_toolbox.git
cd audio_toolbox

# Checkout to a specific version if you want a reliable build
git checkout vX.Y.Z

# Install pre-commit hooks (optional but recommended)
python3 -m pip install pre-commit
pre-commit install
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
# Whether to build the tests
AT_BUILD_TESTS=ON|OFF

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

## Running all the unit tests

```bash
cmake .. -DAT_BUILD_TESTS=ON
make -j
ctest --output-on-failure
```

## Aknowledgements

A special thanks to [Andrea Molgora](https://github.com/cagnolone) for the bitcrush effect implementation.
