# Este arquivo CMakeLists.txt foi adaptado a partir do projeto castor
# do PET INF/UFRGS (https://github.com/petcomputacaoufrgs/castor-fcg),
# com algumas modificações vindas do arquivo CMakeLists.txt criado
# pelos alunos Luis Melo e Santiago Gonzaga em 2023/1.

# Define the directory containing the source files
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src) # Assuming 'src' is a subdirectory

# Collect all .c/.cpp sources recursively under src/
file(GLOB_RECURSE SOURCES
  CONFIGURE_DEPENDS
  LIST_DIRECTORIES false
  "${SOURCE_DIR}/*.c"
  "${SOURCE_DIR}/*.cpp"
  "${SOURCE_DIR}/*.cc"
  "${SOURCE_DIR}/*.cxx"
)

cmake_minimum_required(VERSION 3.12)

project(LAB_FCG VERSION 1.0.0)

set(CMAKE_CXX_STANDARD          23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS        OFF)

if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${PROJECT_SOURCE_DIR}/bin/Debug")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin/Release")
elseif(UNIX)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/Linux")
endif()

# -----------------------------------------------------------------------------
# Create an INTERFACE target for each direct subfolder under src/
# This makes it easy to consume headers from a specific module using
#   target_link_libraries(my_target PRIVATE ${PROJECT_NAME}::module)
# where `module` is the folder name under `src/` (for example `engine`).
# The INTERFACE target exports the folder as an include directory only.
# -----------------------------------------------------------------------------

# Sanitize project name to a CMake-safe identifier for aliasing
string(REGEX REPLACE "[^A-Za-z0-9_]" "_" PROJECT_SAFE "${PROJECT_NAME}")

# Get direct subdirectories of ${SOURCE_DIR}
file(GLOB CHILDREN RELATIVE ${SOURCE_DIR} "${SOURCE_DIR}/*")

