# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "AkademineIS_autogen"
  "CMakeFiles\\AkademineIS_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AkademineIS_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\sqlite3_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\sqlite3_autogen.dir\\ParseCache.txt"
  "sqlite3_autogen"
  )
endif()
