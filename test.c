/* Simple program to test SWIFFT hash function.
   Usage: SWIFFT file [n=1]
   Hash function key is read from first 1024 characters of 'file'.
   Data block is read from first 56 bytes of standard input.
   Run SWIFFT n times, starting with IV=0, on n copies of data block.
*/

#include <stdio.h>
#include <stdlib.h>
#include "swifft.h"

HashKey key ALIGN;
HashState state ALIGN;
HashData data ALIGN;

void initState(HashState st) {
  for (int i=0; i<Mstate; i++)
    for (int j=0; j<N/W; j++)
      st[i][j]=0;
}

void printState(HashState st) {
  for (int i=0; i<Mstate; i++) {
    for (int j=0; j<N/W; j=j+2)
      printf("%02x%02x ", st[i][j], st[i][j+1]);
    printf("\n");
  }
}

void printKey(HashKey k) {
  for (int i=0; i<M; i++) {
    for (int j=0; j<N; j++)
      printf("%3d ", k.keyval[i][j/W][j%W]);
    printf("\n");
  }
}

void printData(HashData d) {
  for (int i=0; i<Mdata; i++) {
    for (int j=0; j<N/W; j=j+2)
      printf("%02x%02x ", d[i][j], d[i][j+1]);
    printf("\n");
  }
}

void readKey(char filename[], HashKey *key) {
  FILE *keyfile = fopen(filename, "r");
  for (int i=0; i<M; i++)
    for (int j=0; j<N; j++)
      key->keyval[i][j/W][j%W] = getc(keyfile);
  fclose(keyfile);
  setupKey(*key);
}

void readInput(HashData data) {
  for (int i=0; i<Mdata; i++) 
    for (int j=0; j<N/W; j++) 
      data[i][j] = getchar();
}
 
int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: SWIFFT file [n=1] <input\n");
    printf("\tRead key from first 1024 characters of 'file'\n");
    printf("\tRead data from first 56 bytes of standard input\n");
    printf("\tRun SWIFFT n times, with IV=0 and n copies of data.\n");
    return 1;
  }

  setupTables();
  initState(state);
  readKey(argv[1],&key);

  readInput(data);
  if (argc == 2) {
    SwiFFT(key, state, data);
    printState(state);
  } else {
    int count = atoi(argv[2]);
    printf("Executing %d times.\n", count);
    for (int i=0; i < count; i++) 
      SwiFFT(key, state, data);
    printState(state);
  }
  return 0;
}
