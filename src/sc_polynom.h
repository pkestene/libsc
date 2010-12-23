/*
  This file is part of the SC Library.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2010 The University of Texas System

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

#ifndef SC_POLYNOM_H
#define SC_POLYNOM_H

#include <sc_containers.h>

SC_EXTERN_C_BEGIN;

/* Data structure and basic constructor and destructor */

typedef struct sc_polynom
{
  int                 degree;   /* Degree of polynom sum_i=0^degree c_i x^i */
  sc_array_t         *c;        /* Array of type double stores coefficients.
                                   It holds degree < c->elem_count. */
}
sc_polynom_t;

/** Create the zero polynom */
sc_polynom_t       *sc_polynom_new (void);

/** Destroy all memory used by a polynom */
void                sc_polynom_destroy (sc_polynom_t * p);

/* Alternate constructors */

/** Create a polynom from given monomial coefficients.
 * \param[in] degree            Degree of the polynom, >= 0.
 * \param[in] coefficients      Monomial coefficients [0..degree].
 */
sc_polynom_t       *sc_polynom_new_from_coefficients (int degree,
                                                      const double
                                                      *coefficients);
sc_polynom_t       *sc_polynom_new_from_lagrange (int degree, int which,
                                                  const double *points);

/* Alternate constructors using other polynoms */

sc_polynom_t       *sc_polynom_new_from_scale (const sc_polynom_t * a,
                                               int exponent, double factor);
sc_polynom_t       *sc_polynom_new_from_sum (const sc_polynom_t * a,
                                             const sc_polynom_t * b);
sc_polynom_t       *sc_polynom_new_from_product (const sc_polynom_t * a,
                                                 const sc_polynom_t * b);

/* Manipulating a polynom */

/** Set degree of a polynomial.
 * If the new degree is larger than the old degree, the new coefficients
 * are set to zero.  If the new degree is smaller, the smaller set of
 * coefficients is unchanged.
 */
void                sc_polynom_set_degree (sc_polynom_t * p, int degree);
void                sc_polynom_set_value (sc_polynom_t * p, double value);

/** Scale a polynom by a monomial.
 * \param[in] exponent  Exponent of the monomial, >= 0.
 * \param[in] factor    Prefactor of the monomial.
 */
void                sc_polynom_scale (sc_polynom_t * p,
                                      int exponent, double factor);

/** Modify a polynom by adding another.
 * \param[in,out] p     The polynom p will be set to p + q.
 * \param[in] q         The polynom that is added to p; it is not changed.
 */
void                sc_polynom_add (sc_polynom_t * p, const sc_polynom_t * q);

/** Modify a polynom by substracting another.
 * \param[in,out] p     The polynom p will be set to p - q.
 * \param[in] q         The polynom that is substracted from p; not changed.
 */
void                sc_polynom_sub (sc_polynom_t * p, const sc_polynom_t * q);

/** Perform the well-known BLAS-type operation Y := A * X + Y.
 * \param[in] A         The factor for multiplication.
 * \param[in] X         The polynom X is used as input only.
 * \param[in,out] Y     The polynom that A * X is added to in-place.
 */
void                sc_polynom_AXPY (double A, const sc_polynom_t * X,
                                     sc_polynom_t * Y);

SC_EXTERN_C_END;

#endif /* !SC_POLYNOM_H */