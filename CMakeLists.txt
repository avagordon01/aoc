cmake_minimum_required(VERSION 4.2)
project(aoc CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=-*,bugprone-*,cert-dcl21-cpp,cert-dcl50-cpp,cert-env33-c,cert-err34-c,cert-err52-cpp,cert-err60-cpp,cert-flp30-c,cert-msc50-cpp,cert-msc51-cpp,cppcoreguidelines-*,google-build-using-namespace,google-explicit-constructor,google-global-names-in-headers,google-readability-casting,google-runtime-int,google-runtime-operator,hicpp-*,-hicpp-vararg,misc-*,modernize-*,performance-*,readability-*,-readability-identifier-length,-misc-use-anonymous-namespace")

include(CTest)
include(FetchContent)

find_package(PkgConfig REQUIRED)
find_package(GTest)
# https://eigen.tuxfamily.org/dox/group__QuickRefPage.html
FetchContent_Declare(
  Eigen3
  URL https://gitlab.com/libeigen/eigen/-/archive/5.0.1/eigen-5.0.1.tar.gz
  EXCLUDE_FROM_ALL
  SYSTEM
  OVERRIDE_FIND_PACKAGE
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(Eigen3)

set(BOOST_INCLUDE_LIBRARIES
  parser      # https://www.boost.org/doc/libs/release/doc/html/parser.html
  graph       # https://www.boost.org/doc/libs/release/libs/graph/
  algorithm   # https://www.boost.org/doc/libs/release/libs/algorithm/doc/html/index.html
  bimap       # https://www.boost.org/doc/libs/release/libs/bimap/doc/html/index.html
  icl         # https://www.boost.org/doc/libs/release/libs/icl/doc/html/index.html
  rational    # https://www.boost.org/doc/libs/release/libs/rational/rational.html
  unordered   # https://www.boost.org/doc/libs/release/libs/unordered/doc/html/unordered.html
)
FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.87.0/boost-1.87.0-cmake.tar.gz
  EXCLUDE_FROM_ALL
  SYSTEM
  OVERRIDE_FIND_PACKAGE
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(Boost)

FetchContent_Declare(
  libassert
  GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
  GIT_TAG        v2.1.2
  EXCLUDE_FROM_ALL
  SYSTEM
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(libassert)

# TODO compare mesh libraries
# CGAL polygon or halfedge modules https://doc.cgal.org/latest/Manual/packages.html
# openmesh https://www.graphics.rwth-aachen.de/media/openmesh_static/Documentations/OpenMesh-Doc-Latest/
# boost.polygon https://www.boost.org/doc/libs/1_86_0/libs/polygon/doc/index.htm

pkg_check_modules(clp REQUIRED IMPORTED_TARGET) # https://www.coin-or.org/Clp/userguide/index.html
pkg_check_modules(cbc REQUIRED IMPORTED_TARGET) # https://www.coin-or.org/Cbc/cbcuserguide.html

add_compile_options(
  -Wall -Wextra -Wpedantic -Werror
  -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable
  -Wno-comma-subscript
)
link_libraries(
  Boost::parser Boost::graph
  Eigen3::Eigen
  libassert::assert
  PkgConfig::clp PkgConfig::cbc
)
add_library(utilmod)
target_sources(utilmod
  PUBLIC
  FILE_SET CXX_MODULES FILES
  utilmod.cc
)
include_directories(PUBLIC "${PROJECT_SOURCE_DIR}")

file(GLOB srcs RELATIVE "${PROJECT_SOURCE_DIR}" 202*/*.cc)
foreach(src ${srcs})
  get_filename_component(year ${src} DIRECTORY)
  get_filename_component(day ${src} NAME_WE)
  set(name ${year}-${day})
  add_executable(${name} ${src})
  install(TARGETS ${name})
endforeach()

FetchContent_Declare(
  libmorton
  GIT_REPOSITORY https://github.com/Forceflow/libmorton
  GIT_TAG v0.2.12
  EXCLUDE_FROM_ALL
  SYSTEM
  OVERRIDE_FIND_PACKAGE
)
FetchContent_MakeAvailable(libmorton)

target_link_libraries(2025-8 libmorton::libmorton)

target_link_libraries(2025-test PRIVATE utilmod)
