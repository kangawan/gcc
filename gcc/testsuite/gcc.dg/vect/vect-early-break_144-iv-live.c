/* Test that a linear induction variable live after an early-break loop is
   computed using scalar math (base + niters_var * step) at both the early
   break exit and the main (no-match/fallthrough) exit, rather than
   extracting from the vector IV register.  This makes the vector IV inside
   the loop dead, allowing DCE to remove redundant vmv/vadd instructions on
   targets like RVV.  */

/* { dg-add-options vect_early_break } */
/* { dg-do run } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */
/* { dg-additional-options "-fno-tree-scev-cprop" } */

/* { dg-final { scan-tree-dump "LOOP VECTORIZED" "vect" } } */
/* { dg-final { scan-tree-dump "using scalar math for linear IV live-out on early break exit" "vect" } } */
/* { dg-final { scan-tree-dump "using scalar math for linear IV live-out on main exit" "vect" } } */

#include "tree-vect.h"

#define N 64

int arr[N];

/* A typical find_if style loop: the loop index i is live after the loop.
   The vectorizer should compute i = start + niters_var * 1 at the early
   break exit, and i = start + iter_var * 1 at the main (no-match) exit,
   rather than extracting i from a vector register.  */
__attribute__((noipa))
int find_first (int start, int val)
{
  int i;
  for (i = start; i < N; i++)
    if (arr[i] == val)
      break;
  return i;
}

extern void abort (void);

int main (void)
{
  check_vect ();

  /* Initialize: arr[k] = k.  */
  for (int k = 0; k < N; k++)
    arr[k] = k;

  /* No match: should return N (fell off the end — main exit).  */
  if (find_first (0, 999) != N)
    abort ();

  /* Match at index 7 (early break exit).  */
  if (find_first (0, 7) != 7)
    abort ();

  /* Match at index 20, starting from index 15 (early break exit).  */
  if (find_first (15, 20) != 20)
    abort ();

  /* No match starting from a non-zero offset (main exit).  */
  if (find_first (10, 999) != N)
    abort ();

  return 0;
}
