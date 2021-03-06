# Copyright (c) 2017 apertus° Association & contributors
# Project: OpenCine / OCLauncher
# License: GNU GPL Version 3 (https://www.gnu.org/licenses/gpl-3.0.en.html)

cmake_minimum_required(VERSION 3.2)
PROJECT(OCLauncher)

CMAKE_POLICY(SET CMP0020 NEW)

set( CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5QuickWidgets REQUIRED)

#TODO: Rework this part to be similar to other projects
FILE(GLOB_RECURSE SOURCE_FILES "*.cpp")
FILE(GLOB_RECURSE HEADER_FILES "*.h")
FILE(GLOB_RECURSE UI_FILES "*.ui")
FILE(GLOB_RECURSE JSON_FILES "*.json")
#---------------------------------------

QT5_WRAP_UI(UI_FILES_MOC ${UI_FILES})

INCLUDE_DIRECTORIES("${OCui_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${OCcore_SOURCE_DIR}")

#SET(INCLUDE_DIRS "")
#foreach (_headerFile ${HEADER_FILES})
    #get_filename_component(_dir ${_headerFile} PATH)
    #list (APPEND INCLUDE_DIRS ${_dir})
#endforeach()
#list(REMOVE_DUPLICATES INCLUDE_DIRS)

#INCLUDE_DIRECTORIES(${BUILD_DIR}/include)
include_directories("${OCui_SOURCE_DIR}")

IF(WIN32)
    ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES_MOC} ${JSON_FILES})
ELSE(UNIX)
    ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${UI_FILES_MOC} ${JSON_FILES})
ENDIF()

TARGET_LINK_LIBRARIES(${PROJECT_NAME} OCcore OCui)

QT5_USE_MODULES(${PROJECT_NAME} Widgets)

FILE(COPY "Resources" DESTINATION ${CMAKE_BINARY_DIR}/bin)
CONFIGURE_FILE("${CMAKE_CURRENT_SOURCE_DIR}/OCLauncher_settings.json" "${CMAKE_BINARY_DIR}/bin" COPYONLY)
