option(ITAPS_USE_BABEL "Should ITAPS use Babel to generate C and FORTRAN clients?" "OFF")

if (ITAPS_USE_BABEL)
  add_subdirectory(SIDL)
endif()

set(CGMA_IGEOM_SRCS
	CATag.cpp
	iGeom_CGMA.cc
	iGeomError.cc
)

set(CGMA_IGEOM_HDRS
  iBase.h
  iBase_f.h
  iGeom.h
  iGeom_FCDefs.h
  iGeom_f.h
  #iGeom_protos.h
)

include_directories(
  ${cubit_geom_SOURCE_DIR}
  ${cubit_util_SOURCE_DIR}
  ${cubit_geom_SOURCE_DIR}/ACIS
  ${cubit_geom_SOURCE_DIR}/virtual
  ${cubit_geom_SOURCE_DIR}/facet
  ${cubit_geom_SOURCE_DIR}/cholla
  ${cgma_init_SOURCE_DIR}
  ${cubit_geom_BINARY_DIR}
)

add_library(iGeom ${CGMA_IGEOM_SRCS})
target_link_libraries(iGeom
  LINK_PUBLIC
    ${CGMA_LIBS}
)

install(
  TARGETS iGeom
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINARY_DIR} COMPONENT Runtime
  LIBRARY DESTINATION ${CMAKE_INSTALL_BINARY_DIR} COMPONENT Runtime
  ARCHIVE DESTINATION ${CMAKE_INSTALL_BINARY_DIR} COMPONENT Development
)
install(
  FILES ${CGMA_IGEOM_HDRS}
  DESTINATION include
  COMPONENT Development
)

install(
  FILES iGeom_protos.h
  DESTINATION include
  COMPONENT Development
)

set(IGEOM_CGM_TARGETS "-lcgma_init -lcholla -lcubit_facet -lcubit_facetboolstub -lcubit_geom -lcubit_occ -lcubit_util -lcubit_virtual -liGeom")
set(IGEOM_OCE_LIB_DIR "${OCE_DIR}/..")
string(REPLACE ";" " -l" CGM_EXT_LIBS "${CGMA_OCC_DEPLIBS}")
set(CGM_EXT_LIBS "${CGM_EXT_LIBS}")
set(abs_srcdir "${CMAKE_CURRENT_SOURCE_DIR}")
set(abs_builddir "${CMAKE_CURRENT_BINARY_DIR}")
set(IGEOM_CXX_LDFLAGS "${CMAKE_INSTALL_PREFIX}/lib")
set(IGEOM_INCLUDEDIR  "${CMAKE_INSTALL_PREFIX}/include")
configure_file( iGeom-Defs.inc.in
  ${CMAKE_INSTALL_PREFIX}/lib/iGeom-Defs.inc
  )
