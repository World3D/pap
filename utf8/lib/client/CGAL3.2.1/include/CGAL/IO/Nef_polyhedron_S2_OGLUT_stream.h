// Copyright (c) 1997-2000  Max-Planck-Institute Saarbruecken (Germany).
// All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.2-branch/Nef_S2/include/CGAL/IO/Nef_polyhedron_S2_OGLUT_stream.h $
// $Id: Nef_polyhedron_S2_OGLUT_stream.h 29754 2006-03-24 13:08:31Z hachenb $
// 
//
// Author(s)     : Michael Seel <seel@mpi-sb.mpg.de>

#ifndef CGAL_NEF_POLYHEDRON_S2_OGLUT_STREAM_H
#define CGAL_NEF_POLYHEDRON_S2_OGLUT_STREAM_H

#include <CGAL/Nef_polyhedron_S2.h>
#include <CGAL/Nef_S2/SM_visualizor.h>
#include <string>

CGAL_BEGIN_NAMESPACE


struct OGLUT_stream { // dummy class
  OGLUT_stream() {}
  void display() { CGAL::OGL::start_viewer(); }
};

static OGLUT_stream ogl;

template <typename K,typename I,typename M>
CGAL::OGLUT_stream& operator<<(CGAL::OGLUT_stream& ogls, 
			       const Nef_polyhedron_S2<K,I,M>& P)
{
  typedef Nef_polyhedron_S2<K,I,M> Polyhedron;
  typedef typename Polyhedron::Sphere_map Sphere_map;
  typedef typename Polyhedron::Sphere_kernel Sphere_kernel;
  typedef CGAL::SM_visualizor<Polyhedron> Visualizor;
  CGAL::OGL::add_sphere();
  Visualizor V(&P,CGAL::OGL::spheres_.back()); V.draw_map();
  // CGAL::OGL::spheres_.back().print();
  return ogls;
}

static CGAL::OGLUT_stream& operator<<(CGAL::OGLUT_stream& ogls, 
			              const char* s)
{
  CGAL::OGL::titles_.push_back(std::string(s));
  return ogls;
}

CGAL_END_NAMESPACE

#endif // CGAL_NEF_POLYHEDRON_S2_OGLUT_STREAM_H


