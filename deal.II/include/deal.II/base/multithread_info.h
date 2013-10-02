// ---------------------------------------------------------------------
// $Id$
//
// Copyright (C) 2000 - 2013 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE at
// the top level of the deal.II distribution.
//
// ---------------------------------------------------------------------

#ifndef __deal2__multithread_info_h
#define __deal2__multithread_info_h
//---------------------------------------------------------------------------


#include <deal.II/base/config.h>
#include <deal.II/base/types.h>
#include <deal.II/base/exceptions.h>

DEAL_II_NAMESPACE_OPEN

/**
 * This class provides information about the system which may be of
 * use in multithreaded programs.  At the moment this is just the
 * number of cpus. If deal.II is compiled with multithreading support,
 * some functions will use multiple threads for their action. Currently
 * the library supports both thread-based and task-based parallelism. @ref threads
 * describes the different uses of each. Thread-based parallel methods will
 * use the member variable n_default_threads of this class as a guide to the
 * number of threads to start.  This variable
 * n_default_threads is set to the number of CPUs by default, but
 * can be adjusted by the user to fit the requirements. The default number of
 * threads used for task-based parallel methods is selected automatically
 * by the Threading Building
 * Blocks library. See @ref threads for more information on this.
 * To set n_default_threads add the following at the start of your main():
 * <code>
 * multithread_info.n_default_threads=1;
 * </code>
 *
 * @ingroup threads
 * @author Thomas Richter, Wolfgang Bangerth, 2000
 */
class MultithreadInfo
{
public:
  /**
   * The constructor determines the
   * number of CPUs in the system.
   * At the moment detection of
   * CPUs is only implemented on
   * Linux computers with the /proc
   * filesystem and on Sun
   * machines.  The number of CPUs
   * present is set to one if
   * detection failed or if
   * detection is not supported.
   */
  MultithreadInfo ();

  /**
   * The number of CPUs in the
   * system.  It is one if
   * detection is not implemented
   * or failed.
   *
   * If it is one, although you
   * are on a multi-processor
   * machine, please refer to the
   * documentation in
   * <tt>multithread_info.cc</tt>
   * near to the <tt>error</tt> directive.
   */
  const unsigned int n_cpus;

  /**
   * The number of threads to use as
   * a default value for all functions
   * that support multithreading.
   * At start time this is <tt>n_cpus</tt> or
   * one, if detection of the number
   * of CPUs is not possible.
   *
   * @deprecated: use set_thread_limit() instead.
   */
  unsigned int n_default_threads DEAL_II_DEPRECATED;

  /*
   * Returns the number of threads to use. This is governed by the number
   * of cores the system has (n_cpus) and can be further restricted by
   * set_thread_limit().
   */
  unsigned int n_threads() const;

  /**
   * Determine an estimate for
   * the memory consumption (in
   * bytes) of this
   * object. Since sometimes
   * the size of objects can
   * not be determined exactly
   * (for example: what is the
   * memory consumption of an
   * STL <tt>std::map</tt> type with a
   * certain number of
   * elements?), this is only
   * an estimate. however often
   * quite close to the true
   * value.
   */
  static std::size_t memory_consumption ();

  /**
   * Sets the maximum number of threads to be used
   * to the minimum of the environment variable DEAL_II_NUM_THREADS
   * and the given parameter (if not the default value). This
   * affects the initialization of the TBB. If neither is given, the
   * default from TBB is used (based on the number of cores in the system).
   *
   * This routine is called automatically by MPI_InitFinalize. Due to
   * limitations in the way TBB can be controlled, only the first call to this
   * method will have any effect. Use the parameter of the MPI_InitFinalize
   * if you have an MPI based code.
   */
  void set_thread_limit(const unsigned int max_threads = numbers::invalid_unsigned_int);


  /**
   * Returns if the TBB is running using a single thread either
   * because of thread affinity or because it is set via a call
   * to set_thread_limit. This is used in the PETScWrappers to
   * avoid using the interface that is not thread-safe.
   */
  bool is_running_single_threaded();
  /**
   * Exception
   */
  DeclException0(ExcProcNotPresent);

private:

  /**
   * Private function to determine
   * the number of CPUs.
   * Implementation for Linux, OSF,
   * SGI, and Sun machines; if no
   * detection of the number of CPUs is
   * supported, or if detection
   * fails, this function returns
   * one.
   */
  static unsigned int get_n_cpus();

  /**
   * variable representing the maximum number of threads
   */
  unsigned int n_max_threads;
};



/**
 * Global variable of type <tt>MultithreadInfo</tt> which you may ask for the
 * number of CPUs in your system, as well as for the default number of
 * threads that multithreaded functions shall use.
 *
 * @ingroup threads
 */
extern MultithreadInfo multithread_info;




//---------------------------------------------------------------------------
DEAL_II_NAMESPACE_CLOSE
// end of #ifndef __deal2__multithread_info_h
#endif
//---------------------------------------------------------------------------
