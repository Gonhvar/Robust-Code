#include <stdio.h>
#include <stdint.h>
#include <PCANBasic.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>

using namespace std;

//Definition des COBID
#define COBID_ALL_CAN_SDO 0x600
#define COBID_CAN1_SDO 0x601 
#define COBID_CAN2_SDO 0x602 
#define COBID_CAN3_SDO 0x603 

//-W = write | R = Read | B = Bytes-
#define W_1B 0x2F
#define W_2B 0x2B
#define W_4B 0x23
#define R 0x40
#define R_1B 0x4F
#define R_2B 0x4B
#define R_4B 0x43

//INDEX :
#define CONTROLWORD 0x6040
#define STATUSWORD  0x6041
#define MODES_OF_OPERATION 0x6060
#define TARGET_TORQUE 0x6071
#define TARGET_POSITION 0x607A
#define HOMING_METHOD 0x6098
#define TORQUE_OFFSET 0x60B2



TPCANHandle find_channel();
TPCANBaudrate init_baudrate_doc();
void initialise_CAN_USB();
void init_msg_SDO(TPCANMsg* msg, int id, uint8_t data_length,uint8_t index_1, uint8_t index_2, uint8_t subIndex, uint8_t data[4]);
void get_manualWantedPos(float *wantPosX , float *wantPosY);
void checkEndTarget(uint8_t* status, int id);
void checkAllEndTarget();