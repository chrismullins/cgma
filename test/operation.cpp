/**
 * \file operation.cpp
 *
 * \brief operation, another simple C++ driver for CGM
 *
 * This program acts as a simple driver for CGM.  It reads in a geometry,
 * and performs varies checks for bodies, surfaces, curves and vertices.
 */
#include "config.h"
#include "CpuTimer.hpp"
#include "GeometryModifyTool.hpp"
#include "GeometryQueryTool.hpp"
#include "OCCQueryEngine.hpp"
#include "CubitUtil.hpp"
#include "CubitMessage.hpp"
#include "CubitDefines.h"
#include "RefEntity.hpp"
#include "Body.hpp"
#include "RefVolume.hpp"
#include "RefFace.hpp"
#include "RefEdge.hpp"
#include "RefVertex.hpp"
#include "CubitObserver.hpp"
#include "CastTo.hpp"
#include "OCCModifyEngine.hpp"
#include "AppUtil.hpp"
#include "RefEntityFactory.hpp"
#include "RefEdge.hpp"
#include "BodySM.hpp"
#include "Lump.hpp"
#include "OCCLump.hpp"
#include "OCCBody.hpp"
#include "OCCSurface.hpp"
#include "OCCCurve.hpp"
#include "OCCShell.hpp"
#include "TopoDS_Shape.hxx"
#include "RefEntityName.hpp"
#include "RefEntityFactory.hpp"

#include <algorithm>

#ifndef SRCDIR
# define SRCDIR .
#endif

#define STRINGIFY_(X) #X
#define STRINGIFY(X) STRINGIFY_(X)
#define SRCPATH STRINGIFY(SRCDIR) "/"

// forward declare some functions used and defined later
CubitStatus read_geometry(int, char **, bool local = false);
CubitStatus make_Point();
// macro for printing a separator line
#define PRINT_SEPARATOR   PRINT_INFO("=======================================\n");


// main program - initialize, then send to proper function
int main (int argc, char **argv)
{

  CubitObserver::init_static_observers();
    // Initialize the GeometryTool
  
  CGMApp::instance()->startup( argc, argv );
  OCCQueryEngine::instance();
  OCCModifyEngine::instance();

    // If there aren't any file arguments, print usage and exit
  //if (argc == 1) {
  //  PRINT_INFO("Usage: mergechk <geom_file> [<geom_file> ...]\n");
  //  exit(0);
  //}
  
  CubitStatus status = CUBIT_SUCCESS;



  //Do make point.
  status = make_Point();
  if (status == CUBIT_FAILURE) 
     PRINT_INFO("Operation Failed");

  int ret_val = ( CubitMessage::instance()->error_count() );
  if ( ret_val > 0 )
  {
    PRINT_ERROR("Errors found during Mergechk session.\n");
  }
  return ret_val;
  
}

std::string type_from_file_name( const std::string& filename )
{
  size_t dot_pos = filename.find_last_of( '.' );
  if (dot_pos == std::string::npos)
    return std::string();
 
  std::string extension = filename.substr( dot_pos + 1 );
  std::transform( extension.begin(), extension.end(), extension.begin(), tolower );
  if (extension == "occ" || extension == "brep")
    return "OCC";
  else if (extension == "step" || extension == "stp")
    return "STEP";
  else if (extension == "iges" || extension == "igs")
    return "IGES";
  else
    return std::string();
}

/// attribs module: list, modify attributes in a give model or models
/// 
/// Arguments: file name(s) of geometry files in which to look
///
CubitStatus read_geometry(int num_files, const char **argv, bool local) 
{
  CubitStatus status = CUBIT_SUCCESS;
  GeometryQueryTool *gti = GeometryQueryTool::instance();
  assert(gti);
  int i;
  
  PRINT_SEPARATOR;

  for (i = 0; i < num_files; i++) {
    std::string type = type_from_file_name( argv[i] );
    if (type.empty()) // just guess OCC
      type = "OCC";
    std::string filename( local ? "./" : SRCPATH );
    filename += argv[i];
    status = gti->import_solid_model(filename.c_str(), type.c_str());
    if (status != CUBIT_SUCCESS) {
      PRINT_ERROR("Problems reading geometry file %s.\n", filename.c_str());
      abort();
    }
  }
  PRINT_SEPARATOR;

  return CUBIT_SUCCESS;
}

CubitStatus make_Point()
{
  GeometryQueryTool *gti = GeometryQueryTool::instance();
  GeometryModifyTool *gmti = GeometryModifyTool::instance();

  OCCQueryEngine::instance();

  DLIList<Body*> bodies;
  DLIList<RefEntity*>  free_entities;

  // Read in the geometry from files specified on the command line
  const char *argv = "stitch.name_occ";
  CubitStatus status = read_geometry(1, &argv, false);
  if (status == CUBIT_FAILURE) exit(1);
  //Read in 2 volumes.

  gti->bodies(bodies);
  DLIList<Body*> new_bodies;
  DLIList<Body*> from_bodies;
  BodySM* from_body = bodies.get()->get_body_sm_ptr();
  from_bodies.append(bodies.get());
  CubitVector v1(1,1,3);
  CubitVector v2(1,2,2);
  CubitVector v3(1,1,1);
  BodySM* mid_plane;
  status = OCCModifyEngine::instance()->get_mid_plane(v1, v2, v3, from_body, mid_plane);
  if(mid_plane)
  {
    Body *midplane_body;
    midplane_body = gti->make_Body(mid_plane);
    double d = midplane_body->measure();
    //d = 100
  }

  v1.x(2);
  v2.x(2);
  v3.x(2);
  status = OCCModifyEngine::instance()->get_mid_plane(v1, v2, v3, from_body, mid_plane);
  if(mid_plane)
  {
    Body *midplane_body;
    midplane_body = gti->make_Body(mid_plane);
    double d = midplane_body->measure();
    //d = 70
  }

  status = gmti->webcut_with_plane(from_bodies, v1, v2, v3, new_bodies, CUBIT_TRUE);
  //double d = new_bodies.step_and_get()->measure();
  //CubitVector v = new_bodies.get()->center_point();
  //int n = new_bodies.get()->num_ref_faces();
  // n = 6
  //new bodies has 2 bodies, one has a volume = 10 and the other has a 
  //volume = 50; each of them has 6 ref_faces, of which 3 are new and 3 are
  //remaining (unchanged or modified).

  bodies.clean_out();
  gti->bodies(bodies);
  //delete all entities
  gti->delete_Body(bodies); 
  
  gti->get_free_ref_entities(free_entities);
  assert(free_entities.size() ==0);

  return CUBIT_SUCCESS;
}