/* Test that -mrvv-early-break-max-lmul= limits LMUL for early break loops.  */
/* { dg-do compile } */
/* { dg-options "-march=rv64gcv -mabi=lp64d -O3 -mrvv-early-break-max-lmul=m1 -fdump-tree-vect-details" } */

#define N 803

unsigned vect_a[N];
unsigned vect_b[N];

unsigned test (unsigned x, int n)
{
  unsigned ret = 0;

  for (int i = 0; i < n; i++)
    {
      vect_b[i] = x + i;

      if (vect_a[i] > x)
	break;

      vect_a[i] = x;
    }

  return ret;
}

/* { dg-final { scan-tree-dump-times "LOOP VECTORIZED" 1 "vect" } } */
/* { dg-final { scan-assembler {e32,m1} } } */
