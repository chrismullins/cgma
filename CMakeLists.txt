project(CGMA)
cmake_minimum_required(VERSION 2.8)

# Place targets in 1 or 2 top-level directories;
# make external linking easy.
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CGMA_BINARY_DIR}/bin")
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
  if(UNIX)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CGMA_BINARY_DIR}/lib")
  else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CGMA_BINARY_DIR}/bin")
  endif()
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CGMA_BINARY_DIR}/lib")
endif()
mark_as_advanced(
  CMAKE_RUNTIME_OUTPUT_DIRECTORY
  CMAKE_LIBRARY_OUTPUT_DIRECTORY
  CMAKE_ARCHIVE_OUTPUT_DIRECTORY
)

# Choose static or shared libraries:
option(BUILD_SHARED_LIBS "Build CGMA with shared libraries." ON)
set(CGMA_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})

# Start with an empty list of libraries that, in combination, form cgm:
set(CGMA_LIBS)
set(CGMA_DEPLIBS)
# Add rules for targets.
# These will alter the environment with set(... PARENT_SCOPE),
# and particularly CGMA_LIBS, CGMA_DEPLIBS.

set(cgma_init_SOURCE_DIR "${CMAKE_SOURCE_DIR}/init")
set(cgma_init_BINARY_DIR "${CMAKE_BINARY_DIR}/init")
set(cubit_geom_SOURCE_DIR "${CMAKE_SOURCE_DIR}/geom")
set(cubit_geom_BINARY_DIR "${CMAKE_BINARY_DIR}/geom")
set(cubit_util_SOURCE_DIR "${CMAKE_SOURCE_DIR}/util")
set(cubit_util_BINARY_DIR "${CMAKE_SOURCE_DIR}/util")
add_subdirectory(init)
add_subdirectory(geom)
set(CGMA_OCC_DEPLIBS ${CGMA_OCC_DEPLIBS})
add_subdirectory(itaps)
add_subdirectory(util)
# Use the environment to configure the cgm.make file:
# Create a list of paths to target libraries.
get_property(CGMA_LIB_LOCS TARGET cubit_util PROPERTY LOCATION)
get_filename_component(CGMA_LIB_LOCS "${CGMA_LIB_LOCS}" DIRECTORY)
if(WIN32)
  # On Windows $(OutDir) gets appended for some reason.
  # I'd rather change it here than the FindCGM.cmake
  string(REPLACE "$(OutDir)" "" CGMA_LIB_LOCS "${CGMA_LIB_LOCS}")
  set(CGMA_LIB_LOCS "${CGMA_LIB_LOCS}Debug;${CGMA_LIB_LOCS}Release;${CMAKE_INSTALL_PREFIX}/lib")
endif()
set(CGMA_LIB_LINKS)
foreach(_lib ${CGMA_LIBS})
  get_property(_libloc TARGET ${_lib} PROPERTY OUTPUT_NAME)
  if (NOT _libloc)
    get_property(_libloc TARGET ${_lib} PROPERTY NAME)
  endif()
  set(CGMA_LIB_LINKS "${CGMA_LIB_LINKS} -l${_libloc}")
endforeach()
set(CGMA_DEPLIB_LINKS)
#foreach(_lib ${CGMA_DEPLIBS})
#  set(CGMA_DEPLIB_LINKS "${CGMA_DEPLIB_LINKS} -l${_lib}")
#endforeach()
configure_file(
  ${CGMA_SOURCE_DIR}/cgm.cmake.in
  ${CGMA_BINARY_DIR}/cgm.make
  @ONLY
)

# Make a space-separated list instead
set(CGMA_LIB_LIST "${CGMA_LIBS}")
string(REPLACE ";" " " CGMA_LIB_LIST "${CGMA_LIB_LIST}")
configure_file(
  ${CGMA_SOURCE_DIR}/CGMConfig.cmake.in
  ${CGMA_BINARY_DIR}/CGMConfig.cmake
  @ONLY
)

# Install top-level headers:
install(
  FILES
    ${CGMA_SOURCE_DIR}/cgm_version.h
    ${CGMA_BINARY_DIR}/cgm.make
  DESTINATION include
  COMPONENT Development
)

install(
  FILES
    ${CGMA_BINARY_DIR}/CGMConfig.cmake
  DESTINATION include
  COMPONENT Development
)
