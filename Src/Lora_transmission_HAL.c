#include "Lora_transmission_HAL.h"
#include "stdio.h"
#include "stdlib.h"
#include "logging.h"
#include "string.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "My_type.h"
#define printUSB(x) CDC_Transmit_FS((uint8_t*)x,strlen((char*)x))
extern uint32_t tick_temp;
Lora_HandleTypeDef Lora;

u8  sx1276_7_8Data[20] = {"Lora sx1276_7_8"};
//const u8  sx1276_7_8Data[] = {"Mark1 Lora sx1276_7_8"};

u8 RxData[3][20];
u8 free_ptr = 0;
u8 RxData1[64];
u8 temp;
u8 result;

/**********************************************************
**Variable define
**********************************************************/

void sx1276_7_8_Config_Init(void);

void sx1276_7_8_Reset(void){
	HAL_GPIO_WritePin(Reset_GPIO_Port,Reset_Pin,GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(Reset_GPIO_Port,Reset_Pin,GPIO_PIN_SET);
}

/**********************************************************
**Name:     sx1276_7_8_Standby
**Function: Entry standby mode
**Input:    None
**Output:   None
**********************************************************/
void Switch_To_Standby(){	
	while(HAL_GPIO_ReadPin(DIO5_GPIO_Port,DIO5_Pin) == 0){		
		SPIWrite(Lora_1278_RegOpMode,LORA_STDBY);                              		//Standby//Low Frequency Mode		
	}	
}

/**********************************************************
**Name:     sx1276_7_8_Sleep
**Function: Entry sleep mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_Sleep(void)
{
  SPIWrite(Lora_1278_RegOpMode,LORA_SLEEP);                              		//Sleep//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x00);                            		 //Sleep//High Frequency Mode
}

/*********************************************************/
//LoRa mode
/*********************************************************/
/**********************************************************
**Name:     sx1276_7_8_EntryLoRa
**Function: Set RFM69 entry LoRa(LongRange) mode
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_EntryLoRa(void)
{
  SPIWrite(Lora_1278_RegOpMode,LORA_ON);//Low Frequency Mode
	//SPIWrite(LR_RegOpMode,0x80);//High Frequency Mode
}

/**********************************************************
**Name:     sx1276_7_8_LoRaClearIrq
**Function: Clear all irq
**Input:    None
**Output:   None
**********************************************************/
void sx1276_7_8_LoRaClearIrq(void)
{
  SPIWrite(Lora_1278_RegIrqFlags,IRQFLAG_CLEAR);
}

/**********************************************************
**Name:     sx1276_7_8_LoRaEntryRx
**Function: Entry Rx mode
**Input:    None
**Output:   TRUE - success; FALSE - fail
**********************************************************/
bool Switch_To_Rx(void)
{
  bool switch_success = FALSE;
  u8 addr;  
	
  SPIWrite(Lora_1278_RegPaDac,PADAC_NORMAL);                              //Normal and Rx
  SPIWrite(Lora_1278_RegHopPeriod,0xFF);                          //RegHopPeriod NO FHSS
  SPIWrite(Lora_1278_RegDioMapping1,DIOMAPPING1_RX);                       //DIO0=00, DIO1=00, DIO2=00, DIO3=01

  SPIWrite(Lora_1278_RegIrqFlagsMask,0x3F);                                     
  sx1276_7_8_LoRaClearIrq();
 
  SPIWrite(Lora_1278_RegPayloadLength,NUM_PAYLOAD);                       //RegPayloadLength  20byte(this register must difine when the data long of one byte in SF is 6)

  addr = SPIRead((Lora_1278_RegFifoRxBaseAddr));           				//Read RxBaseAddr
  SPIWrite(Lora_1278_RegFifoAddrPtr,addr);                        //RxBaseAddr -> FiFoAddrPtr¡¡
  SPIWrite(Lora_1278_RegOpMode,LORA_RXCONTINUOUS);                        		//Continuous Rx Mode//Low Frequency Mode	
	while(1)
	{
          if((SPIRead(Lora_1278_RegModemStat)&0x04)==0x04){   //Rx-on going RegModemStat
            switch_success = TRUE;
            break;		                         
          }
	}
	return switch_success;
}

/**********************************************************
**Name:     sx1276_7_8_LoRaReadRSSI
**Function: Read the RSSI value
**Input:    none
**Output:   temp, RSSI value
**********************************************************/
s8 sx1276_7_8_LoRaReadRSSI(void)
{
  s8 temp=10;
	s8 snr = 0;
  temp=SPIRead(Lora_1278_RegPktRssiValue);                  //Read RegRssiValue£¬Rssi value
	snr =SPIRead(Lora_1278_RegPktSnrValue);
	if(snr > 0){
		temp=temp*16/15 - 164;
	}
	else if(snr < 0){
		temp=temp - 164 + snr*0.25;
	}
	else temp=temp-164;                                        
  return temp;
}

/**********************************************************
**Name:     Indicate_Rx_Packet
**Function: Receive a incoming Rx packet
**Input:    None
**Output:   TRUE - success; FALSE - fail
**********************************************************/
bool Indicate_Rx_Packet(char* slave_id, u8 m_or_s)
{
  bool receive_success = FALSE;
  u8 i, result;
  if(HAL_GPIO_ReadPin(nIrq_GPIO_Port,nIrq_Pin) == 1)
  {	
    bool switch_success = FALSE;
    for(i=0;i<19;i++){     
            RxData[free_ptr][i] = 0;
    }
    result = Read_Rx_Packet((char*)RxData, 20, slave_id, m_or_s);
    sx1276_7_8_LoRaClearIrq();		
    receive_success = TRUE;
  }  
  return receive_success;
}

/**********************************************************
**Name:     Read_Rx_Packet
**Function: Receive a incoming Rx packet
**Input:    RxData, Design lenth
**Output:   TRUE - success; FAIL - fail
**********************************************************/
bool Read_Rx_Packet(char* Rx_Packet, u8 length, char* slave_id, u8 m_or_s){
    bool receive_success = FALSE;
    u8 addr;
    u8 packet_size;
    addr = SPIRead(Lora_1278_RegFifoRxCurrentAddr);      //last packet addr
    SPIWrite(Lora_1278_RegFifoAddrPtr,addr);                      //RxBaseAddr -> FiFoAddrPtr
    if(Lora.SpreadFactor==SF_6){           //When SpreadFactor is six£¬will used Implicit Header mode(Excluding internal packet length)
      packet_size=20;
    }
    else{		
      packet_size = SPIRead(Lora_1278_RegRxNbBytes);     //Number for received bytes
    }
    if(packet_size <= length){
      u8 temp[20];
      for(u8 i=0;i<19;i++){     
            temp[i] = 0;
      }
      SPIBurstRead(0x00, (char*)temp, packet_size);	
      if(strncmp((char*)temp,slave_id,1) == 0 || m_or_s == 0){
            strcpy(*(RxData + free_ptr),(char*)temp);	
            free_ptr++;
            if(free_ptr == 3) free_ptr = 0;
            receive_success = TRUE;
      }
      if(strncmp((char*)temp,"a",1) == 0 || m_or_s == 0){
            strcpy(*(RxData + free_ptr),(char*)temp);	
            free_ptr++;
            if(free_ptr == 3) free_ptr = 0;
            receive_success = TRUE;
      }
    }
    else{
            receive_success = FALSE;
    }
    return receive_success;
}

/**********************************************************
**Name:     sx1276_7_8_LoRaEntryTx
**Function: ----'Entry Tx mode
**Input:    None
**Output:   TRUE - success; FAIL - fail
**********************************************************/
bool Switch_To_Tx(void)
{
  bool switch_success = FALSE;
  u8 addr,temp;
  
  SPIWrite(Lora_1278_RegPaDac,PADAC_HIGH);                                      //Tx for 20dBm
  SPIWrite(Lora_1278_RegHopPeriod,0x00);	                                //RegHopPeriod NO FHSS
  SPIWrite(Lora_1278_RegDioMapping1,DIOMAPPING1_TX);   

  sx1276_7_8_LoRaClearIrq();
  SPIWrite(Lora_1278_RegIrqFlagsMask,0xF7);                       //Open TxDone interrupt
  SPIWrite(Lora_1278_RegPayloadLength,NUM_PAYLOAD);                       //RegPayloadLength  21byte

  addr = SPIRead(Lora_1278_RegFifoTxBaseAddr);           //RegFiFoTxBaseAddr
  SPIWrite(Lora_1278_RegFifoAddrPtr,addr);                        //RegFifoAddrPtr
  while(1)
  {
    temp = SPIRead(Lora_1278_RegPayloadLength);
    if(temp == 20)
    {
      switch_success = TRUE;
      break;
    }
  }
  return switch_success;
}
/**********************************************************
**Name:     sx1276_7_8_LoRaTxPacket
**Function: Send data in LoRa mode
**Input:    None
**Output:   TRUE - success; FAIL - fail
**********************************************************/
bool Send_Tx_Packet(u8* buf, u8 length)
{
  bool switch_success = FALSE;
  if(strlen((char*)buf) <= length){
          BurstWrite(Lora_1278_RegFifo, buf, length);
          SPIWrite(Lora_1278_RegOpMode,LORA_TX);                    //Tx Mode
          Wait_Tx_Done();
          switch_success = TRUE;
  }
  return switch_success;
}

/**********************************************************
**Name:     Wait_Tx_Done
**Function: wait for packet to be sent
**Input:    mode
**Output:   TRUE - success; FAIL - fail
**********************************************************/
u8 Wait_Tx_Done(){
        bool switch_success = FALSE;
	while(1)
        {
            if(HAL_GPIO_ReadPin(nIrq_GPIO_Port,nIrq_Pin) == 1)       //Packet send over
            {			
                    //SPIRead(LR_RegIrqFlags); //check
                    sx1276_7_8_LoRaClearIrq();                                //Clear irq									
                    switch_success = TRUE;
            }
        }		
        return switch_success;
}

/**********************************************************
**Name:     sx1276_7_8_Config_Init
**Function: sx1276_7_8 base config
**Input:    mode
**Output:   None
**********************************************************/
void sx1276_7_8_Config_Init(void)
{
  sx1276_7_8_Sleep();                                      //Change modem mode Must in Sleep mode
	HAL_Delay(15); 
	sx1276_7_8_EntryLoRa();	

	BurstWrite(Lora_1278_RegFrMsb,(u8*)SX1278_Frequency[Lora.Frequency],3);

	//setting base parameter
	SPIWrite(Lora_1278_RegPaConfig,Lora.OutputPower);             
	SPIWrite(Lora_1278_RegOcp,OCP_DISABLE);                              
	SPIWrite(Lora_1278_RegLna,LNA_G1);                                      //RegLNA,High & LNA Enable

	if(Lora.SpreadFactor==SF_6)         //SFactor=6
	{
		u8 tmp;
		SPIWrite(Lora_1278_RegModemConfig1,((Lora.BandWidth<<4)+(Lora.CodingRate<<1)+IMPLICIT_MODE));
                SPIWrite(Lora_1278_RegModemConfig2,((Lora.SpreadFactor<<4)+(Lora.CRCheck<<2)+Lora.RxTimeOut.TimeOutMsb));

		SPIWrite(Lora_1278_RegDetectOptimize,DETECTION_OPTIMIZE_SF6);
                SPIWrite(Lora_1278_RegDetectionThreshold,DETECTION_THRESHOLD_SF6);
	}
	else
	{
		SPIWrite(Lora_1278_RegModemConfig1,((Lora.BandWidth<<4)+(Lora.CodingRate<<1)+EXPLICIT_MODE));
            SPIWrite(Lora_1278_RegModemConfig2,((Lora.SpreadFactor<<4)+(Lora.CRCheck<<2)+Lora.RxTimeOut.TimeOutMsb));  
	}
	SPIWrite(Lora_1278_RegSymbTimeoutLsb,Lora.RxTimeOut.TimeOutLsb);        //RegSymbTimeoutLsb Timeout = 0x3FF(Max)
	SPIWrite(Lora_1278_RegPreambleMsb,PREAMBLE_MSB);                        //RegPreambleMsb
	SPIWrite(Lora_1278_RegPreambleLsb,PREAMBLE_LSB);                        //RegPreambleLsb 8+4=12byte Preamble
	SPIWrite(Lora_1278_RegDioMapping2,DIOMAPPING2_MODE);                    //RegDioMapping2 DIO5=00, DIO4=01
}

void Lora_1278_Init(void)
{
  Lora.Frequency = LORA_433MHZ;
  Lora.OutputPower = OP_20_dBm;
  Lora.SpreadFactor = SF_6;
  Lora.BandWidth = BW_500_kHz;
  Lora.CodingRate = CR_4_5;
  Lora.CRCheck = CRC_ENABLE; 
  Lora.RxTimeOut.TimeOutMsb = RX_TIMEOUT_MSB;
  Lora.RxTimeOut.TimeOutLsb = RX_TIMEOUT_LSB;
}