# Directory in the build tree where forwarding headers will be generated.
# Using the build directory avoids writing into the source tree.
set(GEN_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated_includes")
file(MAKE_DIRECTORY "${GEN_INCLUDE_DIR}")

foreach(child ${CHILDREN})
  if(IS_DIRECTORY "${SOURCE_DIR}/${child}")
    # Create an interface target name and an alias
    set(_iface_target "${PROJECT_SAFE}_${child}_iface")

    # Create INTERFACE library with include directories pointing to the subfolder
    # and the src/ root so consumers can include headers as <${child}/...>
    add_library(${_iface_target} INTERFACE)
    target_include_directories(${_iface_target} INTERFACE
      ${SOURCE_DIR}/${child}
      ${SOURCE_DIR}
      $<BUILD_INTERFACE:${GEN_INCLUDE_DIR}>
    )

    # Create a convenient namespaced alias: e.g. <PROJECT_SAFE>::engine
    # Only create the alias if it doesn't already exist.
    set(_alias_name "${PROJECT_SAFE}::${child}")
    if(NOT TARGET ${_alias_name})
      add_library(${_alias_name} ALIAS ${_iface_target})
    endif()
  endif()
endforeach()

# -----------------------------------------------------------------------------
# Build small libraries found under lib-src/
# For each direct subdirectory of lib-src, if it contains C/C++ sources, create
# a STATIC library target named ${PROJECT_SAFE}::lib_<name> and add its include
# directory so consumers can use #include <Name/header.h>
# -----------------------------------------------------------------------------
set(LIBSRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib-src)
file(GLOB LIBSRC_CHILDREN RELATIVE ${LIBSRC_DIR} "${LIBSRC_DIR}/*")
foreach(lib ${LIBSRC_CHILDREN})
  if(IS_DIRECTORY "${LIBSRC_DIR}/${lib}")
    # Collect C/C++ sources in that lib folder (non-recursive)
    file(GLOB LIB_SOURCES
      LIST_DIRECTORIES false
      "${LIBSRC_DIR}/${lib}/*.c"
      "${LIBSRC_DIR}/${lib}/*.cpp"
      "${LIBSRC_DIR}/${lib}/*.cc"
      "${LIBSRC_DIR}/${lib}/*.cxx"
    )
    if(LIB_SOURCES)
      # Create a static library target
      set(_lib_target "${PROJECT_SAFE}_lib_${lib}")
      add_library(${_lib_target} STATIC ${LIB_SOURCES})

      # Expose include directory so users can write #include <${lib}/...>
      # Export the parent lib-src directory so consumers can include
      # headers with the path <${lib}/header.h> (e.g. <Quaternion/Quaternion.h>)
      target_include_directories(${_lib_target} PUBLIC
        $<BUILD_INTERFACE:${LIBSRC_DIR}>
        $<BUILD_INTERFACE:${GEN_INCLUDE_DIR}>
      )

      # Create alias for convenience
      set(_lib_alias "${PROJECT_SAFE}::${lib}")
      if(NOT TARGET ${_lib_alias})
        add_library(${_lib_alias} ALIAS ${_lib_target})
      endif()

      # Also generate a forwarding header in generated_includes so that
      # #include <${lib}/header.h> can be resolved by the build tree
      # (if the lib provides a top-level header name equal to the folder)
      # If the lib folder contains a header named the same as the folder,
      # generate a forwarding header that includes it; otherwise skip.
      if(EXISTS "${LIBSRC_DIR}/${lib}/${lib}.h")
        # If a generated file with the same name exists (from previous
        # forwarding-header generation), remove it so we can create a
        # directory with that name. This avoids "File exists" errors when
        # switching between file and directory usage in generated_includes.
        if(EXISTS "${GEN_INCLUDE_DIR}/${lib}" AND NOT IS_DIRECTORY "${GEN_INCLUDE_DIR}/${lib}")
          file(REMOVE "${GEN_INCLUDE_DIR}/${lib}")
        endif()
        # Ensure a subdirectory exists inside generated_includes for this lib
        file(MAKE_DIRECTORY "${GEN_INCLUDE_DIR}/${lib}")
        # Generate a forwarding header named <lib>/<lib>.hpp that includes the
        # C header provided by the library. This makes both
        #   #include <Quaternion/Quaternion.h>
        # and
        #   #include <Quaternion/Quaternion.hpp>
        # work for consumers.
        file(GENERATE
          OUTPUT "${GEN_INCLUDE_DIR}/${lib}/${lib}.hpp"
          CONTENT "#pragma once\n#include \"${lib}/${lib}.h\"\n"
        )
        message(STATUS "Will generate forwarding include for lib-src: ${GEN_INCLUDE_DIR}/${lib}/${lib}.hpp")
      endif()
    endif()
  endif()
endforeach()

# Previously we wrote forwarding headers into the source tree. Avoid creating
# real files in the source directory; instead generate them in the build tree
# and add that directory to the INTERFACE include dirs so `#include <child>`
# resolves.
set(GEN_INCLUDE_DIR "${CMAKE_BINARY_DIR}/generated_includes")
file(MAKE_DIRECTORY "${GEN_INCLUDE_DIR}")
foreach(child ${CHILDREN})
  if(IS_DIRECTORY "${SOURCE_DIR}/${child}")
    # Generate a tiny forwarding header in the build directory (NOT the
    # source tree). This allows consumers to write `#include <engine>` and
    # have it include `engine/include.hpp` from the src tree.
    file(GENERATE
      OUTPUT "${GEN_INCLUDE_DIR}/${child}"
      CONTENT "#pragma once\n#include \"${child}/include.hpp\"\n"
    )
    message(STATUS "Will generate forwarding include in build dir: ${GEN_INCLUDE_DIR}/${child}")
  endif()
endforeach()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
message(STATUS
  "Build type: ${CMAKE_BUILD_TYPE}

               Change the build type on the command line with

                   -DCMAKE_BUILD_TYPE=type

               for type in {Release, Debug, RelWithDebInfo}.
")

set(EXECUTABLE_NAME main)

if(NOT SOURCES)
  message(FATAL_ERROR "No source files found under ${SOURCE_DIR}.")
endif()

add_executable(${EXECUTABLE_NAME} ${SOURCES})

# Link the executable to all module alias targets created above so consumers
# that use angle-bracket includes like <engine/...> will have the include
# directories available when linking to the module alias.
foreach(child ${CHILDREN})
  if(IS_DIRECTORY "${SOURCE_DIR}/${child}")
    set(_alias_name "${PROJECT_SAFE}::${child}")
    if(TARGET ${_alias_name})
      target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${_alias_name})
    endif()
  endif()
endforeach()

# Link libraries built under lib-src to the executable so their include
# directories are available when compiling the main target.
foreach(lib ${LIBSRC_CHILDREN})
  if(IS_DIRECTORY "${LIBSRC_DIR}/${lib}")
    set(_lib_alias "${PROJECT_SAFE}::${lib}")
    if(TARGET ${_lib_alias})
      target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${_lib_alias})
    endif()
  endif()
