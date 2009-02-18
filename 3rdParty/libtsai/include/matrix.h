#ifndef matrix_h
#define matrix_h
/* matrix.h -- define types for matrices using Iliffe vectors
 *
 *************************************************************
 * HISTORY
 * April 1997 Thierry Zimmerman zimmerman@imt.dmt.epfl.ch
 *	Removed all references to the system errno global variable.
 *	It was previously used as a flag in somme operations.
 *	Some (most) systems sure don't like anybody else than themselves
 *	to modify this variable.
 *
 * 02-Apr-95  Reg Willson (rgwillson@mmm.com) at 3M St. Paul, MN
 *      Rewrite memory allocation to avoid memory alignment problems
 *      on some machines.
 */

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
  {
  int		lb1,ub1,lb2,ub2;
  char  	*mat_sto;
  double  	**el;
  } dmat;

void print_mat (dmat mat);
dmat newdmat (int rs, int re, int cs, int ce, int *error);
int matmul (dmat a, dmat b, dmat c);
int matcopy (dmat A, dmat RSLT);
int transpose (dmat A, dmat ATrans);
double matinvert (dmat a);
int solve_system (dmat M, dmat a, dmat b);
#define freemat(m) free((m).mat_sto) ; free((m).el)

#ifdef __cplusplus
}
#endif

#endif
