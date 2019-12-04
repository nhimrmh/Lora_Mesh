#include "Lora_transmission_HAL.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "Slave.h"
#include "logging.h"
#include "mainApp.h"
#include "Master.h"

LoraSlave myLoraSlave;

/**********************************************************
**Name:     Delay_By_Id
**Function: Delay_By_Id
**Input:    slave id to delay
**Output:   none
**********************************************************/
void Delay_By_Id(char* slave_id){
  HAL_Delay(BROADCAST_DELAY_TIME * atoi(slave_id));
}

/**********************************************************
**Name:     Slave_Send_Response_Broadcast
**Function: Slave_Send_Response_Broadcast
**Input:    slave id to send data
**Output:   none
**********************************************************/
void Slave_Send_Response_Broadcast(char* slave_id){
  Delay_By_Id(slave_id);                            			
  sprintf((char*)sx1276_7_8Data,"%s_%d\n", slave_id, myLoraSlave.rssi_value);		
  sprintf((char*)(myTxPacket.Data), "Data sent: %s", (char*)sx1276_7_8Data);														
  printUSB((char*)(myTxPacket.Data));
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)sx1276_7_8Data, PACKET_LENGTH);																								
  Switch_To_Rx();																			
  myLoraMode.mode = SLAVE_RX;	
}

/**********************************************************
**Name:     Slave_Send_Response_Unicast
**Function: Slave_Send_Response_Unicast
**Input:    slave id to send data
**Output:   none
**********************************************************/
void Slave_Send_Response_Unicast(char* slave_id){  			
  sprintf((char*)sx1276_7_8Data,"%s_%d\n", slave_id, myLoraSlave.rssi_value);		
  sprintf((char*)(myTxPacket.Data), "Data sent: %s", (char*)sx1276_7_8Data);														
  printUSB((char*)(myTxPacket.Data));
  Switch_To_Tx();																											
  Send_Tx_Packet((u8*)sx1276_7_8Data, PACKET_LENGTH);																								
  Switch_To_Rx();																			
  myLoraMode.mode = SLAVE_RX;	
}

/**********************************************************
**Name:     Slave_Send_Response
**Function: Slave_Send_Response
**Input:    mode unicast or broadcast and slave id if unicast
**Output:   none
**********************************************************/
void Slave_Send_Response(u8 uni_or_broad, char* slave_id){
  if(uni_or_broad == 1){
    Slave_Send_Response_Broadcast(slave_id);
  }
  else{
    Slave_Send_Response_Unicast(slave_id);
  }
}

/**********************************************************
**Name:     Slave_Receive_Unicast
**Function: Slave_Receive_Unicast
**Input:    none
**Output:   none
**********************************************************/
void Slave_Receive_Unicast(){
  myLoraSlave.rssi_value = sx1276_7_8_LoRaReadRSSI();						
  sprintf(myLoraMode.strBuf,"Data received: %s\n",(char*)(RxData + myLoraPtr.current_ptr));		
  myLoraPtr.current_ptr++;
  if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
  printUSB(myLoraMode.strBuf);										
  myLoraMode.mode = SLAVE_TX;                                                        
  myLoraMode.uni_or_broad = UNICAST;                                
}

/**********************************************************
**Name:     Slave_Receive_Broadcast
**Function: Slave_Receive_Broadcast
**Input:    none
**Output:   none
**********************************************************/
void Slave_Receive_Broadcast(){
  myLoraSlave.rssi_value = sx1276_7_8_LoRaReadRSSI();						
  sprintf(myLoraMode.strBuf,"Data received: %s\n",(char*)(RxData + myLoraPtr.current_ptr));		
  myLoraPtr.current_ptr++;
  if(myLoraPtr.current_ptr == LAST_POSITION_OF_QUEUE) myLoraPtr.current_ptr = FIRST_POSITION_OF_QUEUE;
  printUSB(myLoraMode.strBuf);										
  myLoraMode.mode = SLAVE_TX;             
  myLoraMode.uni_or_broad = BROADCAST;                                
}

/**********************************************************
**Name:     Slave_Receive_Data
**Function: Slave_Receive_Data
**Input:    slave id to receive data if in unicast mode
**Output:   none
**********************************************************/
void Slave_Receive_Data(char* slave_id){
  if(Indicate_Rx_Packet(slave_id, SLAVE_IS_RECEIVING) == 1) //Receive a legal packet
  {		
    if(strncmp((char*)(RxData + myLoraPtr.current_ptr),slave_id,1) == 0){																
        Slave_Receive_Unicast();
    }
    else if(strncmp((char*)(RxData + myLoraPtr.current_ptr),"a",1) == 0){																
        Slave_Receive_Broadcast();
    }
  }
}