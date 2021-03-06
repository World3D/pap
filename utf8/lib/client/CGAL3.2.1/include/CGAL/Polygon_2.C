// Copyright (c) 1997  Utrecht University (The Netherlands),
// ETH Zurich (Switzerland), Freie Universitaet Berlin (Germany),
// INRIA Sophia-Antipolis (France), Martin-Luther-University Halle-Wittenberg
// (Germany), Max-Planck-Institute Saarbruecken (Germany), RISC Linz (Austria),
// and Tel-Aviv University (Israel).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; version 2.1 of the License.
// See the file LICENSE.LGPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.2-branch/Polygon/include/CGAL/Polygon_2.C $
// $Id: Polygon_2.C 28567 2006-02-16 14:30:13Z lsaboret $
// 
//
// Author(s)     : Wieger Wesselink <wieger@cs.ruu.nl>

#include <CGAL/Polygon_2.h>

//-----------------------------------------------------------------------//
//                          operator==
//-----------------------------------------------------------------------//

CGAL_BEGIN_NAMESPACE

namespace i_polygon {
template <class Equal_2, class Point_2>
class Equal_pred {
    Equal_2 m_equal_2;
    Point_2 m_pt;
public:
    Equal_pred(Equal_2 equal_2, Point_2 const &pt)
    : m_equal_2(equal_2), m_pt(pt) {}
    bool operator()(Point_2 const &pt) const
    { return m_equal_2(m_pt, pt); }
};
}

template <class Traits_P, class Container1_P, class Container2_P>
bool operator==( const Polygon_2<Traits_P,Container1_P> &x,
                 const Polygon_2<Traits_P,Container2_P> &y )
{
  if (&x == &y)
    return true;
  typedef typename Traits_P::Equal_2 Equal_2;
  typedef typename Traits_P::Point_2 Point_2;
//  CGAL_polygon_precondition( (x.size() != 0) || (y.size() != 0));
  if ((x.size() == 0) && (y.size() == 0)) return true;

  if (x.size() != y.size()) return false;
  Equal_2 equal_2 = x.traits_member().equal_2_object();
  typename Polygon_2<Traits_P,Container1_P>::Vertex_const_iterator x_iter =
    x.vertices_begin();

  typename Polygon_2<Traits_P,Container2_P>::Vertex_const_iterator y_iter =
    std::find_if(y.vertices_begin(), y.vertices_end(),
    i_polygon::Equal_pred<Equal_2, Point_2>(equal_2, *x.vertices_begin()));

  // if y doesn't contain the first point of x ...
  if (y_iter == y.vertices_end()) return false;

  ++x_iter;
  ++y_iter;

  while (y_iter != y.vertices_end()) {
    if (!equal_2(*x_iter, *y_iter)) return false;
    ++x_iter;
    ++y_iter;
  }

  y_iter = y.vertices_begin();
  while (x_iter != x.vertices_end()) {
    if (!equal_2(*x_iter, *y_iter)) return false;
    ++x_iter;
    ++y_iter;
  }

  return true;
}

//-----------------------------------------------------------------------//
//                          operator>>
//-----------------------------------------------------------------------//

template <class Traits_P, class Container_P>
std::istream &operator>>(std::istream &is, Polygon_2<Traits_P,Container_P>& p)
{
  int n; // number of vertices
  is >> n;
  typename Traits_P::Point_2 point;
 
  if (is) {
      p.erase(p.vertices_begin(),p.vertices_end());
      for (int i=0; i<n; i++) {
        is >> point;
        p.push_back(point);
      }
  }
 
  return is;
}

//-----------------------------------------------------------------------//
//                          operator<<
//-----------------------------------------------------------------------//

template <class Traits_P, class Container_P>
std::ostream
&operator<<(std::ostream &os, const Polygon_2<Traits_P,Container_P>& p)
{
  typename Polygon_2<Traits_P,Container_P>::Vertex_const_iterator i;

  switch(os.iword(IO::mode)) {
    case IO::ASCII :
      os << p.size() << ' ';
      for (i = p.vertices_begin(); i != p.vertices_end(); ++i) {
        os << *i << ' ';
      }
      return os;

    case IO::BINARY :
      os << p.size();
      for (i = p.vertices_begin(); i != p.vertices_end(); ++i) {
        os << *i;
      }
      return os;

    default:
      os << "Polygon_2(" << std::endl;
      for (i = p.vertices_begin(); i != p.vertices_end(); ++i) {
        os << "  " << *i << std::endl;
      }
      os << ")" << std::endl;
      return os;
  }
}

CGAL_END_NAMESPACE

//-----------------------------------------------------------------------//
//                          transform
//-----------------------------------------------------------------------//

#ifdef CGAL_REP_CLASS_DEFINED

CGAL_BEGIN_NAMESPACE

template <class Transformation, class Traits_P, class Container_P>
Polygon_2<Traits_P,Container_P>
transform(const Transformation& t, const Polygon_2<Traits_P,Container_P>& p)
{
  typedef typename Polygon_2<Traits_P,Container_P>::Vertex_const_iterator VI;
  Polygon_2<Traits_P,Container_P> result;
  for (VI i = p.vertices_begin(); i != p.vertices_end(); ++i)
    result.push_back(t(*i));
  return result;
}

CGAL_END_NAMESPACE

#endif // CGAL_REP_CLASS_DEFINED

