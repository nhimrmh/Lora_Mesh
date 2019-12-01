#include "My_type.h"
#include "mainApp.h"
#include "main.h"
#include "Master.h"
#define printUSB(x) CDC_Transmit_FS((uint8_t*)x,strlen((char*)x))
LoraMaster myLoraMaster;

void Master_Send_Data(char* slave_id){
  u8 tx_b[20];
  u8 tx_u[20];
  u8 Tx_Packet_b[20];
  u8 Tx_Packet_u[20];
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1){
    HAL_Delay(100000 * 0.5);                            			
    sprintf((char*)tx_b,"all\n");		
    sprintf((char*)Tx_Packet_b, "Data sent: Broadcast\n");														
    printUSB((char*)Tx_Packet_b);
    Switch_To_Tx();																											
    Send_Tx_Packet((u8*)tx_b, 20);																								
    Switch_To_Rx();																			
    myLoraMode.mode = 4;	
    myLoraMode.uni_or_broad = 1;
    for(u8 i = 1; i < 100; i++){
          myLoraMaster.status[i] = 0;
    }
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1 
          || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
    if(myLoraMaster.sent == 0){
      myLoraMaster.uni_received = 0;
      myLoraMaster.uni_sent = 0;
      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 1){
        slave_id = "1";
        myLoraMode.uni_or_broad = 0;
      }
      else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1){
        slave_id = "2";
        myLoraMode.uni_or_broad = 0;
      }
      else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1){
        slave_id = "3";
        myLoraMode.uni_or_broad = 0;
      }
      else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
        slave_id = "4";
        myLoraMode.uni_or_broad = 0;
      }
      u8 Tx_Packet[20];					
      sprintf((char*)tx_u,"%s\n", slave_id);		
      sprintf((char*)Tx_Packet_u, "Data sent: Unicast to 1.%s", (char*)tx_u);														
      printUSB((char*)Tx_Packet_u);
      Switch_To_Tx();																											
      Send_Tx_Packet((u8*)tx_u, 20);																								
      Switch_To_Rx();																			
      myLoraMode.mode = 4;
      myLoraMaster.sent = 1;
    }
  }      
}