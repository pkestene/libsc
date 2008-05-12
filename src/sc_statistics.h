/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2008 Carsten Burstedde, Lucas Wilcox.

  The SC Library is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with the SC Library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SC_STATISTICS_H
#define SC_STATISTICS_H

/* sc.h should be included before this header file */

typedef struct sc_statinfo
{
  int                 count;    /* the global count is 52bit accurate */
  double              sum_values, sum_squares, min, max;        /* inout */
  int                 min_at_rank, max_at_rank; /* out */
  double              average, variance, standev;      /* out */
  double              variance_mean, standev_mean;     /* out */
  const char         *variable; /* name of the variable for output */
}
sc_statinfo_t;

typedef struct sc_flopinfo
{
  double              seconds;  /* time from MPI_Wtime() */
  float               rtime;    /* real time */
  float               ptime;    /* process time */
  long long           flpops;   /* floating point operations */
  float               mflops;   /* MFlop/s rate */
}
sc_flopinfo_t;

/**
 * Populate a sc_statinfo_t structure assuming count=1.
 */
void                sc_statinfo_set1 (sc_statinfo_t * stats,
                                      double value, const char *variable);

/**
 * Compute global average and standard deviation.
 * \param [in]     mpicomm   MPI communicator to use.
 * \param [in]     nvars     Number of variables to be examined.
 * \param [in,out] stats     Set of statisitcs for each variable.
 * On input, set the following fields for each variable separately.
 *    count         Number of values for each process.
 *    sum_values    Sum of values for each process.
 *    sum_squares   Sum of squares for each process.
 *    min, max      Minimum and maximum of values for each process.
 *    variable      String describing the variable, or NULL.
 * On output, the fields have the following meaning.
 *    count                        Global number of values.
 *    sum_values                   Global sum of values.
 *    sum_squares                  Global sum of squares.
 *    min, max                     Global minimum and maximum values.
 *    min_at_rank, max_at_rank     The ranks that attain min and max.
 *    average, variance, standev   Global statistical measures.
 *    variance_mean, standev_mean  Statistical measures of the mean.
 */
void                sc_statinfo_compute (MPI_Comm mpicomm, int nvars,
                                         sc_statinfo_t * stats);

/**
 * Version of sc_statistics_statistics that assumes count=1.
 * On input, the field sum_values needs to be set to the value
 * and the field variable must contain a valid string or NULL.
 */
void                sc_statinfo_compute1 (MPI_Comm mpicomm, int nvars,
                                          sc_statinfo_t * stats);

/**
 * Print measured statistics. Should be called on 1 core only.
 * \param [in] full      Boolean: print full information for every variable.
 * \param [in] summary   Boolean: print summary information all on 1 line.
 * \param [in] nout      Stream used for output if not NULL.
 */
void                sc_statinfo_print (int nvars, sc_statinfo_t * stats,
                                       bool full, bool summary, FILE * nout);

/**
 * Start counting times and flops.
 * \param [out] rtime    Initialized to negative real time.
 * \param [out] ptime    Initialized to negative Process time.
 * \param [out] flpops   Initialized to negative total floating point ops.
 */
void                sc_papi_start (float *rtime, float *ptime,
                                   long long *flpops);

/**
 * Start counting times and flops.
 * \param [out] fi   Members will be initialized.
 */
void                sc_flopinfo_start (sc_flopinfo_t * fi);

/**
 * Compute the times, flops and flop rate since the corresponding _start call.
 * \param [in,out] rtime    Real time.
 * \param [in,out] ptime    Process time.
 * \param [in,out] flpops   Floating point operations.
 * \param [out]    mflops   Flop/s rate.
 */
void                sc_papi_stop (float *rtime, float *ptime,
                                  long long *flpops, float *mflops);

/**
 * Compute the times, flops and flop rate since the corresponding _start call.
 * \param [in,out] fi   Flop info structure.
 */
void                sc_flopinfo_stop (sc_flopinfo_t * fi);

#endif /* !SC_STATISTICS_H */
