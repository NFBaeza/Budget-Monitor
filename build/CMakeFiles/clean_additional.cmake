# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/budget_monitor_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/budget_monitor_autogen.dir/ParseCache.txt"
  "budget_monitor_autogen"
  )
endif()
