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

LoraMode myLoraMode;
u8 mode;//lora--1/FSK--0

u8 irq_mainapp = 0;

void mainApp()
{
	u16 i=0;//,j,k=0,g;
	
	mode = 0x01;//lora mode
	
	sx1276_7_8_Reset();
		
	/*
        First initialization for Lora 
	*/
        
        for(uint32_t i = 1; i < 100; i++){
          myLoraMaster.status_prev[i] = 0;
        }
        
	u8 start = 0;
	u8 stop = 0;				
	Lora_1278_Init();
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
