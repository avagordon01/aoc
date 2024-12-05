cmake_minimum_required(VERSION 3.21)
project(starter)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)
set(CMAKE_COMPILE_WARNING_AS_ERROR TRUE)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address,undefined")

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Wno-deprecated-declarations")

#set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=-*,bugprone-*,cert-dcl21-cpp,cert-dcl50-cpp,cert-env33-c,cert-err34-c,cert-err52-cpp,cert-err60-cpp,cert-flp30-c,cert-msc50-cpp,cert-msc51-cpp,cppcoreguidelines-*,google-build-using-namespace,google-explicit-constructor,google-global-names-in-headers,google-readability-casting,google-runtime-int,google-runtime-operator,hicpp-*,-hicpp-vararg,misc-*,modernize-*,performance-*,readability-*,-readability-identifier-length,-misc-use-anonymous-namespace")

include(CTest)
include(FetchContent)

find_package(GTest)

#NOTE include Boost beta for 1.87 parser library
set(BOOST_INCLUDE_LIBRARIES parser assert hana type_index)
FetchContent_Declare(
  Boost
  URL https://github.com/boostorg/boost/releases/download/boost-1.87.0.beta1/boost-1.87.0.beta1-cmake.tar.gz
)
FetchContent_MakeAvailable(Boost)

foreach(i RANGE 1 5)
  set(year 2024)
  add_executable(${year}-${i} ${year}/${i}.cc)
  target_link_libraries(${year}-${i} Boost::parser)
  target_include_directories(${year}-${i} PUBLIC "${PROJECT_SOURCE_DIR}")
  install(TARGETS ${year}-${i})
endforeach()

foreach(i 2 5 6)
  set(year 2023)
  add_executable(${year}-${i} ${year}/${i}.cc)
  target_link_libraries(${year}-${i} Boost::parser)
  target_include_directories(${year}-${i} PUBLIC "${PROJECT_SOURCE_DIR}")
  install(TARGETS ${year}-${i})
endforeach()
