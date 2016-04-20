/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2015 The University of Texas System

  The SC Library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with the SC Library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

/* activate random & srandom functions */
#if !defined(_XOPEN_SOURCE)
#define _XOPEN_SOURCE 500
#elif defined(_XOPEN_SOURCE) && _XOPEN_SOURCE < 500
#undef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

#include <sc.h>
#include <sc_mpi.h>
#include <sc_shmem.h>

int
test_shmem (int count, sc_MPI_Comm comm, sc_shmem_type_t type)
{
  int                 i, p, size, mpiret, check;
  long int           *myval, *recv_self, *recv_shmem, *scan_self, *scan_shmem,
                     *copy_shmem;
  sc_MPI_Comm         intranode, internode;

  sc_shmem_set_type (comm, type);

  mpiret = sc_MPI_Comm_size (comm, &size);
  SC_CHECK_MPI (mpiret);

  sc_mpi_comm_get_node_comms (comm,
			      0,
			      &intranode,
			      &internode);
  
  myval = SC_ALLOC (long int, count);
  for (i = 0; i < count; i++) {
    myval[i] = random ();
  }

  recv_self = SC_ALLOC (long int, count * size);
  scan_self = SC_ALLOC (long int, count * (size + 1));
  mpiret = sc_MPI_Allgather (myval, count, sc_MPI_LONG,
                             recv_self, count, sc_MPI_LONG, comm);
  SC_CHECK_MPI (mpiret);

  for (i = 0; i < count; i++) {
    scan_self[i] = 0;
  }
  for (p = 0; p < size; p++) {
    for (i = 0; i < count; i++) {
      scan_self[count * (p + 1) + i] =
        scan_self[count * p + i] + recv_self[count * p + i];
    }
  }

  recv_shmem = SC_SHMEM_ALLOC (long int, (size_t) count * size, comm, intranode, internode);
  sc_shmem_allgather (myval, count, sc_MPI_LONG,
                      recv_shmem, count, sc_MPI_LONG, comm,
		      intranode, internode);
  check = memcmp (recv_self, recv_shmem, count * sizeof (long int) * size);
  if (check) {
    SC_GLOBAL_LERROR ("sc_shmem_allgather mismatch\n");
    return 1;
  }

  copy_shmem = SC_SHMEM_ALLOC (long int, (size_t) count * size, comm, intranode, internode);
  sc_shmem_memcpy (copy_shmem, recv_shmem,
                   (size_t) count * sizeof (long int) * size, comm,
		   intranode, internode);
  check = memcmp (recv_shmem, copy_shmem, count * sizeof (long int) * size);
  if (check) {
    SC_GLOBAL_LERROR ("sc_shmem_copy mismatch\n");
    return 1;
  }
  SC_SHMEM_FREE (copy_shmem, comm, intranode, internode);
  SC_SHMEM_FREE (recv_shmem, comm, intranode, internode);

  scan_shmem = SC_SHMEM_ALLOC (long int, (size_t) count * (size + 1), comm, intranode, internode);
  sc_shmem_prefix (myval, scan_shmem, count, sc_MPI_LONG, sc_MPI_SUM, comm,
		   intranode, internode);
  check =
    memcmp (scan_self, scan_shmem, count * sizeof (long int) * (size + 1));
  if (check) {
    SC_GLOBAL_LERROR ("sc_shmem_prefix mismatch\n");
    return 2;
  }
  SC_SHMEM_FREE (scan_shmem, comm, intranode, internode);

  sc_MPI_Comm_free(&intranode);
  sc_MPI_Comm_free(&internode);
  
  SC_FREE (scan_self);
  SC_FREE (recv_self);
  SC_FREE (myval);
  return 0;
}

int
main (int argc, char **argv)
{
  int                 mpiret, rank, size;
  int                 count;
  int                 itype;
  int                 retval = 0;

  mpiret = sc_MPI_Init (&argc, &argv);
  SC_CHECK_MPI (mpiret);
  mpiret = sc_MPI_Comm_rank (sc_MPI_COMM_WORLD, &rank);
  SC_CHECK_MPI (mpiret);
  mpiret = sc_MPI_Comm_size (sc_MPI_COMM_WORLD, &size);
  SC_CHECK_MPI (mpiret);

  sc_init (sc_MPI_COMM_WORLD, 1, 1, NULL, SC_LP_DEFAULT);

  srandom (rank);
  for (itype = 0; itype < (int) SC_SHMEM_NUM_TYPES; itype++) {

    SC_GLOBAL_PRODUCTIONF ("sc_shmem type: %s\n",
                           sc_shmem_type_to_string[itype]);
    for (count = 1; count <= 3; count++) {
      int                 retvalin = retval;

      SC_GLOBAL_PRODUCTIONF ("  count = %d\n", count);
      retval +=
        test_shmem (count, sc_MPI_COMM_WORLD, (sc_shmem_type_t) itype);
      if (retval != retvalin) {
        SC_GLOBAL_PRODUCTION ("    unsuccessful\n");
      }
      else {
        SC_GLOBAL_PRODUCTION ("    successful\n");
      }
    }
  }

  sc_finalize ();

  mpiret = sc_MPI_Finalize ();
  SC_CHECK_MPI (mpiret);
  return retval;
}
