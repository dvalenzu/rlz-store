cmake_minimum_required(VERSION 2.8.7)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules")
include(AppendCompilerFlags)

find_package(Boost)

project(rlz-docstore CXX C)
set(CMAKE_BUILD_TYPE "Release")

# C++11 compiler Check
if(NOT CMAKE_CXX_COMPILER_VERSION) # work around for cmake versions smaller than 2.8.10
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE CMAKE_CXX_COMPILER_VERSION)
endif()
if(CMAKE_CXX_COMPILER MATCHES ".*clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      set(CMAKE_COMPILER_IS_CLANGXX 1)
endif()
if( (CMAKE_COMPILER_IS_GNUCXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 4.7) OR
    (CMAKE_COMPILER_IS_CLANGXX AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 3.2))
  message(FATAL_ERROR "Your C++ compiler does not support C++11. Please install g++ 4.7 (or greater) or clang 3.2 (or greater)")
else()
  message(STATUS "Compiler is recent enough to support C++11.")
endif()
if( CMAKE_COMPILER_IS_GNUCXX )
    append_cxx_compiler_flags("-std=c++11 -Wall -Wextra " "GCC" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-msse4.2 -O3 -ffast-math -funroll-loops -fno-omit-frame-pointer -g" "GCC" CMAKE_CXX_FLAGS_RELEASE)
else()
    append_cxx_compiler_flags("-std=c++11" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-stdlib=libc++" "CLANG" CMAKE_CXX_FLAGS)
    append_cxx_compiler_flags("-msse4.2 -O3  -ffast-math -funroll-loops -DNDEBUG" "CLANG" CMAKE_CXX_FLAGS_RELEASE)
endif()
include(CheckSSE)
FindSSE ()
if( SSE4_2_FOUND )
    if( CMAKE_COMPILER_IS_GNUCXX )
        append_cxx_compiler_flags("-msse4.2" "GCC" CMAKE_CXX_FLAGS)
    else()
        append_cxx_compiler_flags("-msse4.2" "CLANG" CMAKE_CXX_FLAGS)
    endif()
    message(STATUS "CPU does support SSE4.2.")
else()
	message(STATUS "CPU does NOT support SSE4.2")
endif()

add_subdirectory(external/sdsl-lite)

add_subdirectory(external/bzip2-1.0.6)

add_subdirectory(external/zlib)
# disable zlib examples
if(TARGET example)
    set_target_properties(example minigzip PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()
if(TARGET example64)
    set_target_properties(example64 minigzip64 PROPERTIES EXCLUDE_FROM_ALL 1 EXCLUDE_FROM_DEFAULT_BUILD 1)
endif()

# liblzma
include(ExternalProject)
ExternalProject_Add(
  liblzma
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/liblzma/
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/external/liblzma/configure --prefix=${CMAKE_BINARY_DIR}/external/liblzma --disable-xz --disable-scripts --disable-xzdec --disable-lzmadec --disable-lzmainfo --disable-lzma-links
  PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/external/liblzma/
  BUILD_COMMAND make install
  BUILD_IN_SOURCE 1
)

include_directories("${PROJECT_SOURCE_DIR}/external/googletest/include"
                     ${CMAKE_HOME_DIRECTORY}/include
                     ${CMAKE_BINARY_DIR}/external/sdsl-lite/include
                     ${CMAKE_BINARY_DIR}/external/sdsl-lite/external/libdivsufsort/include/
                     ${CMAKE_HOME_DIRECTORY}/external/googletest/include
                     ${CMAKE_HOME_DIRECTORY}/external/sdsl-lite/external/FastPFor/headers/
                     ${CMAKE_HOME_DIRECTORY}/external/easyloggingpp/src/
                     ${CMAKE_HOME_DIRECTORY}/external/lz4/lib
                     ${CMAKE_HOME_DIRECTORY}/external/zlib/
                     ${CMAKE_HOME_DIRECTORY}/external/brotli/enc/
                     ${CMAKE_HOME_DIRECTORY}/external/brotli/dec/
                     ${CMAKE_BINARY_DIR}/external/zlib/
                     ${CMAKE_BINARY_DIR}/external/liblzma/include/
                     ${CMAKE_HOME_DIRECTORY}/external/bzip2-1.0.6/
                     ${CMAKE_HOME_DIRECTORY}/external/mongoose/
                     ${STXXL_INCLUDE_DIRS}
                    )

link_directories(${CMAKE_BINARY_DIR}/external/liblzma/lib/)

set(LZ4_DIR  ${CMAKE_HOME_DIRECTORY}/external/lz4/lib/)
set(LZ4_SRCS_LIB ${LZ4_DIR}lz4.c ${LZ4_DIR}lz4hc.c ${LZ4_DIR}lz4.h ${LZ4_DIR}lz4hc.h ${LZ4_DIR}lz4frame.c ${LZ4_DIR}lz4frame.h ${LZ4_DIR}xxhash.c)
add_library(lz4 ${LZ4_SRCS_LIB})

set(BROTLI_DIR  ${CMAKE_HOME_DIRECTORY}/external/brotli/)
file(GLOB BROTLI_SRCS_ENC_LIB ${BROTLI_DIR}/enc/*.cc)
file(GLOB BROTLI_SRCS_DEC_LIB ${BROTLI_DIR}/dec/*.c)
add_library(brotli ${BROTLI_SRCS_ENC_LIB} ${BROTLI_SRCS_DEC_LIB})

add_executable(rlzs-create-www.x src/rlzs-create-www.cpp)
target_link_libraries(rlzs-create-www.x sdsl pthread divsufsort divsufsort64 zlib lz4 bzip2 brotli lzma)

add_executable(rlzs-create.x src/rlzs-create.cpp)
target_link_libraries(rlzs-create.x sdsl pthread divsufsort divsufsort64 zlib lz4 bzip2 brotli lzma)

add_executable(lzs-create.x src/lzs-create.cpp)
target_link_libraries(lzs-create.x sdsl pthread divsufsort divsufsort64 zlib lz4 bzip2 brotli lzma)

add_executable(create-collection.x src/create-collection.cpp)
target_link_libraries(create-collection.x sdsl pthread zlib lz4 bzip2 brotli lzma)

add_executable(unit-tests.x src/unit-tests.cpp)
target_link_libraries(unit-tests.x sdsl pthread zlib gtest_main lz4 bzip2 brotli lzma)

