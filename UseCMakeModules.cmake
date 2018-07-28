SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} $ENV{CMAKE_MODULE_PATH})

INCLUDE(common OPTIONAL)

IF(WITH_REMOTE_CMAKE_MODULES OR NOT COMMON_MODULE_FOUND)
  FIND_PROGRAM(HG_EXECUTABLE hg
    PATHS
      /opt/local/bin
      "C:/Program Files/TortoiseHg"
      "C:/Program Files (x86)/TortoiseHg"
    )

  IF(HG_EXECUTABLE)
    IF(EXISTS ${CMAKE_SOURCE_DIR}/CMakeModules)
      EXECUTE_PROCESS(COMMAND ${HG_EXECUTABLE} pull -u
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/CMakeModules
        ERROR_VARIABLE HG_ERRORS
        RESULT_VARIABLE HG_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      IF(HG_RESULT EQUAL 0)
        MESSAGE(STATUS "CMake modules successfully updated")
      ELSE()
        MESSAGE(FATAL_ERROR "Unable to pull CMake modules: ${HG_ERRORS}")
      ENDIF()
    ELSE()
      EXECUTE_PROCESS(COMMAND ${HG_EXECUTABLE} clone http://hg.kervala.net/cmake_modules CMakeModules
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        ERROR_VARIABLE HG_ERRORS
        RESULT_VARIABLE HG_RESULT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      IF(HG_RESULT EQUAL 0)
        MESSAGE(STATUS "CMake modules successfully cloned")
      ELSE()
        MESSAGE(FATAL_ERROR "Unable to clone CMake modules: ${HG_ERRORS}")
      ENDIF()
    ENDIF()

    # retry to include common.cmake
    SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

    INCLUDE(common)
  ELSE()
    MESSAGE(FATAL_ERROR "Unable to find Mercurial to clone CMake modules repository!")
  ENDIF()
ENDIF()
