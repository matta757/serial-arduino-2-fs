**C++ tool that allow communication between an Arduino serial board and a IP server (like SIOC) to interact with flight simulators.**

# Dependencies #
  * At least Boost 1.39.0 is required (system and thread modules)
  * Build system is based on CMake.

# Linux build steps #
  * git clone https://code.google.com/p/serial-arduino-2-fs/
  * cd serial-arduino-2-fs/
  * mkdir build
  * cd build/
  * cmake ..
  * make