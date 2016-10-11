/* SwiFFT hash function. */

#include "swifft.h"

#define INLINE inline extern __attribute__((always_inline))
    
INLINE ZW shift(ZW x, int s) // x*2^s mod (P=257)
{ return ((x << s) & 255) - (x >> (8-s)); }

// Reduces mm mod P=257 to the range {-127,383}
INLINE ZW qReduce(ZW x) // (x mod 256) - floor(x/256) 
{ return (x & 255) - (x >> 8); } 

// Reduces mm mod P=257 to the range {0,..,(P-1)=256}
INLINE ZW modP(ZW mm){
  ZW tmp = qReduce(qReduce(mm)); 
  return tmp ^ ((tmp == -1) & (-257)); 
}

#define AddSub(a, b) { ZW tmp = b; b = a - b; a = a + tmp; }

INLINE void FFT(const BitsN t, ZN u) {
  int i;
  ZN v;
    
  for (i=0; i<N/W; i++) v[i] = fftTable[t[i]] * mulTable[i];

  AddSub(v[0],v[1]);
  AddSub(v[2],v[3]);
  AddSub(v[4],v[5]);
  AddSub(v[6],v[7]);
    
  v[2] = qReduce(v[2]);  AddSub(v[0],v[2]);
  v[3] = shift(v[3],4);  AddSub(v[1],v[3]);
  v[6] = qReduce(v[6]);  AddSub(v[4],v[6]);
  v[7] = shift(v[7],4);  AddSub(v[5],v[7]);

  v[4] = qReduce(v[4]);
  v[5] = shift(v[5],2);
  v[6] = shift(v[6],4);
  v[7] = shift(v[7],6);
  
  u[0] = v[0]+v[4];
  u[4] = v[0]-v[4];
  u[1] = v[1]+v[5];
  u[5] = v[1]-v[5];
  u[2] = v[2]+v[6];
  u[6] = v[2]-v[6];
  u[3] = v[3]+v[7];
  u[7] = v[3]-v[7];
}

// ZW parameters must be aligned (16 bytes)
void SwiFFT(const HashKey key, HashState hash, const HashData data) {
  ZN fft;
  ZN out;
  for (int j=0; j<N/W; j++) out[j] = key.keysum[j];
  for (int i=0; i<M; i++) {
    if (i < Mstate) FFT(hash[i],  fft);
    else        FFT(data[i-Mstate],fft);
    for (int j=0; j<N/W; j++) 
      out[j] += qReduce((qReduce(fft[j]) - ((P-1)/2)) * key.keyval[i][j]);    
  }
  ZW overflow;
  overflow = overflow ^ overflow;
  for (int j=0; j<N/W; j++) {
    out[j] = modP(out[j]);
    for (int i=0; i<W; i++)
      hash[j][i] = (BitsW) out[j][i];
    overflow = (overflow << 1) | (out[j] >> 8);
  }
  for (int i=0; i<W; i++)
    hash[N/W][i] = (BitsW) overflow[i];
}

