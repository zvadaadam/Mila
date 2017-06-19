
cmake_minimum_required(VERSION 3.7)
project(Mila)

# LLVM
find_package(LLVM REQUIRED CONFIG)

set (CMAKE_CXX_STANDARD 11)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

message("${LLVM_INCLUDE_DIRS}")
message("${LLVM_DEFINITIONS}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITION})

llvm_map_components_to_libnames(llvm_libs x86asmparser analysis instcombine scalaropts transformutils  support core irreader executionengine interpreter mc support nativecodegen)

set(SOURCE_FILES main.cpp ast.cpp ast.h parser.cpp parser.h lexan.cpp lexan.h tabsym.cpp tabsym.h input.cpp input.h)

add_executable(Mila ${SOURCE_FILES})
target_link_libraries(Mila ${llvm_libs})