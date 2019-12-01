#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "My_type.h"
#include "logging.h"

TxPacket myTxPacket;
LoraPtr myLoraPtr;

void print_data( char * x, u16 lenx)
{
  u8 y[64];
	u8 k[64];
	sprintf((char*)k, "%d:",HAL_GetTick());
	sprintf((char*)y, "%d:%s\n",HAL_GetTick(),x);
	u16 lenk = strlen((char*)k);
	u16 leng = lenk + lenx +1 ;
	CDC_Transmit_FS((uint8_t*)y,(uint16_t)leng);
}

u16 timer_measure_start(void)
{
	u16 time_start =0;
	time_start = HAL_GetTick();
	return time_start;
}

u16 timer_measure_stop(void)
{
	u16 time_stop =0;
	time_stop = HAL_GetTick();
	return time_stop;
}

void HAL_SYSTICK_Callback(void){
        SW_TIMER_ISR();
}