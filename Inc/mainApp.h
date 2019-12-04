/*
 * mainApp.h
 *
 *  Created on: Sep 8, 2017
 *      Author: dkhairnar
 */
#include "My_type.h"
#ifndef MAINAPP_H_
#define MAINAPP_H_
#define SLAVE_TX 0
#define SLAVE_RX 1
#define MASTER_TX 3
#define MASTER_RX 4
#define PACKET_LENGTH 20
#define PRINTUSB_LENGTH 50
#define NUMBER_OF_SLAVES 8
#define FIRST_POSITION_OF_QUEUE 0
#define LAST_POSITION_OF_QUEUE 3
#define RESET_VALUE 0
void mainApp(void);

typedef struct{
  u8 mode;
  u8 uni_or_broad;
  u8 slave_count;
  char strBuf[128];
  u8 flag_timer;
}LoraMode;

extern LoraMode myLoraMode;

#endif /* MAINAPP_H_ */
