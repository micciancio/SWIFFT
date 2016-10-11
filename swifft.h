/* File swifft.h */
#ifndef SWIFFT_H
#define SWIFFT_H

#include <stdint.h>

#define N 64       // Vector length. Must be a power of 2.
#define P 257      // Modulus. Integer arithmetic is performed modulo P. 
#define M 16       // Number of vectors per block. (= Mstate+Mdata)
#define W 8        // Number of coordinates in vectorized register
#define Mstate 9       // (N/W)+1: Hash function State size, in blocks of N bits
#define Mdata  7       // (N/W)-1: Hash function Data size,  in blocks of N bits
#define OMEGA 42   // (2N)th primitive root of unity modulo P

#define ALIGN __attribute__ ((aligned (16)))

typedef int16_t Z; // 16 bit integer

typedef Z ZW __attribute__ ((vector_size (W*sizeof(Z)))); // Z^W
typedef ZW ZN[N/W]; // Z^N, in blocks of size W

typedef uint8_t BitsW;    // block of W=8 input bits
typedef BitsW BitsN[N/W]; // N input bits, in blocks of size W
typedef BitsN HashState[Mstate];
typedef BitsN HashData[Mdata]; 
typedef struct {
  ZN keyval[M] ALIGN;
  ZN keysum ALIGN;
} HashKey;

void setupTables();
void setupKey(HashKey key);
void SwiFFT(const HashKey key, HashState state, const HashData data);

extern ZN mulTable       ALIGN;
extern ZW fftTable[1<<W] ALIGN;

#endif /* SWIFFT_H */
