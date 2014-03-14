// Copyright (c) 2000  Utrecht University (The Netherlands),
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
// $URL: svn+ssh://scm.gforge.inria.fr/svn/cgal/branches/CGAL-3.2-branch/Intersections_2/include/CGAL/Bbox_2_Line_2_intersection.h $
// $Id: Bbox_2_Line_2_intersection.h 31166 2006-05-17 16:30:56Z spion $
// 
//
// Author(s)     : Geert-Jan Giezeman


#ifndef CGAL_BBOX_2_LINE_2_INTERSECTION_H
#define CGAL_BBOX_2_LINE_2_INTERSECTION_H

#include <CGAL/Bbox_2.h>
#include <CGAL/Line_2.h>
//#include <CGAL/Segment_2.h>
//#include <CGAL/Point_2.h>
#include <CGAL/kernel_assertions.h>
#include <CGAL/number_utils.h>

CGAL_BEGIN_NAMESPACE

class Bbox_2_Line_2_pair_impl;

class Bbox_2_Line_2_pair {
public:
    enum Intersection_results {NO_INTERSECTION, POINT, SEGMENT};
    Bbox_2_Line_2_pair() ;
    Bbox_2_Line_2_pair(Bbox_2_Line_2_pair const &);
    Bbox_2_Line_2_pair(Bbox_2 const &bbox,
                            double line_a, double line_b, double line_c);
    ~Bbox_2_Line_2_pair() ;
    Bbox_2_Line_2_pair &operator=(Bbox_2_Line_2_pair const &o);
    // set_bbox(Bbox_2 const &bbox);
    // set_line(double line_a, double line_b, double line_c);
    Intersection_results intersection_type() const;
    bool intersection(double &x, double &y) const;
    bool intersection(double &x1, double &y1, double &x2, double &y2) const;
protected:
    Bbox_2_Line_2_pair_impl *pimpl;
};

template <class Line>
Bbox_2_Line_2_pair intersection_computer_line_2(
    Bbox_2 const &bbox, Line const &line)
{
    return Bbox_2_Line_2_pair(bbox, to_double(line->a()),
        to_double(line->b()), to_double(line->c()));
}

inline bool do_intersect_line_2(
    const Bbox_2 &box, double line_a, double line_b, double line_c)
{
    Bbox_2_Line_2_pair pair(box, line_a, line_b, line_c);
    return pair.intersection_type() != Bbox_2_Line_2_pair::NO_INTERSECTION;
}

template <class Line>
bool do_intersect_line_2(
    Bbox_2 const &bbox, Line const &line)
{
    return do_intersect_line_2(bbox, to_double(line->a()),
        to_double(line->b()), to_double(line->c()));
}

template <class Line>
bool do_intersect_line_2(
    Line const &line, Bbox_2 const &bbox)
{
    return do_intersect_line_2(bbox, to_double(line->a()),
        to_double(line->b()), to_double(line->c()));
}

template <class R>
inline bool do_intersect(
    const Line_2<R> &line,
    const Bbox_2 &box)
{
    return do_intersect(box, line);
}

CGAL_END_NAMESPACE

#endif
