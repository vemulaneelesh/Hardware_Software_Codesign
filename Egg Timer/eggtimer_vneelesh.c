#include <stdio.h>
#include <unistd.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "sys/times.h"
#include "sys/alt_alarm.h"
#include "alt_types.h"

static int count = 0;
static int volatile state = 1;

static alt_alarm my_alarm;
int RTC_Time = 0;

static void regular_display()
{
	int min10 = count/600;
	int min = (count%600)/60;
	int sec10 = ((count%600)%60)/10;
	int sec = (((count%600)%60)%10);

	int segment[10] = {0x40,0xF9,0x24,0x30,0x19,0x12,0x02,0xF8,0x00,0x18};
	*(unsigned int *)(PIO_7_BASE) = 0xAF;
	*(unsigned int *)(PIO_6_BASE) = 0xE3;
	*(unsigned int *)(PIO_5_BASE) = 0xAB;
	*(unsigned int *)(PIO_4_BASE) = 0xFF;
	*(unsigned int *)(PIO_3_BASE) = segment[min10];
	*(unsigned int *)(PIO_2_BASE) = segment[min];
	*(unsigned int *)(PIO_1_BASE) = segment[sec10];
	*(unsigned int *)(PIO_0_BASE) = segment[sec];
}

static void regular_display_no_run()
{
	int min10 = count/600;
	int min = (count%600)/60;
	int sec10 = ((count%600)%60)/10;
	int sec = (((count%600)%60)%10);

	int segment[10] = {0x40,0xF9,0x24,0x30,0x19,0x12,0x02,0xF8,0x00,0x18};
	*(unsigned int *)(PIO_3_BASE) = segment[min10];
	*(unsigned int *)(PIO_2_BASE) = segment[min];
	*(unsigned int *)(PIO_1_BASE) = segment[sec10];
	*(unsigned int *)(PIO_0_BASE) = segment[sec];	
}

static void increment_minutes()
{
	count = count + 60;
	regular_display();
	if(count > 6039)
	{
		count = 0;
	}	
	
}

static void increment_seconds()
{
	count = count + 1;
	regular_display();
	
}

static void display_end()
{
	*(unsigned int *)(PIO_7_BASE) = 0x06;
	*(unsigned int *)(PIO_6_BASE) = 0xAB;
	*(unsigned int *)(PIO_5_BASE) = 0x21;
	*(unsigned int *)(PIO_4_BASE) = 0xFF;
}

static void display_set()
{
	*(unsigned int *)(PIO_7_BASE) = 0x12;
	*(unsigned int *)(PIO_6_BASE) = 0x06;
	*(unsigned int *)(PIO_5_BASE) = 0x07;
	*(unsigned int *)(PIO_4_BASE) = 0xFF;
	regular_display_no_run();

}


static void running()
{
	if(count >= 0 )
	{
	  regular_display();
//	  usleep(1000000);
	  count --;
	}
	else
	{
	  	state = 3;
	}
}


alt_u32 my_alarm_callback (void* context)
{
	/* This function is called once per second */
	running();
	return alt_ticks_per_second();
}

static void setting()
{
	int in;
	in = IORD_ALTERA_AVALON_PIO_DATA(PIO_8_BASE);
	usleep(150000);
	switch(in)
	{
		case 0x3:
		state = 2;
		usleep(9000);
		alt_alarm_start (&my_alarm,alt_ticks_per_second(),my_alarm_callback,NULL);
		break;
		case 0x5:
		increment_minutes();
		break;
		case 0x6:
		increment_seconds();
		break;
		default:
		break;
	}

}


int main()
{
  while (1) 
  {
	  switch(state)
	  {
	  case 1: 
	  		display_set();

	  		setting();
	  		break;
	  case 2:
	  		break;
	  case 3: 
	  		  display_end();
	  		  int in = IORD_ALTERA_AVALON_PIO_DATA(PIO_8_BASE);
	  		  usleep(100000);
	  		  if (in == 3)
	  		  {
	  		  	state = 1;
	  		  	count = 0;
	  		  	alt_alarm_stop(&my_alarm);
	  		  	display_set();

	  		  }
	  		  break;
	  }		  
  }
}