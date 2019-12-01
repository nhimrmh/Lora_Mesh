/*
 * mainApp.h
 *
 *  Created on: Sep 8, 2017
 *      Author: dkhairnar
 */
#include "My_type.h"
#ifndef MAINAPP_H_
#define MAINAPP_H_

void mainApp(void);

typedef struct{
  u8 mode;
  u8 uni_or_broad;
  u8 slave_count;
  char strBuf[128];
}LoraMode;

extern LoraMode myLoraMode;

#endif /* MAINAPP_H_ */
