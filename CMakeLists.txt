
cmake_minimum_required(VERSION 3.7)
project(Mila)

# LLVM
# set(LLVM_INSTALL_PREFIX "/Users/adamzvada/Documents/School/6.semester/PJP/LLVM_DEV/llvm")
# set(LLVM_DIR "/Users/adamzvada/Documents/School/6.semester/PJP/LLVM_DEV/llvm/cmake/modules")
find_package(LLVM REQUIRED CONFIG)

set (CMAKE_CXX_STANDARD 11)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

message("${LLVM_INCLUDE_DIRS}")
message("${LLVM_DEFINITIONS}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITION})

# include_directories( ${LLVM_ROOT}/include )
# link_directories( ${LLVM_ROOT}/lib )

llvm_map_components_to_libnames(llvm_libs x86asmparser analysis instcombine scalaropts transformutils  support core irreader executionengine interpreter mc support nativecodegen)
#        x86 x86asmparser x86asmprinter x86codegen x86desc x86disassembler x86info x86utils )

set(SOURCE_FILES main.cpp ast.cpp ast.h parser.cpp parser.h lexan.cpp lexan.h tabsym.cpp tabsym.h input.cpp input.h)

add_executable(Mila ${SOURCE_FILES})
target_link_libraries(Mila ${llvm_libs})