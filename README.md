# SWIFFT
## An Efficient Lattice-Based Cryptographic Hash Function

This code is open-source software distributed under the terms of the GNU General Public Licence. 
See the file [LICENSE](LICENSE) for complete details on licensing of this software.

This repository contains the implementation of SWIFFT, 
an efficient lattice-based cryptographic hash function, described in the paper 
[**"SWIFFT: a modest proposal for FFT hashing"**](http://dx.doi.org/10.1007/978-3-540-71039-4_4) 
(*V. Lyubashevsky, D. Micciancio, C. Peikert, A. Rosen.* Fast Software Encryption -- FSE 2008, LNCS 5086, pp. 54-72. [[pdf](http://cseweb.ucsd.edu/~daniele/papers/SWIFFT.pdf)]).
This is essentially the original code, used to run the tests in that paper, and written in 2007.
The implementation is optimized for the computers of that time, 
Intel 32-bit CPUs with SSE instruction set. 
The code should compile and run on recent Intel CPUs, and still achieve reasonably good speed, 
but it does not take advantage of the more recent 64-bit architecture and AVX/AVX2 instruction set. 

Code is provided primarily for reproducibility and testing purposes, and it is not meant to be 
a ready-to-use solution for any application. In particular, it only implements the core 
SWIFFT *compression function* for fixed-size messages. (See below.)
A full featured hash function for arbitrary length messages can be easily obtained 
using standard padding and chaining techniques, but it is not included in this code base.

## The SWIFFT compression function 

For a detailed description and security analysis, see the paper. 
These are just implementation specific comments for people interested in testing the function.

SWIFFT is a keyed hash function: it takes as input a key **K** and message **M**, and it produces 
a digest **H**, where

 - The key **K** is a vector of 1024 integers, each in the range *[0,256]*, 
   but it can be chosen by picking each integer (uniformly at random)
   in the range *[0,255]* so that it fits a byte.
 - The message **M** is a vector of 1024 bits (or, equivalently, 128 bytes)
 - The digest **H** is a vector of 64 integers in the range *[0,256]*

Notice how each integer in the digest can take values as large as *256*, which do not fit 
a single byte. (This is necessary because SWIFFT is based on arithmetic modulo the prime
*P=257*.) 
For simplicity, the compression function, as implemented in this code base, encodes the 
digest **H** as a sequence 
of 64 bytes (storing the first 8 bits of each number) followed by 64 additional bits, for a total 
of 576 bit (or 64+8=72 bytes). 
(The last 64 bits are typically 0, and more efficient encoding are possible. 
E.g., by regarding **H** as a single 64-digit number in base 257, it could be represented using 
just 513 bits, rather than 576.)

The compression function is implemented in less than 100 lines of C code, 
in the file [swifft.c](swifft.c) (plus some additional declarations and setup functions in 
[swifft.h](swifft.h) and [setup.c](setup.c).)
The entry point of the compression function is `SwiFFT(key, hash, data)`,
in the file [swifft.c](swifft.c).
It takes a **K**=*key*, and a message **M**=*(hash,data)* conveniently divided into 
a 72-byte *hash* and 56-byte *data* block (for a total of 128 bytes), and 
updates the 72-byte *hash* with the output of the compression function.

## Example/Running

For an example of how to use `SwiFFT` see the file [test.c](test.c). 
The test program can be compiled running `make`, which generates an executable `test`.
Running `test` with no arguments outputs a brief usage message.
You can try the test program by running 

```
./test keyfile
```

which reads a key from the first 128 bytes of keyfile, a 56-byte block of data from standard input,
and outputs the result of the compression function with initialization vector `IV=0`.
Calling 

```
./test keyfile n <input
```

reads 56-bytes from input, and repeatedly applies the compression function 
(always to the same block of data) in chaining mode *n* times, and can be used for 
timing measurements. You can run the tests on any `keyfile` and `input` containing at least 
128 and 56 bytes. E.g., 

```
> make  
gcc  -O3 -msse -funroll-loops -c swifft.c  
gcc  -c setup.c  
gcc  swifft.o  setup.o test.c -o test  
> time ./test test 1000000 <test  
Executing 1000000 times.  
68d6 2a4b 2afc 6abc  
9291 b442 d46a c87e  
6c5d a603 c6ac a12c  
c934 cc8c c800 7d85  
10da cc0f 113b 97c2  
1064 3fb8 10f1 8e8c  
bc24 bb00 39a9 2e79  
bd54 7ba5 9879 cfce  
0000 0000 0000 0000  
   
real	0m0.321s  
user	0m0.320s  
sys	0m0.000s  
```





