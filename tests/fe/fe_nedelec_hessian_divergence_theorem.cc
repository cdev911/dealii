// ---------------------------------------------------------------------
//
// Copyright (C) 2003 - 2018 by the deal.II authors
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


// Observe how the values of the shape functions change as we make a
// cell smaller and smaller. Evaluate the values with FEFaceValues, to
// make sure the values scale as in rt_10 where we used FEValues
//
// the test used to fail because of the issue with computing the
// normals using FEFaceValue, where FEFaceValue by accident uses the
// *face* mapping, not the *cell* mapping to compute the Piola
// transform (leading to a missing power of h in the determinant)

#include <deal.II/base/function.h>
#include <deal.II/base/quadrature_lib.h>

#include <deal.II/dofs/dof_handler.h>

#include <deal.II/fe/fe_dgq.h>
#include <deal.II/fe/fe_nedelec.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/fe_raviart_thomas.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/fe/fe_values.h>
#include <deal.II/fe/mapping_q1.h>

#include <deal.II/grid/grid_generator.h>
#include <deal.II/grid/manifold_lib.h>

#include <deal.II/lac/vector.h>

#include <sstream>

#include "../tests.h"



template <int dim>
Tensor<1, dim>
ones()
{
  Tensor<1, dim> result;
  for (unsigned int i = 0; i < dim; ++i)
    result[i] = 1.0;
  return result;
}

template <int dim>
void
test(const Triangulation<dim> &tr,
     const FiniteElement<dim> &fe,
     const double              tolerance)
{
  DoFHandler<dim> dof(tr);
  dof.distribute_dofs(fe);

  std::stringstream ss;

  deallog << "FE=" << fe.get_name() << std::endl;

  const QGauss<dim> quadrature(4);
  FEValues<dim>     fe_values(fe,
                          quadrature,
                          update_hessians | update_quadrature_points |
                            update_JxW_values);

  const QGauss<dim - 1> face_quadrature(4);
  FEFaceValues<dim>     fe_face_values(fe,
                                   face_quadrature,
                                   update_gradients | update_quadrature_points |
                                     update_normal_vectors | update_JxW_values);

  for (typename DoFHandler<dim>::active_cell_iterator cell = dof.begin_active();
       cell != dof.end();
       ++cell)
    {
      fe_values.reinit(cell);

      deallog << "Cell nodes:" << std::endl;
      for (const unsigned int i : GeometryInfo<dim>::vertex_indices())
        deallog << i << ": ( " << cell->vertex(i) << " )" << std::endl;

      bool cell_ok = true;

      for (unsigned int c = 0; c < fe.n_components(); ++c)
        {
          FEValuesExtractors::Scalar single_component(c);

          for (unsigned int i = 0; i < fe_values.dofs_per_cell; ++i)
            {
              ss << "component=" << c << ", dof=" << i << std::endl;

              Tensor<2, dim> bulk_integral;
              for (const auto q : fe_values.quadrature_point_indices())
                {
                  bulk_integral += fe_values[single_component].hessian(i, q) *
                                   fe_values.JxW(q);
                }

              Tensor<2, dim> boundary_integral;
              for (const unsigned int face : GeometryInfo<dim>::face_indices())
                {
                  fe_face_values.reinit(cell, face);
                  for (const auto q : fe_face_values.quadrature_point_indices())
                    {
                      Tensor<1, dim> gradient =
                        fe_face_values[single_component].gradient(i, q);
                      Tensor<2, dim> gradient_normal_outer_prod =
                        outer_product(gradient,
                                      fe_face_values.normal_vector(q));
                      boundary_integral +=
                        gradient_normal_outer_prod * fe_face_values.JxW(q);
                    }
                }

              if ((bulk_integral - boundary_integral).norm_square() >
                  tolerance * (bulk_integral.norm() + boundary_integral.norm()))
                {
                  deallog << "Failed:" << std::endl;
                  deallog << ss.str() << std::endl;
                  deallog << "    bulk integral=" << bulk_integral << std::endl;
                  deallog << "boundary integral=" << boundary_integral
                          << std::endl;
                  deallog
                    << "Error! difference between bulk and surface integrals is greater than "
                    << tolerance << "!\n\n"
                    << std::endl;
                  cell_ok = false;
                }

              ss.str("");
            }
        }

      deallog << (cell_ok ? "OK: cell bulk and boundary integrals match...\n" :
                            "Failed divergence test...\n")
              << std::endl;
    }
}



template <int dim>
void
test_hyper_ball(const double tolerance)
{
  Triangulation<dim> tr;
  GridGenerator::hyper_ball(tr);

  static const SphericalManifold<dim> boundary;
  tr.set_manifold(0, boundary);

  tr.refine_global(1);

  FE_Nedelec<dim> fe(1);
  test(tr, fe, tolerance);
}


int
main()
{
  initlog();
  deallog << std::setprecision(8);

  test_hyper_ball<2>(1e-6);
  test_hyper_ball<3>(1e-6);

  deallog << "done..." << std::endl;
}
