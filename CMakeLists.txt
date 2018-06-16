cmake_minimum_required(VERSION 3.2)
include(CheckCXXSymbolExists)
include(CheckTypeSize)
include(SetTuxedoVersion.cmake)


project(tuxpp VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
include_directories(main/include SYSTEM "$ENV{TUXDIR}/include")
find_package( Threads )
link_directories("$ENV{TUXDIR}/lib")
set_tuxedo_version()

# help cmake out with SunPro
# change -std option as needed
if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "SunPro")
    set(SUNPRO_LINK_FLAGS -std=c++11 stdc++ gcc_s CrunG3 m c socket nsl)
endif()

add_subdirectory(main)
add_subdirectory(tools)
add_subdirectory(doc)
add_subdirectory(test EXCLUDE_FROM_ALL)

            
