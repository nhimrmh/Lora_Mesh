#ifndef _LOGGING_H
#define _LOGGING_H

#include "My_type.h"

extern void print_data( char * x, u16 lenx);
extern u16 timer_measure_start(void);
extern u16 timer_measure_stop(void);

typedef struct
{
  u8 Data[20];
}TxPacket;

typedef struct
{
  u8 current_ptr;
}LoraPtr;

extern TxPacket myTxPacket;
extern LoraPtr myLoraPtr;
#endif


