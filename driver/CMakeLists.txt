set(TARGET_NAME driver_northstar)

add_library(${TARGET_NAME} SHARED
  driverlog.cpp
  driverlog.h
  deformation_northstar.h
  deformation_northstar.cpp
  driverlog_northstar.h
  driver_northstar.h
  driver_northstar.cpp
  utility_northstar.h
  watchdog_northstar.h
  watchdog_northstar.cpp
  dllmain.cpp
)

# add_definitions(-DDRIVER_SAMPLE_EXPORTS)

target_link_libraries(${TARGET_NAME}
  ${OPENVR_LIBRARIES}
  ${CMAKE_DL_LIBS}
)


# Force output directory destination, especially for MSVC (@so7747857).
function(setTargetOutputDirectory target)
  foreach(type RUNTIME LIBRARY ARCHIVE)
    set_target_properties(${target} PROPERTIES
      ${type}_OUTPUT_DIRECTORY         ${CMAKE_${type}_OUTPUT_DIRECTORY}/northstar
      ${type}_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_${type}_OUTPUT_DIRECTORY}/northstar
      ${type}_OUTPUT_DIRECTORY_RELEASE ${CMAKE_${type}_OUTPUT_DIRECTORY}/northstar
    )
  endforeach()
endfunction()

setTargetOutputDirectory(${TARGET_NAME})

get_cmake_property(_variableNames VARIABLES)
list (SORT _variableNames)
foreach (_variableName ${_variableNames})
    #message(STATUS "${_variableName}=${${_variableName}}")
endforeach()