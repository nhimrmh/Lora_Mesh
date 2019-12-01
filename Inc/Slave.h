#define BROADCAST_DELAY_TIME 100000 * 0.5

extern void Delay_By_Id(char* slave_id);
extern void Slave_Send_Response_Broadcast(char* slave_id);
extern void Slave_Send_Response_Unicast(char* slave_id);
extern void Slave_Send_Response(u8 uni_or_broad, char* slave_id);
extern void Slave_Receive_Data(char* slave_id);