/* Test that a linear induction variable live after an early break loop is
   correctly computed using scalar math (base + niters * step) rather than
   extracting from the vector IV register.  This enables DCE to remove the
   dead vector IV inside the loop, eliminating redundant vmv/vadd instructions
   on targets like RVV.  */

/* { dg-do run } */
/* { dg-add-options vect_early_break } */
/* { dg-require-effective-target vect_early_break_hw } */
/* { dg-require-effective-target vect_int } */

/* { dg-final { scan-tree-dump "LOOP VECTORIZED" "vect" } } */
/* { dg-final { scan-tree-dump "using scalar math for linear IV live-out on early break exit" "vect" } } */

#include "tree-vect.h"

#define N 64

int arr[N];

/* A typical find_if style loop: the loop index i is live after the loop.
   The vectorizer should compute i = start + niters_var * 1 rather than
   extracting i from a vector register, allowing the vector IV to be DCE'd. */
__attribute__((noipa))
int find_first (int start, int val)
{
  int i;
  for (i = start; i < N; i++)
    if (arr[i] == val)
      break;
  return i;
}

/* A loop with a non-unit step IV that is live after break.  */
__attribute__((noipa))
int find_first_stride (int val)
{
  int i;
  for (i = 0; i < N; i += 2)
    if (arr[i] == val)
      break;
  return i;
}

extern void abort ();

int main ()
{
  check_vect ();

  /* Initialize: arr[k] = k.  */
  for (int k = 0; k < N; k++)
    arr[k] = k;

  /* No match: should return N (fell off end).  */
  int r = find_first (0, 999);
  if (r != N)
    abort ();

  /* Match at index 7.  */
  r = find_first (0, 7);
  if (r != 7)
    abort ();

  /* Match at index 20, starting from index 15.  */
  r = find_first (15, 20);
  if (r != 20)
    abort ();

  /* Stride-2 loop: arr[k] = k, so arr[10] = 10.
     find_first_stride (10) should return 10.  */
  r = find_first_stride (10);
  if (r != 10)
    abort ();

  /* Stride-2 loop, no match: should return N (64).  */
  r = find_first_stride (999);
  if (r != N)
    abort ();

  return 0;
}
