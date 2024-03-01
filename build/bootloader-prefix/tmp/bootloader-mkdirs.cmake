# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/ruslan/esp/v5.2/esp-idf/components/bootloader/subproject"
  "/Users/ruslan/Firmwares/Channel/memorylcd/esp32_memorylcd/build/bootloader"
  "/Users/ruslan/Firmwares/Channel/memorylcd/esp32_memorylcd/build/bootloader-prefix"
  "/Users/ruslan/Firmwares/Channel/memorylcd/esp32_memorylcd/build/bootloader-prefix/tmp"
  "/Users/ruslan/Firmwares/Channel/memorylcd/esp32_memorylcd/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/ruslan/Firmwares/Channel/memorylcd/esp32_memorylcd/build/bootloader-prefix/src"
  "/Users/ruslan/Firmwares/Channel/memorylcd/esp32_memorylcd/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/ruslan/Firmwares/Channel/memorylcd/esp32_memorylcd/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/ruslan/Firmwares/Channel/memorylcd/esp32_memorylcd/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
