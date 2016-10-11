#include "swifft.h"

ZN mulTable ALIGN;
ZW fftTable[1<<W] ALIGN;

long center(long x) { 
  // center(x) = x mod P, |center(x)| <= P/2
  long y = ((x % P)+P)%P; 
  if (y > P/2) y -= P;
  return y; 
}

int rev(int i, int bound) { // bit reversal permutation
  int irev=0;
  for(i |= bound; i>1; i>>=1)
    irev = (irev<<1) | (i&1);
  return irev;
}

int bit(int x, int b) // extract b-th bit of x
{ return (x >> b) & 1; }

void setupTables() {
  long omega_pow[2*N]; // the powers of omega
  omega_pow[0] = 1;
  for(int i = 1; i < 2*N; i++) 
    omega_pow[i] = center(omega_pow[i-1] * OMEGA);  

  for (int i=0; i<N/W; i++)
    for (int j=0; j<W; j++)
      mulTable[i][j] = omega_pow[rev(i,N/W)*(2*j+1)];

  for (int x=0; x<(1<<W); x++)
    for (int j=0; j<W; j++) {
      long tmp = 0;
      for (int k=0; k<W; k++)
	tmp += omega_pow[((N/W)*(2*j+1)*rev(k,W))%(2*N)]*bit(x,k);
      fftTable[x][j] = center(tmp);
    }
}

void setupKey(HashKey key) { // Reduce key mod P and initialize keysum 
  for (int j=0; j<N; j++) {
    long sum = 0;
    for (int i=0; i<M; i++) {
      key.keyval[i][j/W][j%W] = center(key.keyval[i][j/W][j%W]);
      sum += key.keyval[i][j/W][j%W];
    }
    key.keysum[j/W][j%W] = center(sum * ((P-1)/2));
  }
}
