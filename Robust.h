#include <stdio.h>
#include <stdint.h>

TPCANHandle find_channel();
TPCANBaudrate init_baudrate_doc();
void initialise_CAN_USB();
void init_msg_SDO(TPCANMsg* msg, int id, uint8_t data_length,uint8_t index_1, uint8_t index_2, uint8_t subIndex, uint8_t data[4]);
void print_message(TPCANMsg received);
void write_message(TPCANMsg msg);
void read_message();
void get_value(TPCANMsg toSend);
void def_positionAbsolue(int id);
void init_asservissementPosition(int id);
void set_relativePosition(int id, uint8_t userInput);
void AsservissementPosition(int id);

int main();