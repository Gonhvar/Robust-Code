#include <stdio.h>
#include <stdint.h>
#include <strings.h> 
#include <PCANBasic.h>
#include <iostream>
#include <unistd.h>
#include <string.h>

using namespace std;
//Definition des COBID
#define COBID_ALL_CAN_SDO 0x600
#define COBID_CAN1_SDO 0x601 
#define COBID_CAN3_SDO 0x603 

//-W = write | R = Read | B = Bytes-
#define W_1B 0x2F
#define W_2B 0x2B
#define W_4B 0x23
#define R_1B 0x40

//Valeur de base de la sortie des messages CanOpen
TPCANHandle channelUsed = PCAN_USBBUS1;
TPCANBaudrate baudrateUsed = PCAN_BAUD_1M;

//================INITIALISATION================
TPCANHandle find_channel(){
    TPCANHandle channelsToCheck[] = { PCAN_USBBUS1, PCAN_USBBUS2, PCAN_USBBUS3, PCAN_USBBUS4, PCAN_USBBUS5, PCAN_USBBUS6
     , PCAN_USBBUS7, PCAN_USBBUS8, PCAN_USBBUS9, PCAN_USBBUS10, PCAN_USBBUS11, PCAN_USBBUS12, PCAN_USBBUS13, PCAN_USBBUS14
     , PCAN_USBBUS15, PCAN_USBBUS16};
    DWORD condition, activate;
    TPCANStatus status;

    DWORD channelsCount;
    if (CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS_COUNT, &channelsCount, 4) == PCAN_ERROR_OK)
    {
        printf("Total of %d channels were found:\n", channelsCount);
        if (channelsCount > 0)
        {
            TPCANChannelInformation* channels = new TPCANChannelInformation[channelsCount];
            if (CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS, channels, channelsCount *
             sizeof(TPCANChannelInformation)) == PCAN_ERROR_OK)
            {
                for (int i = 0; i < (int)channelsCount; i++)
                {
                    printf("%d) ---------------------------\n", i + 1);
                    printf("Name: %s\n", channels[i].device_name);
                    printf("Handle: 0x%X\n", channels[i].channel_handle);
                    printf("Controller: %d\n", channels[i].controller_number);
                    printf(" Condition: %d\n\n", channels[i].channel_condition);
                    printf(" ---------------------------\n\n");
                }
                //On renvois la premiére channel trouvé 
                return channels[0].channel_handle;
            }
        }
    }
    else{

        printf(" ---------------------------\n");
        printf(" ERREUR INITIALISATION\n");
        printf(" ---------------------------\n\n");
    }
    return channelsToCheck[0];
}

TPCANBaudrate init_baudrate_doc(){

    TPCANBaudrate baudrates[] = { PCAN_BAUD_125K, PCAN_BAUD_250K, PCAN_BAUD_500K, PCAN_BAUD_1M };
    DWORD listenOnlyState = PCAN_PARAMETER_ON;
    TPCANMsg msg;
    printf("Checking the current bitrate on channel 0x%X\n", channelUsed);
    for (int i = 0; i < 4; i++)
    {
        if (CAN_SetValue(channelUsed, PCAN_LISTEN_ONLY, &listenOnlyState, 4) == PCAN_ERROR_OK)
        {
            if (CAN_Initialize(channelUsed, baudrates[i]) == PCAN_ERROR_OK)
            {
                printf(".....");
                // Wait 1 second before trying to get a message
                sleep(1);
                // If a message is received, then bitrate was found
                if (CAN_Read(channelUsed, &msg, NULL) == PCAN_ERROR_OK)
                {
                    printf("\nBitrate (BTR0BTR1) used is 0x%X", baudrates[i]);
                    return baudrates[i];
                }
                CAN_Uninitialize(channelUsed);
            }
            else
            {
                printf("Error! Channel could not be initialized.\n");
                break;
            }
        }
        else
        {
            printf("Error! The listen-only feature could not be activated\n");
            break;
        }
    }
    return PCAN_BAUD_1M;
}

//Regarde avec lsusb dans un terminal pour savoir le channel 
void initialise_CAN_USB(){
    
    char strMsg[256];
    TPCANStatus result;
    DWORD receptionState;

    if (CAN_Initialize(channelUsed, baudrateUsed) == PCAN_ERROR_OK)
    {
        // Doing work. At some point, when needed, the reception of messages is turned off...
        receptionState = PCAN_PARAMETER_OFF;
        if (CAN_SetValue(channelUsed, PCAN_RECEIVE_STATUS, &receptionState, 4) == PCAN_ERROR_OK)
        {
            printf("Message reception on channel 0x%X is now disabled.\n", channelUsed);
            // do needed work...
            printf("Operation finished. Enabling communication again...\n");
            receptionState = PCAN_PARAMETER_ON;

        if (CAN_SetValue(channelUsed, PCAN_RECEIVE_STATUS, &receptionState, 4) == PCAN_ERROR_OK)
        {
            printf("Normal operation on channel 0x%X restablished.\n\n", channelUsed);
            printf("=======FIN INIT=======\n");
        }
        else
            printf("Message reception of channel 0x%X could not be restablished\n", channelUsed);
        }
        else
            printf("Message reception of channel 0x%X could not be changed\n", channelUsed);
    }
    else
        printf("Channel 0x%X could not be initialized\n", channelUsed);
}

