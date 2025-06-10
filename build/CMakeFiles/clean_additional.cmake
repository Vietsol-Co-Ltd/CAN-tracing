# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CANtracing_autogen"
  "CMakeFiles\\CANtracing_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\CANtracing_autogen.dir\\ParseCache.txt"
  )
endif()
