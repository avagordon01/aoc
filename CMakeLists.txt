cmake_minimum_required(VERSION 3.21)
project(aoc CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=-*,bugprone-*,cert-dcl21-cpp,cert-dcl50-cpp,cert-env33-c,cert-err34-c,cert-err52-cpp,cert-err60-cpp,cert-flp30-c,cert-msc50-cpp,cert-msc51-cpp,cppcoreguidelines-*,google-build-using-namespace,google-explicit-constructor,google-global-names-in-headers,google-readability-casting,google-runtime-int,google-runtime-operator,hicpp-*,-hicpp-vararg,misc-*,modernize-*,performance-*,readability-*,-readability-identifier-length,-misc-use-anonymous-namespace")

include(CTest)
include(FetchContent)

find_package(PkgConfig REQUIRED)
find_package(GTest)
find_package(Eigen3 REQUIRED) # https://eigen.tuxfamily.org/dox/group__QuickRefPage.html
# https://libeigen.gitlab.io/docs/group__QuickRefPage.html

#when boost 1.87 makes it to the arch repos
#find_package(Boost 1.87 REQUIRED COMPONENTS <components>)
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
)
# TODO mesh library
# openmesh https://www.graphics.rwth-aachen.de/media/openmesh_static/Documentations/OpenMesh-Doc-Latest/
# boost.polygon https://www.boost.org/doc/libs/1_86_0/libs/polygon/doc/index.htm
FetchContent_MakeAvailable(Boost)
FetchContent_Declare(
  libassert
  GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
  GIT_TAG        v2.1.2
  EXCLUDE_FROM_ALL
  SYSTEM
)
FetchContent_MakeAvailable(libassert)

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
include_directories(PUBLIC "${PROJECT_SOURCE_DIR}")

file(GLOB srcs RELATIVE "${PROJECT_SOURCE_DIR}" 2023/*.cc 2024/*.cc)
foreach(src ${srcs})
  get_filename_component(year ${src} DIRECTORY)
  get_filename_component(day ${src} NAME_WE)
  set(name ${year}-${day})
  add_executable(${name} ${src})
  install(TARGETS ${name})
endforeach()