//==================MESSAGES==================
void init_msg_SDO(TPCANMsg* msg, int id, uint8_t data_length,uint8_t index_1, uint8_t index_2, uint8_t subIndex, uint8_t data[4]){
    // A CAN message is configured
    //Note : BYTE DATA[8] 

    msg->ID = id;
    msg->MSGTYPE = PCAN_MESSAGE_STANDARD;
    msg->LEN = 8;

    //On copie chacune des valeurs de msg_d dans msg->DATA

    msg->DATA[0] = data_length;
    msg->DATA[1] = index_2;
    msg->DATA[2] = index_1;
    msg->DATA[3] = subIndex;
    
    //On met la data de maniére un peu différente en fonction de la taille du msg
    switch(data_length){
        case W_1B : 
            msg->DATA[4] = data[0];

            msg->DATA[5] = 0;
            msg->DATA[6] = 0;
            msg->DATA[7] = 0;
            break;
        
        case W_2B :
            msg->DATA[4] = data[1];
            msg->DATA[5] = data[0];

            msg->DATA[6] = 0;
            msg->DATA[7] = 0;
            break;
        
        case W_4B : 
            msg->DATA[4] = data[3];
            msg->DATA[5] = data[2];
            msg->DATA[6] = data[1];
            msg->DATA[7] = data[0];
            break;

        default :
            msg->DATA[4] = 0;
            msg->DATA[5] = 0;
            msg->DATA[6] = 0;
            msg->DATA[7] = 0;
    }
}

void print_message(TPCANMsg received){

    uint32_t result;
    
    printf("\n+++++++++++++++++\n");
    printf("Index : 0x%hhx-%hhx | Sub-Index : 0x%hhx\n", received.DATA[2], received.DATA[1], received.DATA[3]);

    switch(received.DATA[0]){
        case W_1B :
            //1 octet
            result = received.DATA[4];
            printf("Data hexa : 0x%hhx\n",received.DATA[4]);
            printf("Data deci : %d", result);
            break;

        case W_2B :
            //2 octets
            result = (received.DATA[5]<<8) | received.DATA[4];
            printf("Data hexa : 0x%hhx-%hhx\n",received.DATA[5],received.DATA[4]);
            printf("Data deci : %d", result);
            break;

        case W_4B :
            //4 octets
            result = (received.DATA[7]<<24) | (received.DATA[6]<<16)| (received.DATA[5]<<8) | received.DATA[4];
            printf("Data hexa : 0x%hhx-%hhx-%hhx-%hhx\n",received.DATA[7],received.DATA[6],received.DATA[5],received.DATA[4]);
            printf("Data decimal : %d", result); 
            break;
        
        case 0x60 : 
            printf("Message renvoyé aprés ecriture");
            break;
            
        default : 
            printf("Erreur taille du message\n");
    }
    printf("\n+++++++++++++++++\n");
}

void write_message(TPCANMsg msg){

    TPCANStatus result;
    char strMsg[256];

    // The message is sent using the PCAN-USB Channel 1
    result = CAN_Write(channelUsed, &msg);
    if(result != PCAN_ERROR_OK)
    {
        // An error occurred, get a text describing the error and show it
        //
        CAN_GetErrorText(result, 0, strMsg);
        printf("%s\n",strMsg);
    }
    else{
        //printf("Message sent successfully\n");
    }

    //En micro seconde :
    usleep(1000);
    //printf("--------\n");
}

void read_message(){

    TPCANMsg received;
    TPCANStatus result;
    TPCANTimestamp timestamp;
    char strMsg[256];

    do
    {
        // Check the receive queue for new messages
        //
        result = CAN_Read(channelUsed,&received,&timestamp);
        if(result != PCAN_ERROR_QRCVEMPTY)
        {
            // Process the received message
            //
            //printf("msg type : %hhx\n", received.MSGTYPE);
            //printf("Le message est arrivé en %dms\n", timestamp.millis);
            print_message(received);

        }
        else
        {
            // An error occurred, get a text describing the error and show it
            // and handle the error
            //
            CAN_GetErrorText(result, 0, strMsg);
            printf("%s\n", strMsg);
            // Here can be decided if the loop has to be  terminated (eg. the bus
            // status is  bus-off)
            //
        }
    // Try to read a message from the receive queue of the PCAN-USB, Channel 1,
    // until the queue is empty
    //
    }while((result & PCAN_ERROR_QRCVEMPTY) != PCAN_ERROR_QRCVEMPTY);

    printf("--------\n");
}

