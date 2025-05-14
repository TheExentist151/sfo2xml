cmake_minimum_required(VERSION 3.15)
project(sfo2xml VERSION 1.0 LANGUAGES CXX)

set(SFO2XML_SOURCES 
    "Source/main.cpp"
    "Source/SFOFile.cpp"
)

set(SFO2XML_HEADERS
    "Source/Helpers.h"
    "Source/SFOFile.h"
)

set(TINYXML2_LIBRARY
    "Dependencies/tinyxml2/tinyxml2.cpp"
    "Dependencies/tinyxml2/tinyxml2.h")

add_executable(sfo2xml ${SFO2XML_SOURCES} ${SFO2XML_HEADERS} ${TINYXML2_LIBRARY})
target_include_directories(sfo2xml PRIVATE "Dependencies/tinyxml2")

