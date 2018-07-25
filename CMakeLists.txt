CMAKE_MINIMUM_REQUIRED(VERSION 2.8.7)

# Allows qtmain to be linked auto
IF(POLICY CMP0020)
  CMAKE_POLICY(SET CMP0020 NEW)
ENDIF()

# Let automoc also process generated files
IF(POLICY CMP0071)
  CMAKE_POLICY(SET CMP0071 NEW)
ENDIF()

# Use AppleClang for OS X compiler
IF(POLICY CMP0025)
  CMAKE_POLICY(SET CMP0025 NEW)
ENDIF()

SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules;${CMAKE_MODULE_PATH};$ENV{CMAKE_MODULE_PATH}")

INCLUDE(common OPTIONAL)

IF(WITH_FORCE_REMOTE_CMAKE_MODULES OR NOT COMMON_MODULE_FOUND)
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
        OUTPUT_VARIABLE HG_OUTPUT
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
        OUTPUT_VARIABLE HG_OUTPUT
        OUTPUT_STRIP_TRAILING_WHITESPACE)
      IF(HG_RESULT EQUAL 0)
        MESSAGE(STATUS "CMake modules successfully cloned")
      ELSE()
        MESSAGE(FATAL_ERROR "Unable to clone CMake modules: ${HG_ERRORS}")
      ENDIF()
    ENDIF()

    # retry to include common.cmake
    SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")

    INCLUDE(common OPTIONAL)
  ENDIF()
#  MESSAGE(FATAL_ERROR "Unable to find common.cmake! Please place it in CMakeModules subdirectory, set CMAKE_MODULE_PATH environement variable or set CMAKE_MODULE_PATH parameter to CMake.")
ELSE()
  OPTION(WITH_FORCE_REMOTE_CMAKE_MODULES "Force the use of downloading CMake modules into CMakeModules" OFF)
ENDIF()

SET(VERSION_MAJOR 1)
SET(VERSION_MINOR 0)
SET(VERSION_PATCH "REVISION")
SET(AUTHOR "Kervala")
SET(PRODUCT "MapMaker")
SET(DESCRIPTION "Map editor")
SET(TARGET "mapmaker")
SET(YEAR "2018-${CURRENT_YEAR}")

PROJECT(${PRODUCT} C CXX)

# Instruct CMake to run moc automatically when needed.
SET(CMAKE_AUTOMOC ON)

INIT_DEFAULT_OPTIONS()

# Qt doesn't use RTTI or C++ exceptions
SET_OPTION_DEFAULT(WITH_EXCEPTIONS OFF)
SET_OPTION_DEFAULT(WITH_RTTI OFF)
SET_OPTION_DEFAULT(WITH_INSTALL_LIBRARIES OFF)

SETUP_DEFAULT_OPTIONS()

INIT_BUILD_FLAGS()
SETUP_BUILD_FLAGS()

SETUP_PREFIX_PATHS(${TARGET})
SETUP_EXTERNAL()

GEN_CONFIG_H()
GEN_REVISION_H()

USE_QT_MODULES(Gui Svg QT5 Widgets)

FILE(GLOB HEADER src/*.h)
FILE(GLOB SRC src/*.cpp)
FILE(GLOB UI ui/*.ui)
FILE(GLOB RES res/*.qrc res/*.icns res/*.ico)
FILE(GLOB TS translations/*.ts)

SET_TARGET_GUI_EXECUTABLE(${TARGET} ${SRC} ${RES} ${UI} ${HEADER} ${TS})
SET_TARGET_LABEL(${TARGET} ${PRODUCT})

IF(APPLE)
  SET(MACOSX_BUNDLE_GUI_IDENTIFIER "net.kervala.${TARGET}")
ENDIF()

IF(WITH_PCH)
  ADD_NATIVE_PRECOMPILED_HEADER(${TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/src/common.h ${CMAKE_CURRENT_SOURCE_DIR}/src/common.cpp)
ENDIF()

INSTALL_RESOURCES(${TARGET} "")
