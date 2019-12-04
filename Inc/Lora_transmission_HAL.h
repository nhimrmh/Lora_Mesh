/*
 * sx1276_7_8.h
 *
 *  Created on: Sep 8, 2017
 *      Author: dkhairnar
 */

#ifndef SX1276_7_8_H_
#define SX1276_7_8_H_

#include "main.h"
#include "stm32f1xx_hal.h"
#include "My_type.h"
#include "spi.h"

/*****************IO define********************/
#define RED_LED_H()					HAL_GPIO_WritePin(RED_LED_GPIO_Port,RED_LED_Pin,GPIO_PIN_SET)//PD_ODR |= 0x40//PD6
#define RED_LED_L()					HAL_GPIO_WritePin(RED_LED_GPIO_Port,RED_LED_Pin,GPIO_PIN_RESET)//PD_ODR &= 0xbf

#define MAX_QUEUE_LENGTH 3
#define BROADCAST_CHARACTER "a"
#define SLAVE_IS_RECEIVING 1
#define MASTER_IS_RECEIVING 0
#define printUSB(x) CDC_Transmit_FS((uint8_t*)x,strlen((char*)x))
///////////////////////////////////////////////// LoRa mode //////////////////////////////////////////////////


//SX1278 Internal registers Address
/********************Lora Registers***************************/
#define Lora_1278_RegFifo                                  0x00
#define Lora_1278_RegOpMode                                0x01
#define Lora_1278_RegFrMsb                                 0x06
#define Lora_1278_RegFrMid                                 0x07
#define Lora_1278_RegFrLsb                                 0x08
#define Lora_1278_RegPaConfig                              0x09
#define Lora_1278_RegPaRamp                                0x0A
#define Lora_1278_RegOcp                                   0x0B
#define Lora_1278_RegLna                                   0x0C
#define Lora_1278_RegFifoAddrPtr                           0x0D
#define Lora_1278_RegFifoTxBaseAddr                        0x0E
#define Lora_1278_RegFifoRxBaseAddr                        0x0F
#define Lora_1278_RegFifoRxCurrentAddr                     0x10
#define Lora_1278_RegIrqFlagsMask                          0x11
#define Lora_1278_RegIrqFlags                              0x12
#define Lora_1278_RegRxNbBytes                             0x13
#define Lora_1278_RegRxHeaderCntValueMsb                   0x14
#define Lora_1278_RegRxHeaderCntValueLsb                   0x15
#define Lora_1278_RegRxPacketCntValueMsb                   0x16
#define Lora_1278_RegRxPacketCntValueLsb                   0x17
#define Lora_1278_RegModemStat                             0x18
#define Lora_1278_RegPktSnrValue                           0x19
#define Lora_1278_RegPktRssiValue                          0x1A
#define Lora_1278_RegRssiValue                             0x1B
#define Lora_1278_RegHopChannel                            0x1C
#define Lora_1278_RegModemConfig1                          0x1D
#define Lora_1278_RegModemConfig2                          0x1E
#define Lora_1278_RegSymbTimeoutLsb                        0x1F
#define Lora_1278_RegPreambleMsb                           0x20
#define Lora_1278_RegPreambleLsb                           0x21
#define Lora_1278_RegPayloadLength                         0x22
#define Lora_1278_RegMaxPayloadLength                      0x23
#define Lora_1278_RegHopPeriod                             0x24
#define Lora_1278_RegFifoRxByteAddr                        0x25
#define Lora_1278_RegModemConfig3                          0x26
#define Lora_1278_RegFeiMsb                                0x28
#define Lora_1278_RegFeiMid                                0x29
#define Lora_1278_RegFeiLsb                                0x2A
#define Lora_1278_RegRssiWideband                          0x2C
#define Lora_1278_RegDetectOptimize                        0x31
#define Lora_1278_RegInvertIQ                              0x33
#define Lora_1278_RegDetectionThreshold                    0x37
#define Lora_1278_RegSyncWord                              0x39
// I/O settings
#define Lora_1278_RegDioMapping1                           0x40
#define Lora_1278_RegDioMapping2                           0x41
// Version
#define Lora_1278_RegVersion                               0x42
// Additional settings
#define Lora_1278_RegPllHop                                0x44
#define Lora_1278_RegTcxo                                  0x4B
#define Lora_1278_RegPaDac                                 0x4D
#define Lora_1278_RegFormerTemp                            0x5B
#define Lora_1278_RegAgcRef                                0x61
#define Lora_1278_RegAgcThresh1                            0x62
#define Lora_1278_RegAgcThresh2                            0x63
#define Lora_1278_RegAgcThresh3                            0x64
#define Lora_1278_RegPll                                   0x70
/********************Lora mode***************************/
// Lora mode
#define LORA_ON                                            0x88
#define LORA_SLEEP                                         0x08
#define LORA_STDBY                                         0x89
#define LORA_FSTX                                          0x8A
#define LORA_TX                                            0x8B
#define LORA_FSRX                                          0x8C
#define LORA_RXCONTINUOUS                                  0x8D
#define LORA_RXSINGLE                                      0x8E
#define LORA_CAD                                           0x8F
// Frequency Mode
#define F_434_MSB                                          0x6C
#define F_434_MID                                          0x80
#define F_434_LSB                                          0x00
#define LORA_433MHZ                                        0
static const u8 SX1278_Frequency[1][3] = 
{ 
  { 0x6C, 0x80, 0x00 },  // 433 Mhz
};
// OutputPower Mode when PA_BOOST enable
#define OP_20_dBm                                          0xFF  
#define OP_17_dBm                                          0xFC
#define OP_14_dBm                                          0xF9
#define OP_11_dBm                                          0xF6
// SpreadFactor Mode
#define SF_6                                               6
#define SF_7                                               7
#define SF_8                                               8
#define SF_9                                               9
#define SF_10                                              10
#define SF_11                                              11
#define SF_12                                              12
// BandWidth Mode
#define BW_7_kHz                                           0
#define BW_10_kHz                                          1
#define BW_15_kHz                                          2
#define BW_20_kHz                                          3
#define BW_31_kHz                                          4
#define BW_41_kHz                                          5
#define BW_62_kHz                                          6
#define BW_125_kHz                                         7
#define BW_250_kHz                                         8
#define BW_500_kHz                                         9
// CodingRate Mode
#define CR_4_5                                             0x01
#define CR_4_6                                             0x02
#define CR_4_7                                             0x03
#define CR_4_8                                             0x04
// CRC check
#define CRC_ENABLE                                         0x01
#define CRC_DISABLE                                        0x00
// OCP - Overload Current Protection for PA
#define OCP_ENABLE                                         0x2B                 // OCPTrim default 100mA
#define OCP_DISABLE                                        0x0B                 
// LNA gain setting with Boost on
#define LNA_G1                                             0x23                 // Maximum gain
#define LNA_G2                                             0x43
#define LNA_G3                                             0x63
#define LNA_G4                                             0x83
#define LNA_G5                                             0xA3
#define LNA_G6                                             0xC3                 // Minimum gain
// Header Mode 
#define EXPLICIT_MODE                                      0x00
#define IMPLICIT_MODE                                      0x01
// Symb Timeout
#define RX_TIMEOUT_MSB                                     0x03                 // Max is 3
#define RX_TIMEOUT_LSB                                     0xFF                 // Timeout = RX_TIMEOUT.Ts
// LoRa Detection Optimize
#define DETECTION_OPTIMIZE_SF6                             0xC5 
#define DETECTION_OPTIMIZE_SF7_12                          0xC3                 // Reg default
// LoRa Detection Threshold
#define DETECTION_THRESHOLD_SF6                            0x0C
#define DETECTION_THRESHOLD_SF7_12                         0x0A                 // Reg default
// Lora Preamble Length
#define PREAMBLE_MSB                                       0x00                 // Preamble MSB = PREAMBLE_MSB + 4.25
#define PREAMBLE_LSB                                       0x08   
// Lora DioMapping2 Mode DIO5 and DIO4
#define DIOMAPPING2_MODE                                   0x01                 // RegDioMapping2 DIO5=00, DIO4=01
// Lora DioMapping1 Mode DIO3,DIO2,DIO1,DIO0
#define DIOMAPPING1_TX                                     0x41                 // DIO0=01, DIO1=00, DIO2=00, DIO3=01
#define DIOMAPPING1_RX                                     0x01                 // DIO0=00, DIO1=00, DIO2=00, DIO3=01
// Invert IQ 
#define INVERT_NORMAL                                      0x27
#define INVERT_IQ                                          0x67
// Clear IrqFlags
#define IRQFLAG_CLEAR                                      0xFF
// High power + 20 dBm
#define PADAC_HIGH                                         0x87
#define PADAC_NORMAL                                       0x84
// FHSS enable
#define FHSS_ENABLE                                        0x00
#define FHSS_DISABLE                                       0xFF
// Payload Length
#define NUM_PAYLOAD                                        20
/*********************************************************/

