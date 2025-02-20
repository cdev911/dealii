// ---------------------------------------------------------------------
//
// Copyright (C) 2006 - 2020 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE.md at
// the top level directory of deal.II.
//
// ---------------------------------------------------------------------



// Use a circular domain with boundary mappings, and determine cell for points
// close to the boundary

#include <deal.II/base/numbers.h>

#include <deal.II/fe/mapping_q.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/grid_tools.h>
#include <deal.II/grid/manifold_lib.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/tria_accessor.h>
#include <deal.II/grid/tria_iterator.h>

#include "../tests.h"



void
check(Triangulation<2> &tria)
{
  MappingQ<2> map(5);

  // Test for a number of points, every ten degrees
  for (unsigned int i = 0; i < 200; ++i)
    {
      Point<2> p(std::sin((double)i / 100. * numbers::PI),
                 std::cos((double)i / 100. * numbers::PI));
      p *= 1. - 1e-8;

      std::pair<Triangulation<2>::active_cell_iterator, Point<2>> cell =
        GridTools::find_active_cell_around_point(map, tria, p);

      deallog << cell.first << std::endl;
      for (const unsigned int v : GeometryInfo<2>::vertex_indices())
        deallog << "< " << cell.first->vertex(v) << " > ";
      deallog << "[ " << cell.second << " ] ";

      // Now transform back and check distance
      Point<2> pp = map.transform_unit_to_real_cell(
        cell.first, GeometryInfo<2>::project_to_unit_cell(cell.second));
      deallog << pp.distance(p) << std::endl;
      Assert(pp.distance(p) < 5.e-12, ExcInternalError());
    }
}


int
main()
{
  initlog();
  deallog << std::scientific;
  deallog.precision(8);

  {
    Triangulation<2> coarse_grid;
    GridGenerator::hyper_ball(coarse_grid);
    static const SphericalManifold<2> boundary;
    coarse_grid.set_manifold(0, boundary);
    coarse_grid.refine_global(2);
    check(coarse_grid);
  }
}
