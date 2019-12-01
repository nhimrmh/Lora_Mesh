/*
 * mainApp.c
 *
 *  Created on: Sep 8, 2017
 *      Author: dkhairnar
 */

#include "mainApp.h"
#include "Lora_transmission_HAL.h"
#include "string.h"
#include "SW_Timer.h"
#include "logging.h"
#include "Slave.h"
#include "Master.h"
#define printUSB(x) CDC_Transmit_FS((uint8_t*)x,strlen((char*)x))

u16 SysTime;
u16 time2_count;
u16 key1_time_count;
u16 key2_time_count;
u8 rf_rx_packet_length;

u8 mode;//lora--1/FSK--0
u8 Freq_Sel;//
u8 Power_Sel;//
u8 Lora_Rate_Sel;//
u8 BandWide_Sel;//
uint32_t count = 0; 
uint32_t number_of_ack = 0;
uint32_t number_of_noack = 0;
u8 Fsk_Rate_Sel;//
u8 count_idx = 1;
u8 flag_idx = 0;
s8 RSSI_Array[3][3];
u8 count_1 = 0;
u8 count_2 = 0;
u8 count_3 = 0;

u8 temp_count = 0;
LoraMode myLoraMode;
char* slave_id;
s8 rssi_value;
u8 irq_mainapp = 0;
u8 flag_timer = 0;
u8 flag_first_time = 0;

/*key1_count = 0----------->lora master
key1_count = 1----------->lora slaver
*/
u8 time_flag;
/*{
bit0 time_1s;
bit1 time_2s;
bit2 time_50ms;
bit3 ;
bit4 ;
bit5 ;
bit6 ;
bit7 ;
}*/
u8	operation_flag;
/*typedef struct
{
	uchar	:RxPacketReceived-0;
	uchar	:
	uchar	:
	uchar	:
	uchar	:
	uchar	:key2_down;
	uchar	:key1_down;
	uchar	;
} operation_flag;*/
u8 key_flag;
/*{
	uchar	:key1_shot_down;
	uchar	:key1_long_down;
	uchar	:key2_short_down;
	uchar	:key2_long_down
	uchar	:
	uchar	:;
	uchar	:;
	uchar	;
}*/
uint32_t tick_temp;
void mainApp()
{
	u16 i=0;//,j,k=0,g;

	SysTime = 0;
	operation_flag = 0x00;	
	mode = 0x01;//lora mode
	Freq_Sel = 0x00;//433M
	Power_Sel = 0x00;//
	Lora_Rate_Sel = 0x00;// Spreading Factor config, 0x00 = 6
	BandWide_Sel = 0x09;
	Fsk_Rate_Sel = 0x00;
	
	//RED_LED_L();
	//HAL_Delay(500);
	//RED_LED_H();

	sx1276_7_8_Reset();
	
	/*
	Init RSSI table with all values are 0
	*/
	for(uint32_t i = 0; i < 3; i++){
		for(uint32_t j = 0; j < 3; j++){
			RSSI_Array[i][j] = 0;
		}
	}
	
	/*
        First initialization for Lora 
	*/
        
        for(uint32_t i = 1; i < 100; i++){
          myLoraMaster.status_prev[i] = 0;
        }
        
	u8 start = 0;
	u8 stop = 0;				
	
	sx1276_7_8_Config_Init();                                         //setting base parameter

	Switch_To_Rx();	
	
        /*
	Choose mode
	*/ 
        slave_id = "2"; //Id of slave
	myLoraMode.mode = 3; //Mode 1: Slave, 3:Master  
        myLoraMode.uni_or_broad = 0;
	myLoraMode.slave_count = 0;
	while (1)
	{		
		switch(myLoraMode.mode)
		{
			case 0://lora master Tx
                        {	
                          Slave_Send_Response(myLoraMode.uni_or_broad, slave_id);
                        }		
			break;
				
			case 1://lora slaver Rx continuous
			{									
                          Slave_Receive_Data(slave_id);
                        }		
			break;	
                        
                        case 3://lora master Tx
                        {	
                          Master_Send_Data(slave_id);                    
                        }		
			break;
				
			case 4://lora slaver Rx continuous
			{	                         
                          
                          if(flag_timer == 0){
                            flag_timer = 1;
                            SW_TIMER_CALLBACK temp = fun1;	
                            SW_TIMER_CREATE_FunCallBack(SW_TIMER1, 300000, temp);
                            SW_TIMER_START(SW_TIMER1);
                          }
                          if(Indicate_Rx_Packet("10", 0) == 1) //Receive a legal packet
                          {		
                            flag_timer = 0;
                            SW_TIMER_CLEAR(SW_TIMER1);
                            myLoraMaster.uni_sent = 1;
                            u8 store_packet[20];
                            u8 store_id[8];                            
                            for(uint32_t i = 0; i < 7; i++){
                                    store_id[i] = 0;
                            }
                            strncpy((char*)store_id,(char*)(RxData + myLoraPtr.current_ptr),1);
                            myLoraMaster.status[atoi((char*)store_id)] = 1;
                            strcpy((char*)store_packet, (char*)(RxData + myLoraPtr.current_ptr));
                            myLoraPtr.current_ptr++;
                            if(myLoraPtr.current_ptr == 3) myLoraPtr.current_ptr = 0;
                            myLoraMode.slave_count++;
                            rssi_value = sx1276_7_8_LoRaReadRSSI();	
                            sprintf(myLoraMode.strBuf,"Node 1.%s connected with RSSI: %d\n", (char*)store_id, rssi_value);				
                            printUSB(myLoraMode.strBuf);	                                                            
                                                     
                            if(myLoraMode.uni_or_broad == 1){
                              if(myLoraMode.slave_count == 7){
                                myLoraMode.mode = 3;	
                                myLoraMode.slave_count = 0;
                              }
                            }
                            else{       
                              SW_TIMER_CLEAR(SW_TIMER1);
                              myLoraMode.mode = 7;
                            }
                          }                                                       
                        }		
			break;	
                        
                        case 5:
                        {
                          for(uint32_t i = 1; i < 100; i++){
                            if(myLoraMaster.status_prev[i] == 1 && myLoraMaster.status[i] == 0){
                              sprintf(myLoraMode.strBuf,"Node 1.%d disconnected\n", i);				
                              printUSB(myLoraMode.strBuf);	                                                                                          
                            }
                          }
                          for(uint32_t i = 1; i < 100; i++){
                            myLoraMaster.status_prev[i] = myLoraMaster.status[i];
                          }                                                    
                          myLoraMode.mode = 3; 
                        }
                        break;
                        
                        case 6:
                        {
                          if(myLoraMaster.uni_received == 0){
                            myLoraMaster.uni_received = 1;
                            sprintf(myLoraMode.strBuf,"Node 1.%s disconnected\n", slave_id);				
                            printUSB(myLoraMode.strBuf);	
                            myLoraMode.mode = 7;
                          }
                        }
                        break;
                        
                        case 7:     
                          flag_timer = 0;
                          if(atoi(slave_id) == 1){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(atoi(slave_id) == 2){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(atoi(slave_id) == 3){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(atoi(slave_id) == 4){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(myLoraMaster.sent == 0){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1 || 
                               HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1
                               || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
                              myLoraMode.mode = 3;
                            }
                          }
                        break;
	}
      }
}
