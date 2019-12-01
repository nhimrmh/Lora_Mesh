#include "My_type.h"


typedef void (*SW_TIMER_CALLBACK)(void);

typedef enum 
{
  SW_TIMER1,
  SW_TIMER_UNI,
  SUM_TIMER
} SW_TIMER_ENUM;

extern void SW_TIMER_CLEAR(u8 Timer);
extern void SW_TIMER_INIT(void);
extern void SW_TIMER_CREATE(u8 Timer, u16 TimeSetup);
extern void SW_TIMER_CREATE_FunCallBack(u8 Timer,u32 TimeSetup, SW_TIMER_CALLBACK fun);
extern void SW_TIMER_START(u8 Timer);
extern void SW_TIMER_STOP(u8 Timer);
extern void SW_TIMER_ISR(void);
extern u16 GET_SW_TIMER_COUNTER(u8 Timer);
extern u8 GET_SW_TIMER_IS_RUNNING(u8 Timer);
extern u8 GET_SW_TIMER_IS_DONE(u8 Timer);
extern void fun1(void);