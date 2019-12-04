#include "My_type.h"
#include "mainApp.h"
#include "main.h"
#include "Master.h"
#include "Slave.h"
#include "SW_Timer.h"
#include "Lora_transmission_HAL.h"
#include "logging.h"
#include "string.h"
#include "stdio.h"
LoraMaster myLoraMaster;

void Send_Broadcast_Data(char* data){
  u8 tx_b[PACKET_LENGTH];
  u8 Tx_Packet_b[PACKET_LENGTH];
  HAL_Delay(TIME_BETWEEN_DATA_SENT);                            			
  sprintf((char*)tx_b,"%s\n", data);		
  sprintf((char*)Tx_Packet_b, "Data sent: %s\n\n",tx_b);														
  printUSB((char*)Tx_Packet_b);
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)tx_b, PACKET_LENGTH);																								
  Switch_To_Rx();																			
  myLoraMode.mode = MASTER_RX;	
  myLoraMode.uni_or_broad = BROADCAST;
  for(u8 i = 1; i < 100; i++){
        myLoraMaster.status[i] = 0;
  }
}

void Send_Unicast_Data(){
  u8 tx_u[PACKET_LENGTH];
  u8 Tx_Packet_u[PRINTUSB_LENGTH];
  myLoraMaster.uni_received = NOT_RECEIVED_YET;
  myLoraMaster.uni_sent = NOT_SENT_YET;
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 1){
    myLoraSlave.slave_id = "1";
    myLoraMode.uni_or_broad = UNICAST;
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1){
    myLoraSlave.slave_id = "2";
    myLoraMode.uni_or_broad = UNICAST;
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1){
    myLoraSlave.slave_id = "3";
    myLoraMode.uni_or_broad = UNICAST;
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
    myLoraSlave.slave_id = "4";
    myLoraMode.uni_or_broad = UNICAST;
  }				
  sprintf((char*)tx_u,"%s", myLoraSlave.slave_id);		
  sprintf((char*)Tx_Packet_u, "Data sent: Unicast to 1.%s\n\n", (char*)tx_u);														
  printUSB((char*)Tx_Packet_u);
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)tx_u, PACKET_LENGTH);																								
  Switch_To_Rx();																			
  myLoraMode.mode = MASTER_RX;
  myLoraMaster.sent = ALREADY_SENT;
}

void Master_Send_Data(){
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == 1){
    Send_Broadcast_Data("all");
  }
  else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == 1 
          || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1 || HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == 1){
    if(myLoraMaster.sent == NOT_SENT_YET){
      Send_Unicast_Data();
    }
  }      
}

void Start_Timer(SW_TIMER_CALLBACK function, u32 time){
  SW_TIMER_CREATE_FunCallBack(SW_TIMER1, time, function);
  SW_TIMER_START(SW_TIMER1);
}

void Receive_Data(){
  myLoraMaster.uni_sent = ALREADY_SENT;
  u8 store_packet[PACKET_LENGTH];
  u8 store_id[NUMBER_OF_SLAVES];                            
  for(uint32_t i = 0; i < NUMBER_OF_SLAVES - 1; i++){
          store_id[i] = 0;
  }
  strncpy((char*)store_id,(char*)(RxData + myLoraPtr.current_ptr),1);
  myLoraMaster.status[atoi((char*)store_id)] = 1;
  strcpy((char*)store_packet, (char*)(RxData + myLoraPtr.current_ptr));
  myLoraPtr.current_ptr++;
  if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
  myLoraMode.slave_count++;
  myLoraSlave.rssi_value = sx1276_7_8_LoRaReadRSSI();	
  sprintf(myLoraMode.strBuf,"Node 1.%s connected with RSSI: %d\n\n", (char*)store_id, myLoraSlave.rssi_value);				
  printUSB(myLoraMode.strBuf);	  
}

void Master_Receive_Data(){
  if(myLoraMode.flag_timer == 0){
    SW_TIMER_CALLBACK callback_function = fun1;
    myLoraMode.flag_timer = TIMER_SET;
    Start_Timer(callback_function, TIME_OUT);
  }
  if(Indicate_Rx_Packet("10", 0) == 1) //Receive a legal packet
  {		
    myLoraMode.flag_timer = TIMER_RESET;
    SW_TIMER_CLEAR(SW_TIMER1);
    
    Receive_Data();
                             
    if(myLoraMode.uni_or_broad == 1){
      if(myLoraMode.slave_count == 7){
        myLoraMode.mode = MASTER_TX;	
        myLoraMode.slave_count = 0;
      }
    }
    else{       
      SW_TIMER_CLEAR(SW_TIMER1);
      myLoraMode.mode = 7;
    }
  } 
}