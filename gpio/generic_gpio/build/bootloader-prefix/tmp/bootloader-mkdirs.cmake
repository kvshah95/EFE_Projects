# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/kvshah95/espf/esp-idf/components/bootloader/subproject"
  "/Users/kvshah95/espf/gpio/generic_gpio/build/bootloader"
  "/Users/kvshah95/espf/gpio/generic_gpio/build/bootloader-prefix"
  "/Users/kvshah95/espf/gpio/generic_gpio/build/bootloader-prefix/tmp"
  "/Users/kvshah95/espf/gpio/generic_gpio/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/kvshah95/espf/gpio/generic_gpio/build/bootloader-prefix/src"
  "/Users/kvshah95/espf/gpio/generic_gpio/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/kvshah95/espf/gpio/generic_gpio/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/kvshah95/espf/gpio/generic_gpio/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
