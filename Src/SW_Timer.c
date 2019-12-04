



#include "stm32f1xx_hal.h"
#include "SW_Timer.h"
#include "My_type.h"
#include "main.h"
#include "mainApp.h"
#include "Master.h"

typedef struct
{
    u32 countdown;
    u32 time_setup;
    u8 is_Running :1;
    u8 is_Done    :1;
  
    SW_TIMER_CALLBACK func;
} SW_TIMER;

SW_TIMER SWTimer[SUM_TIMER];

void SW_TIMER_CLEAR(u8 Timer)
{
  SWTimer[Timer].countdown = 0xffffffff;
  SWTimer[Timer].is_Running = 0;
  SWTimer[Timer].is_Done = 0;
  SWTimer[Timer].time_setup = 0;
  SWTimer[Timer].func = NULL;
}

void SW_TIMER_INIT(void)
{
  u8 i;
  for( i =0; i < SUM_TIMER; i++)
  {
    SW_TIMER_CLEAR(i);
  }
}

void SW_TIMER_CREATE(u8 Timer, u16 TimeSetup)
{
  SWTimer[Timer].time_setup = TimeSetup;
  SWTimer[Timer].is_Running = 0;
  SWTimer[Timer].is_Done = 0;
}

void SW_TIMER_CREATE_FunCallBack(u8 Timer,u32 TimeSetup, SW_TIMER_CALLBACK fun)
{
  SWTimer[Timer].time_setup = TimeSetup;
  SWTimer[Timer].is_Running = 0;
  SWTimer[Timer].is_Done = 0;
  SWTimer[Timer].func = fun;
}


void SW_TIMER_START(u8 Timer)
{
  SWTimer[Timer].countdown = SWTimer[Timer].time_setup;
  SWTimer[Timer].is_Running = 1;
  SWTimer[Timer].is_Done = 0;
}

void SW_TIMER_STOP(u8 Timer)
{
  SWTimer[Timer].countdown = 0xffffffff;
  SWTimer[Timer].is_Running = 0;
}

void SW_TIMER_ISR()
{
  u8 i;
  for( i = 0; i < SUM_TIMER; i++)
  {
    if(SWTimer[i].is_Running)
    {
      if((SWTimer[i].countdown > 0) && (SWTimer[i].countdown != 0xffffffff))
      {
        SWTimer[i].countdown--;
        if(SWTimer[i].countdown == 0)
        {
          SWTimer[i].is_Done = 1;
          SWTimer[i].is_Running = 0;
          SWTimer[i].countdown = 0xffffffff;
          if(SWTimer[i].func != NULL)
          {
            SWTimer[i].func();
          }
        }
      }
    }
  }
}

u16 GET_SW_TIMER_COUNTER(u8 Timer)
{
  return (SWTimer[Timer].time_setup - SWTimer[Timer].countdown);
}

u8 GET_SW_TIMER_IS_RUNNING(u8 Timer)
{
  return SWTimer[Timer].is_Running;
}

u8 GET_SW_TIMER_IS_DONE(u8 Timer)
{
  u8 done;
  if(SWTimer[Timer].is_Done ==1)
  {
    done = 1;
    SWTimer[Timer].is_Done = 0;
  }
  else
    done = 0;
  
  return done;
}

void fun1(void)
{
  if(myLoraMode.uni_or_broad == 1){
    myLoraMode.slave_count = 0;
    myLoraMode.flag_timer = 0;
    myLoraMode.mode = 5;	
  }
  else{
    if(myLoraMaster.uni_sent == 0){
      myLoraMode.mode = 6;
    }
    else{
      myLoraMode.mode = 7;
    }
  }
}
  
  


