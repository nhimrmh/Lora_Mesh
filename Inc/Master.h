typedef struct{
  u8 uni_received;
  u8 uni_sent;
  u8 sent;  
  u8 status[100];
  u8 status_prev[100];
}LoraMaster;

extern void Master_Send_Data();
extern LoraMaster myLoraMaster;