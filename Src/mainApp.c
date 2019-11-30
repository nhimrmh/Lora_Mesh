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

#define printUSB(x) CDC_Transmit_FS((uint8_t*)x,strlen((char*)x))

char strBuf[128];
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
u8 current_ptr = 0;
u8 temp_count = 0;
u8 key1_count;
char* slave_id;
s8 rssi_value;
u8 uni_or_broad = 0;
u8 slave_count;
u8 irq_mainapp = 0;
u8 flag_timer = 0;
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
	key1_count = 0x00;
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
	u8 start = 0;
	u8 stop = 0;				
	
	sx1276_7_8_Config_Init();                                         //setting base parameter

	Switch_To_Rx();	
	
        /*
	Choose mode
	*/ 
        slave_id = "3";
	key1_count = 3;
	slave_count = 0;
	while (1)
	{		
		switch(key1_count)
		{
			case 0://lora master Tx
                        {	
                          if(uni_or_broad == 1){
                            HAL_Delay(100000 * atoi(slave_id));
                            u8 Tx_Packet[20];					
                            sprintf((char*)sx1276_7_8Data,"%s_%d\n", slave_id, rssi_value);		
                            sprintf((char*)Tx_Packet, "Data sent: %s", (char*)sx1276_7_8Data);														
                            printUSB((char*)Tx_Packet);
                            Switch_To_Tx();																											
                            Send_Tx_Packet((u8*)sx1276_7_8Data, 20);																								
                            Switch_To_Rx();																			
                            key1_count = 1;	
                          }
                          else{
                            u8 Tx_Packet[20];					
                            sprintf((char*)sx1276_7_8Data,"%s_%d\n", slave_id, rssi_value);		
                            sprintf((char*)Tx_Packet, "Data sent: %s", (char*)sx1276_7_8Data);														
                            printUSB((char*)Tx_Packet);
                            Switch_To_Tx();																											
                            Send_Tx_Packet((u8*)sx1276_7_8Data, 20);																								
                            Switch_To_Rx();																			
                            key1_count = 1;	
                          }
                        }		
			break;
				
			case 1://lora slaver Rx continuous
			{									
                          if(Indicate_Rx_Packet(slave_id, 1) == 1) //Receive a legal packet
                          {		
                            if(strncmp((char*)(RxData + current_ptr),slave_id,1) == 0){																
                                rssi_value = sx1276_7_8_LoRaReadRSSI();						
                                sprintf(strBuf,"Data received: %s\n",(char*)(RxData + current_ptr));		
                                current_ptr++;
                                if(current_ptr == 3) current_ptr = 0;
                                printUSB(strBuf);										
                                key1_count = 0;                                                        
                                uni_or_broad = 0;
                            }
                            if(strncmp((char*)(RxData + current_ptr),"a",1) == 0){																
                                rssi_value = sx1276_7_8_LoRaReadRSSI();						
                                sprintf(strBuf,"Data received: %s\n",(char*)(RxData + current_ptr));		
                                current_ptr++;
                                if(current_ptr == 3) current_ptr = 0;
                                printUSB(strBuf);										
                                key1_count = 0;             
                                uni_or_broad = 1;
                            }
                          }                                          
                        }		
			break;	
                        
                        case 3://lora master Tx
                        {	                          
                          if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 1){
                            HAL_Delay(100000 * 0.5);
                            u8 Tx_Packet[20];					
                            sprintf((char*)sx1276_7_8Data,"all\n");		
                            sprintf((char*)Tx_Packet, "Data sent: %s", (char*)sx1276_7_8Data);														
                            printUSB((char*)Tx_Packet);
                            Switch_To_Tx();																											
                            Send_Tx_Packet((u8*)sx1276_7_8Data, 20);																								
                            Switch_To_Rx();																			
                            key1_count = 4;	
                            uni_or_broad = 1;
                          }
                          if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1){
                              slave_id = "1";
                              uni_or_broad = 0;
                            }
                            else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1){
                              slave_id = "2";
                              uni_or_broad = 0;
                            }
                            if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
                              slave_id = "3";
                              uni_or_broad = 0;
                            }
                            u8 Tx_Packet[20];					
                            sprintf((char*)sx1276_7_8Data,"%s_1\n", slave_id);		
                            sprintf((char*)Tx_Packet, "Data sent: %s", (char*)sx1276_7_8Data);														
                            printUSB((char*)Tx_Packet);
                            Switch_To_Tx();																											
                            Send_Tx_Packet((u8*)sx1276_7_8Data, 20);																								
                            Switch_To_Rx();																			
                            key1_count = 4;	
                          }                                                    
                        }		
			break;
				
			case 4://lora slaver Rx continuous
			{	
                          if(flag_timer == 0){
                            flag_timer = 1;
                            SW_TIMER_CALLBACK temp = fun1;	
                            SW_TIMER_CREATE_FunCallBack(SW_TIMER1, 500000, temp);
                            SW_TIMER_START(SW_TIMER1);
                          }
                          if(Indicate_Rx_Packet("10", 0) == 1) //Receive a legal packet
                          {		
                            flag_timer = 0;
                            
                            if(strncmp((char*)(RxData + current_ptr),"1",1) == 0){
                                current_ptr++;
                                if(current_ptr == 3) current_ptr = 0;
                                slave_count++;
                                rssi_value = sx1276_7_8_LoRaReadRSSI();	
                                RSSI_Array[0][count_1] = rssi_value;                                                                
                                count_1++;
                                if(count_1 == 3) count_1 = 0;
                                sprintf(strBuf,"\n ID  1   2   3 \n    ___________\n\n 1 |%3d %3d %3d|\n 2 |%3d %3d %3d|\n 3 |%3d %3d %3d|\n    ___________\n",
                                RSSI_Array[0][0],RSSI_Array[0][1],RSSI_Array[0][2], RSSI_Array[1][0],RSSI_Array[1][1],RSSI_Array[1][2]
                                ,RSSI_Array[2][0],RSSI_Array[2][1],RSSI_Array[2][2]);				
                                printUSB(strBuf);	                                
                            }
                            else if(strncmp((char*)(RxData + current_ptr),"2",1) == 0){	
                                current_ptr++;
                                if(current_ptr == 3) current_ptr = 0;
                                slave_count++;
                                rssi_value = sx1276_7_8_LoRaReadRSSI();	
                                RSSI_Array[1][count_2] = rssi_value;                                                                
                                count_2++;
                                if(count_2 == 3) count_2 = 0;
                                sprintf(strBuf,"\n ID  1   2   3 \n    ___________\n\n 1 |%3d %3d %3d|\n 2 |%3d %3d %3d|\n 3 |%3d %3d %3d|\n    ___________\n",
                                RSSI_Array[0][0],RSSI_Array[0][1],RSSI_Array[0][2], RSSI_Array[1][0],RSSI_Array[1][1],RSSI_Array[1][2]
                                ,RSSI_Array[2][0],RSSI_Array[2][1],RSSI_Array[2][2]);				
                                printUSB(strBuf);	                                
                            }
                            else if(strncmp((char*)(RxData + current_ptr),"3",1) == 0){	
                                current_ptr++;
                                if(current_ptr == 3) current_ptr = 0;
                                slave_count++;
                                rssi_value = sx1276_7_8_LoRaReadRSSI();	
                                RSSI_Array[2][count_3] = rssi_value;                                
                                count_3++;
                                if(count_3 == 3) count_3 = 0;
                                sprintf(strBuf,"\n ID  1   2   3 \n    ___________\n\n 1 |%3d %3d %3d|\n 2 |%3d %3d %3d|\n 3 |%3d %3d %3d|\n    ___________\n",
                                RSSI_Array[0][0],RSSI_Array[0][1],RSSI_Array[0][2], RSSI_Array[1][0],RSSI_Array[1][1],RSSI_Array[1][2]
                                ,RSSI_Array[2][0],RSSI_Array[2][1],RSSI_Array[2][2]);				
                                printUSB(strBuf);	                                
                            }                            
                            if(uni_or_broad == 1){
                              if(slave_count == 2){
                                key1_count = 3;	
                                slave_count = 0;
                              }
                            }
                            else{                              
                              key1_count = 3;	                              
                            }
                          }                                                       
                        }		
			break;	
	}
      }
}
