#include "Lora_transmission_HAL.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "Slave.h"
#include "logging.h"
#include "mainApp.h"
#define printUSB(x) CDC_Transmit_FS((uint8_t*)x,strlen((char*)x))
extern s8 rssi_value;

void Delay_By_Id(char* slave_id){
  HAL_Delay(BROADCAST_DELAY_TIME * atoi(slave_id));
}

void Slave_Send_Response_Broadcast(char* slave_id){
  Delay_By_Id(slave_id);                            			
  sprintf((char*)sx1276_7_8Data,"%s_%d\n", slave_id, rssi_value);		
  sprintf((char*)(myTxPacket.Data), "Data sent: %s", (char*)sx1276_7_8Data);														
  printUSB((char*)(myTxPacket.Data));
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)sx1276_7_8Data, 20);																								
  Switch_To_Rx();																			
  myLoraMode.mode = 1;	
}

void Slave_Send_Response_Unicast(char* slave_id){  			
  sprintf((char*)sx1276_7_8Data,"%s_%d\n", slave_id, rssi_value);		
  sprintf((char*)(myTxPacket.Data), "Data sent: %s", (char*)sx1276_7_8Data);														
  printUSB((char*)(myTxPacket.Data));
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)sx1276_7_8Data, 20);																								
  Switch_To_Rx();																			
  myLoraMode.mode = 1;	
}

void Slave_Send_Response(u8 uni_or_broad, char* slave_id){
  if(uni_or_broad == 1){
    Slave_Send_Response_Broadcast(slave_id);
  }
  else{
    Slave_Send_Response_Unicast(slave_id);
  }
}

void Slave_Receive_Unicast(){
  rssi_value = sx1276_7_8_LoRaReadRSSI();						
  sprintf(myLoraMode.strBuf,"Data received: %s\n",(char*)(RxData + myLoraPtr.current_ptr));		
  myLoraPtr.current_ptr++;
  if(myLoraPtr.current_ptr == MAX_QUEUE_LENGTH) myLoraPtr.current_ptr = 0;
  printUSB(myLoraMode.strBuf);										
  myLoraMode.mode = 0;                                                        
  myLoraMode.uni_or_broad = 0;                                
}

void Slave_Receive_Broadcast(){
  rssi_value = sx1276_7_8_LoRaReadRSSI();						
  sprintf(myLoraMode.strBuf,"Data received: %s\n",(char*)(RxData + myLoraPtr.current_ptr));		
  myLoraPtr.current_ptr++;
  if(myLoraPtr.current_ptr == MAX_QUEUE_LENGTH) myLoraPtr.current_ptr = 0;
  printUSB(myLoraMode.strBuf);										
  myLoraMode.mode = 0;             
  myLoraMode.uni_or_broad = 1;                                
}

void Slave_Receive_Data(char* slave_id){
  if(Indicate_Rx_Packet(slave_id, SLAVE_IS_RECEIVING) == 1) //Receive a legal packet
  {		
    if(strncmp((char*)(RxData + myLoraPtr.current_ptr),slave_id,1) == 0){																
        Slave_Receive_Unicast();
    }
    else if(strncmp((char*)(RxData + myLoraPtr.current_ptr),BROADCAST_CHARACTER,1) == 0){																
        Slave_Receive_Broadcast();
    }
  }
}