typedef enum 
{
  STDBY,
  SLEEP,
  TX,
  RX
}Lora_Status;

typedef struct 
{
  u8 TimeOutMsb : 2;
  u8 TimeOutLsb;
}Lora_RxTimeOut;

typedef struct
{
  u8                    Frequency;
  u8                    OutputPower;
  u8                    SpreadFactor;
  u8                    BandWidth;
  u8                    CodingRate;
  u8                    CRCheck;
  Lora_RxTimeOut        RxTimeOut;
  Lora_Status           status;
}Lora_HandleTypeDef;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern u8 temp;
extern u8 mode;
extern u8 Freq_Sel;
extern u8 Power_Sel;
extern u8 Lora_Rate_Sel;
extern u8 BandWide_Sel;
extern u8 Fsk_Rate_Sel;
extern u16 SysTime;
extern void delay_ms(unsigned int ms);
extern void delay_us(unsigned int us);
extern u8 result;
extern u8 free_ptr;

extern u8 Wait_Tx_Done();
extern void sx1276_7_8_Reset(void);
extern void sx1276_7_8_Config_Init(void);
extern void sx1276_7_8_Config_clk(void);

extern bool Switch_To_Tx(void);
extern bool Switch_To_Rx(void);
extern void Switch_To_Standby();

extern u8 sx1276_7_8Data[20];
extern u8 RxData[3][20];
extern bool Send_Tx_Packet(u8* buf, u8 length);
extern bool Indicate_Rx_Packet(char* slave_id, u8 m_or_s);
extern bool Read_Rx_Packet(char* Rx_Packet, u8 length, char* slave_id, u8 m_or_s);

extern s8 sx1276_7_8_LoRaReadRSSI(void);
extern Lora_HandleTypeDef Lora;
extern void Lora_1278_Init(void);
#endif /* SX1276_7_8_H_ */