endforeach()

# Treat project-level include headers (third-party libs under include/) as
# system headers so compiler warnings originating from those headers are
# suppressed. This prevents flooding warnings when issues are in external
# libraries rather than your `src/` code.
target_include_directories(${EXECUTABLE_NAME} SYSTEM PRIVATE
  ${PROJECT_SOURCE_DIR}/include
)

# For MSVC, mark external headers as warning-free using /external:W0 when
# supported by the toolset (suppresses warnings coming from external headers).
target_compile_options(${EXECUTABLE_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/external:W0>
)

# Make the compiler pedantic
target_compile_options(${EXECUTABLE_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/WX>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<CXX_COMPILER_ID:MSVC>:/WL>
)

if(WIN32)

  if(MINGW)

    # Aqui tentamos descobrir qual libc do Widows está sendo usada
    # pelo compilador MinGW: msvcrt (antiga) ou ucrt (nova). Também
    # diferenciamos entre um compilador 32-bits (antigo) ou 64-bits.
    # Para isso, buscamos pela ocorrência de algumas strings
    # específicas no output do comando "-v" do GCC, que lista os
    # parâmetros de configuração do compilador.
    # TODO: Testar com compilador llvm/clang.
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} "-v"
      ERROR_VARIABLE  COMPILER_VERSION_OUTPUT
      RESULT_VARIABLE COMPILER_VERSION_RESULT
    )

    if (COMPILER_VERSION_RESULT EQUAL 0)
      # NOTE: É importante que o primeiro teste seja buscando pela
      # string ucrt64 no output do compilador, pois a string "mingw64"
      # sempre aparece no output (mesmo quando ucrt64 é a libc utilizada).
      if (COMPILER_VERSION_OUTPUT MATCHES "ucrt64")
        set(LIBGLFW ${PROJECT_SOURCE_DIR}/lib-ucrt-64/libglfw3.a)
      elseif (COMPILER_VERSION_OUTPUT MATCHES "mingw64")
        set(LIBGLFW ${PROJECT_SOURCE_DIR}/lib-mingw-64/libglfw3.a)
      else()
        set(LIBGLFW ${PROJECT_SOURCE_DIR}/lib-mingw-32/libglfw3.a)
      endif()
    else()
      message(FATAL_ERROR "Failed to get MinGW compiler version.")
    endif()

  elseif(MSVC)
    set(LIBGLFW ${PROJECT_SOURCE_DIR}/lib-vc2022/glfw3.lib)
  else()
    message(FATAL_ERROR "This CMakeLists.txt file only supports MINGW or MSVC toolchain on Windows.")
  endif()

  message(STATUS "LIBGLFW = ${LIBGLFW}")

  target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBGLFW} gdi32 opengl32)

elseif(UNIX)

  # Make the compiler as pedantic as possible for GNU/Clang toolchains
  # and treat warnings as errors. Keep the previous small suppression for
  # unused-function coming from some system code if needed.
  target_compile_options(${EXECUTABLE_NAME} PRIVATE
    # GNU or Clang: enable many warnings and make them errors
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:
      -Wall;-Wextra;-Werror;>
    # AppleClang: use a slightly smaller set (some flags differ on Apple) but still treat as error
    $<$<CXX_COMPILER_ID:AppleClang>:-Wall;-Wextra;-Wpedantic;-Werror>
    # Keep an allowance for unused-function warnings that sometimes appear from system headers
    -Wno-unused-function
  )

  # Add custom target for 'run'
  add_custom_target(run
      COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} ./main
      DEPENDS main
      USES_TERMINAL
  )

  find_package(OpenGL REQUIRED)
  find_package(X11 REQUIRED)
  find_library(MATH_LIBRARY m)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_DL_LIBS}
    ${MATH_LIBRARY}
    ${PROJECT_SOURCE_DIR}/lib-linux/libglfw3.a
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENGL_LIBRARIES}
    ${X11_LIBRARIES}
    ${X11_Xrandr_LIB}
    ${X11_Xcursor_LIB}
    ${X11_Xinerama_LIB}
    ${X11_Xxf86vm_LIB}
  )

endif()
