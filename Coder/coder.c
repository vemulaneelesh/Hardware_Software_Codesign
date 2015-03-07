// ECE 4530 
// Fall 2013
// Homework 9
#include "system.h"
#include "io.h"
#include "sys/alt_timestamp.h"
#include <stdio.h>
#include <stdlib.h>



int G[7] = {8, 4, 2, 1, 14, 7, 13};
int H[3] = {116, 58, 105};



int readreg(int id) {
// TO BE DESIGNED BY YOU
  //
  //  1. Consult the system,h file and find the base address of the coder coprocessor
  //  2. Use the IORD macro to read from memory-mapped register id

  return ...
}

void writereg(int id, int n) {
 // TO BE DESIGNED BY YOU
  //
  //  1. Consult the system,h file and find the base address of the coder coprocessor
  //  2. Use the IOWR macro to write n into memory-mapped register id

  ...
}




unsigned char xor_redux(unsigned char v, unsigned len) {
  // very inefficient parity computation
  int i, p = 0;
  for (i = 0; i<len; i++) {
    p = p ^ (v & 1);
    v = v >> 1;
  }
  return p;
}

unsigned char encode(unsigned char x) {
  // x is a 4-bit information word.
  // this function computes the code word
  int i;
  unsigned char c = 0;
  for (i=0; i<7; i++) 
    c = (c << 1) | xor_redux(G[i] & x, 4);
  return c;
}

unsigned char syndrome(unsigned char c) {
  // c is a 7-bit code word.
  // this function computes the syndrome
  int i;
  unsigned char s = 0;
  for (i=0; i<3; i++) 
    s = (s << 1) | xor_redux(H[2-i] & c, 7);
  return s & 0x3F;
}

unsigned encodeword(unsigned x) {
  unsigned c;

  c = encode((x >> 24) & 0xF);
  c <<= 8;
  c += encode((x >> 16) & 0xF);
  c <<= 8;
  c += encode((x >>  8) & 0xF);
  c <<= 8;
  c += encode( x        & 0xF);

  return c;
}

unsigned syndromeword(unsigned c) {
  unsigned s;

  s = syndrome((c >> 24) & 0x7F);
  s <<= 8;
  s |= syndrome((c >> 16) & 0x7F);
  s <<= 8;
  s |= syndrome((c >>  8) & 0x7F);
  s <<= 8;
  s |= syndrome( c & 0x7F);

  return s;
}

int main() {
  int i, j;
  unsigned long long swc, hwc;
  int data[1024];
  unsigned sw_code,sw_syndrome,hw_code,hw_syndrome;

  // random information words, densely packed in words
  alt_timestamp_start();
  for (i=0; i<1024; i++) {
    data[i]   = rand() & 0xf;
    data[i] <<= 8;
    data[i]  += rand() & 0xf;
    data[i] <<= 8;
    data[i]  += rand() & 0xf;
    data[i] <<= 8;
    data[i]  += rand() & 0xf;
  }

  // now, compute code word for each data item,
  // and then compute syndrome of resulting code word

  for (i=0; i<1024; i++) 
  {
    sw_code = encodeword(data[i]);
    sw_syndrome = syndromeword(encodeword(data[i]));
    printf("%8x %8x %8x\n", data[i], sw_code, sw_syndrome);
   
    writereg(0,data[i]);
    hw_code = readreg(0);
    hw_syndrome = readreg(1);
    printf("%8x %8x %8x\n", data[i], hw_code,hw_syndrome);
    if(sw_code!= hw_code | sw_syndrome!=hw_syndrome)
    {
      printf("Error in program\n");
      exit(0);
    }
  

  }

  
  printf("Successful\n");
  swc = alt_timestamp();
  for (i=0; i<1024; i++)
  {
    sw_code = encodeword(data[i]);
    sw_syndrome = syndromeword(encodeword(data[i]));
  }
  swc = alt_timestamp() - swc;

  hwc = alt_timestamp();
  for (i=0; i<1024; i++)
  {
    writereg(0,data[i]);
    hw_code = readreg(0);
    hw_syndrome = readreg(1);
  }
  hwc = alt_timestamp() - hwc;

  printf("SW Cycles %lld\n", swc);
  printf("HW Cycles %lld\n", hwc);
  

  return 0;


}
