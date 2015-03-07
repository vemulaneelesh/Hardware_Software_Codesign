#include <system.h>
#include <stdio.h>
#include <stdlib.h>
#include "sys/alt_timestamp.h"



int seven_seg[] = {0x40, 0x79, 0x24, 0x30, 	0x19, 0x12, 0x02, 0x78, 0x0, 0x10, 0x08, 0x03, 0x46, 0x21, 0x06, 0x0e}; 



void interpolate_HW(int input_sample, int output_phase[4]) {
 
  int temp = ALT_CI_SINCX_0(1,input_sample);
  output_phase[0] = ALT_CI_SINCX_0(2,0);
  output_phase[1] = ALT_CI_SINCX_0(3,0);
  output_phase[2] = ALT_CI_SINCX_0(4,0);
  output_phase[3] = ALT_CI_SINCX_0(5,0);

 // printf("Output phase 0 is %d\n", output_phase[0]);
  //printf("Output phase 1 is %d\n", output_phase[1]);

}

int c0[] = {    0,  -2108, -3477, -2951};
int c1[] = {    0,   4916, 10430, 14750};
int c2[] = {16284,  14750, 10430,  4916};
int c3[] = {    0,  -2951, -3477, -2108};


int compute_phase(int phase, int t[4]) {
  int full, final;

  full  = t[3] * c0[phase];
  full += t[2] * c1[phase];
  full += t[1] * c2[phase];
  full += t[0] * c3[phase];

  final = full >> (21 - 7);

  return final;
}


void interpolate(int input_sample, int output_phase[4]) {

  static int tap[4];

  tap[3] = tap[2];
  tap[2] = tap[1];
  tap[1] = tap[0];
  tap[0] = input_sample;

  output_phase[0] = compute_phase(0, tap);
  output_phase[1] = compute_phase(1, tap);
  output_phase[2] = compute_phase(2, tap);
  output_phase[3] = compute_phase(3, tap);

}

int main() {

  int i,j;
  int s[500], o[4], o1[4];
  unsigned long long swc1, swc2, hwc1, hwc2;
  alt_timestamp_start();
  *(unsigned int *)(PIO_0_BASE) = 0x21;
 
  for (i=0; i<500; i++) 
  {
    
    s[i] = (rand() % 256) - 128;
	
    // s = i % 127 * (i & 1 ? -1 : 1);
  }
   printf("\nhello\n");
   
   
  
	
   
  for (i=0; i<500; i++) {
    // compute
    interpolate(s[i], o);
    printf("Ref SW  %4d | %4d %4d %4d %4d\n", s[i], o[0], o[1], o[2], o[3]);
   

    interpolate_HW(s[i], o1);
    printf("CI HW   %4d | %4d %4d %4d %4d\n", s[i], o[0], o[1], o[2], o[3]);
	
    }
/*	for(j=0;j<4;j++){
	printf("software%d: %d\n",i,o[j]);
	printf("hardware%d: %d\n",i,o1[j]);
   }   */
  //  if((o[0]!=o1[0]) | (o[1]!=o1[1]) | (o[2]!=o1[2]) | (o[3]!=o1[3]) )
   // { 
    //  printf("Error in program\n");
     // exit(0);
     //}
 // }

  printf("Successful\n");
 //  exit(0);
  swc1 = alt_timestamp();
  for (i=0; i<500; i++)
  {
    interpolate(s[i], o);
  }
  swc2 = alt_timestamp() ;
 

  hwc1 = alt_timestamp();
  for (i=0; i<500; i++)
  {
    interpolate_HW(s[i], o1);
  }
  hwc2 = alt_timestamp() ;
  
  printf("SW Cycles %lld\n", swc2-swc1);
  printf("HW Cycles %lld\n", hwc2-hwc1);
 
  
  return 0;
}
