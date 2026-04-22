# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles\\Lab_check_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Lab_check_autogen.dir\\ParseCache.txt"
  "Lab_check_autogen"
  )
endif()