void get_value(TPCANMsg toSend){
    //----------------------------------
    //DATA[0] Toujours égale à 0x40 si on veut read une valeur
    toSend.DATA[0] = R_1B;
    write_message(toSend);

    //-------------------------------------------
    usleep(1000);  //En micro-secondes
    //-------------------------------------------

    read_message();
}
//==================INIT_CONTROL_POSITION==================

void def_positionAbsolue(int id){
    TPCANMsg msg;
    uint8_t msg_d[4];
    bzero(msg_d, 4);

    //Modes of operation (Homing Mode)
    msg_d[0] = 0x06;
    init_msg_SDO(&msg, id, W_1B, 0x60, 0x60, 0x00, msg_d);
    write_message(msg);

    //Homing method
    msg_d[0] = 0x25;
    init_msg_SDO(&msg, id, R_1B, 0x60, 0x98, 0x00, msg_d);
    write_message(msg);

    //Controlword (Shutdown)
    msg_d[0] = 0x00;
    msg_d[1] = 0x06;
    init_msg_SDO(&msg, id, W_2B, 0x60, 0x40, 0x00, msg_d);
    write_message(msg);

    //Controlword (Switch on & Enable)
    msg_d[0] = 0x00;
    msg_d[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, 0x60, 0x40, 0x00, msg_d);
    write_message(msg);

    //Controlword (Start homing)
    msg_d[0] = 0x00;
    msg_d[1] = 0x1F;
    init_msg_SDO(&msg, id, W_2B, 0x60, 0x40, 0x00, msg_d);
    write_message(msg);

    //Controlword (Halt homing)
    msg_d[0] = 0x01;
    msg_d[1] = 0x1F;
    init_msg_SDO(&msg, id, W_2B, 0x60, 0x40, 0x00, msg_d);
    write_message(msg);
}

void init_asservissementPosition(int id){

    TPCANMsg msg;
    uint8_t msg_d[4];
    bzero(msg_d, 4);

    printf("Debut positionAbsolue\n");
    def_positionAbsolue(id);
    printf("Fin positionAbsolue\n");

    //---------Modes of operation
    msg_d[0] = 0x01;
    init_msg_SDO(&msg, id, W_1B, 0x60, 0x60, 0x00, msg_d);
    write_message(msg);

    //---------Set parameter : fait avec EPOSSTUDIO
    //
    //
    //---------Enable device
    //Controlword (shutdown)
    msg_d[0] = 0;
    msg_d[1] = 0x06;
    init_msg_SDO(&msg, id, W_2B, 0x60, 0x40, 0x00, msg_d);
    write_message(msg);

    //Controlword (shutdown)
    msg_d[0] = 0;
    msg_d[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, 0x60, 0x40, 0x00, msg_d);
    write_message(msg);

}

void set_relativePosition(int id, uint8_t userInput){
    TPCANMsg msg;
    uint8_t msg_d[4];
    bzero(msg_d, 4);

    //Target position
    msg_d[0] = userInput;
    msg_d[1] = 0;
    msg_d[2] = 0;
    msg_d[3] = 0;
    init_msg_SDO(&msg, id, W_4B, 0x60, 0x7A, 0x00, msg_d);
    write_message(msg);

    //Controlword (relative position)
    msg_d[0] = 0;
    msg_d[0] = 0x5F;
    init_msg_SDO(&msg, id, W_2B, 0x60, 0x4A, 0x00, msg_d);
    write_message(msg);

    /*//Controlword (New Position)
    msg_d[0] = 0;
    msg_d[0] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, 0x60, 0x40, 0x00, msg_d);
    write_message(msg);*/
}

void AsservissementPosition(int id){
    uint32_t userInput = 1;
    
    printf("Debut setup\n");
    init_asservissementPosition(id);
    printf("Fin setup\n");

    while(userInput != 0){
        printf("Donnez une valeur en decimal pour la position (0 pour quitter): \n");
        cin >> userInput;
        set_relativePosition(id, userInput);
        usleep(1000);
    }
    printf("Fin du programme : \n");
}

//==================MAIN==================
int main(){
    //Initialisation des variables 
    TPCANMsg msg;
    uint8_t msg_d[4];
    bzero(msg_d, 4);

    printf("=======DEBUT PROGRAMME=======\n");
    //Init Dynamique des valeurs de channel et de baudrate 
    channelUsed = find_channel();
    baudrateUsed = init_baudrate_doc();

    //Initialisation du PEAK
    initialise_CAN_USB();

    //Pour le controle en position
    AsservissementPosition(COBID_CAN1_SDO);
    
    /*
    msg_d[0] = 0x25;
    msg_d[1] = 0xFF;
    init_msg_SDO(&msg,COBID_CAN1_SDO, W_2B, 0x60, 0x7F, 0x60, msg_d);
    print_message(msg);
    
    //On receptionne le message
    read_message();
    */
    printf("=======FIN PROGRAMME=======\n");
}