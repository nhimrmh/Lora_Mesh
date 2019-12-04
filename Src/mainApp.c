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
#include "stdio.h"
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
s8 rssi_value;
u8 irq_mainapp = 0;
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
        myLoraSlave.slave_id = "2"; //Id of slave
	myLoraMode.mode = MASTER_TX; //Mode 1: Slave, 3:Master  
        myLoraMode.uni_or_broad = UNICAST;
	myLoraMode.slave_count = RESET_VALUE;
        myLoraPtr.current_ptr = RESET_VALUE;
        myLoraMode.flag_timer = TIMER_RESET;
        myLoraMaster.sent = NOT_SENT_YET;
	while (1)
	{		
		switch(myLoraMode.mode)
		{
			case SLAVE_TX://lora slave Tx
                        {	
                          Slave_Send_Response(myLoraMode.uni_or_broad, myLoraSlave.slave_id);
                        }		
			break;
				
			case SLAVE_RX://lora slave Rx continuous
			{									
                          Slave_Receive_Data(myLoraSlave.slave_id);
                        }		
			break;	
                        
                        case MASTER_TX://lora master Tx
                        {	
                          Master_Send_Data();                    
                        }		
			break;
				
			case MASTER_RX://lora master Rx continuous
			{	                                                   
                          Master_Receive_Data();                                                      
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
                          myLoraMode.mode = MASTER_TX; 
                        }
                        break;
                        
                        case 6:
                        {
                          if(myLoraMaster.uni_received == 0){
                            myLoraMaster.uni_received = 1;
                            sprintf(myLoraMode.strBuf,"Node 1.%s disconnected\n", myLoraSlave.slave_id);				
                            printUSB(myLoraMode.strBuf);	
                            myLoraMode.mode = 7;
                          }
                        }
                        break;
                        
                        case 7:     
                          myLoraMode.flag_timer = 0;
                          if(atoi(myLoraSlave.slave_id) == 1){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(atoi(myLoraSlave.slave_id) == 2){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(atoi(myLoraSlave.slave_id) == 3){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 0){
                                myLoraMaster.sent = 0;
                            }
                          }
                          if(atoi(myLoraSlave.slave_id) == 4){
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
