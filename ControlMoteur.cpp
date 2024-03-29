/**
 * @file Robust.cpp
 * @author Equipe Robust
 * @brief 
 * @version 0.1
 * @date 2023-03-09
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "ControlMoteur.hpp"
#include "modele.hpp"


//Valeur de base de la sortie des messages CanOpen
TPCANHandle channelUsed = PCAN_USBBUS1;
TPCANBaudrate baudrateUsed = PCAN_BAUD_1M;

int ControlMoteur::nombreInstance =0;


//================INITIALISATION_PEAK================

/**
 * @brief Va chercher un channel disponible du PEAK automatiquement
 * 
 * @return TPCANHandle : Channel trouvé
 */
TPCANHandle ControlMoteur::find_channel(){
    TPCANHandle channelsToCheck[] = { PCAN_USBBUS1, PCAN_USBBUS2, PCAN_USBBUS3, PCAN_USBBUS4, PCAN_USBBUS5, PCAN_USBBUS6
     , PCAN_USBBUS7, PCAN_USBBUS8, PCAN_USBBUS9, PCAN_USBBUS10, PCAN_USBBUS11, PCAN_USBBUS12, PCAN_USBBUS13, PCAN_USBBUS14
     , PCAN_USBBUS15, PCAN_USBBUS16};

    DWORD channelsCount;
    if (CAN_GetValue(PCAN_NONEBUS, PCAN_ATTACHED_CHANNELS_COUNT, &channelsCount, 4) == PCAN_ERROR_OK)
    {
        printf("PCAN : Total of %d channels were found:\n", channelsCount);
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
        else {
            //exit(1); [!] REMETRE
        }
    }
    else{

        printf(" ---------------------------\n");
        printf(" ERREUR INITIALISATION\n");
        printf(" ---------------------------\n\n");
    }
    return channelsToCheck[0];
}


/**
 * @brief Va chercher le baudrate de la liaison CANOpen automatiquement 
 * 
 * @return TPCANBaudrate : Baudrate trouvé
 */
TPCANBaudrate ControlMoteur::init_baudrate_doc(){

    TPCANBaudrate baudrates[] = { PCAN_BAUD_1M, PCAN_BAUD_500K, PCAN_BAUD_250K, PCAN_BAUD_125K};
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

/**
 * @brief Initialise la connexion avec le PEAK
 * 
 */
void ControlMoteur::initialise_CAN_USB(){
    
    DWORD receptionState;

    //Init Dynamique des valeurs de channel et de baudrate 
    channelUsed = find_channel();
    //Remettre a la fin

    //baudrateUsed = init_baudrate_doc();

    if (CAN_Initialize(channelUsed, baudrateUsed) == PCAN_ERROR_OK)
    {
        // Doing work. At some point, when needed, the reception of messages is turned off...
        receptionState = PCAN_PARAMETER_OFF;
        if (CAN_SetValue(channelUsed, PCAN_RECEIVE_STATUS, &receptionState, 4) == PCAN_ERROR_OK)
        {
            printf("PCAN : Message reception on channel 0x%X is now disabled.\n", channelUsed);
            // do needed work...
            printf("PCAN : Operation finished. Enabling communication again...\n");
            receptionState = PCAN_PARAMETER_ON;

        if (CAN_SetValue(channelUsed, PCAN_RECEIVE_STATUS, &receptionState, 4) == PCAN_ERROR_OK)
        {
            printf("PCAN : Normal operation on channel 0x%X restablished.\n\n", channelUsed);
            printf("=======FIN INIT=======\n");
        }
        else
            printf("PCAN : Message reception of channel 0x%X could not be restablished\n", channelUsed);
        }
        else
            printf("PCAN : Message reception of channel 0x%X could not be changed\n", channelUsed);
    }
    else
        printf("PCAN : Channel 0x%X could not be initialized\n", channelUsed);
}



//==================MESSAGES==================

/**
 * @brief Initialise un TPCANMsg (Message SDO pour CANOpen) avec les valeurs données 
 * 
 * @param msg : Message renvoyé en sortie
 * @param id : Valeur du COB-ID voulu
 * @param data_length : Taille du message (choix entre 1, 2 et 4 bytes) 
 * @param index : Valeur de l'index
 * @param subIndex : Valeur du sous-index
 * @param data : Valeur des données à envoyer
 */
void ControlMoteur::init_msg_SDO(TPCANMsg* msg, int id, uint8_t data_length, uint16_t index, uint8_t subIndex, uint8_t data[4]){
    // A CAN message is configured
    //Note : BYTE DATA[8] 

    msg->ID = id;
    msg->MSGTYPE = PCAN_MESSAGE_STANDARD;
    msg->LEN = 8;

    //On copie chacune des valeurs de msg_data dans msg->DATA

    msg->DATA[0] = data_length;
    msg->DATA[1] = index % 256;
    msg->DATA[2] = index >> 8;
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


/**
 * @brief Affiche le message donné
 * 
 * @param received (TPCANMsg) : Message à afficher
 */
void ControlMoteur::print_message(TPCANMsg received){

    uint32_t result = -1;

    //Si ce n'est pas un message renvoyé aprés ecriture
    if(received.DATA[0]!=0x60){
        printf("\n---------------n");
        printf("Index : 0x%hhx-%hhx | Sub-Index : 0x%hhx\n", received.DATA[2], received.DATA[1], received.DATA[3]);

        switch(received.DATA[0]){
            case R_1B :
                //1 octet
                printf("Message de 1 octet\n");
                result = received.DATA[4];
                printf("Data hexa : 0x%hhx\n",received.DATA[4]);
                printf("Data deci : %d", result);
                break;

            case R_2B :
                //2 octets
                printf("Message de 2 octets\n");
                result = (received.DATA[5]<<8) | received.DATA[4];
                printf("Data hexa : 0x%hhx|%hhx\n",received.DATA[5],received.DATA[4]);
                printf("Data deci : %d", result);
                break;

            case R_4B :
                //4 octets
                printf("Message de 4 octets\n");
                result = (received.DATA[7]<<24) | (received.DATA[6]<<16)| (received.DATA[5]<<8) | received.DATA[4];
                printf("Data hexa : 0x%hhx|%hhx|%hhx|%hhx\n",received.DATA[7],received.DATA[6],received.DATA[5],received.DATA[4]);
                printf("Data decimal : %d", result); 
                break;
                
            default : 
                printf("Erreur taille du message : 0x%hhx\n", received.DATA[0]);
        }
        printf("\n---------------n");
            
    }
    else{
        printf("Message confirmation de Index : 0x%hhx-%hhx | Sub-Index : 0x%hhx\n", received.DATA[2], received.DATA[1], received.DATA[3]);
        //printf("Data : %hhx | %hhx | %hhx | %hhx", received.DATA[7], received.DATA[6], received.DATA[5], received.DATA[4]);
    }
}

void ControlMoteur::print_vectorMessage(vector<msgRecu> get){

    printf("\n+++++++++DEBUT LISTE+++++++++\n");
    for(msgRecu g : get){
        //Si ce n'est pas un message renvoyé aprés ecriture
        if(!g.isConfirmReception){
            printf("\n---------------\n");
            printf("Index : 0x%hhx-%hhx | Sub-Index : 0x%hhx\n", g.index>>8, g.index, g.subIndex);

            switch(g.taille){
                case R_1B :
                    //1 octet
                    printf("Message de 1 octet\n");
                    printf("Data hexa : 0x%hhx\n", g.valData);
                    printf("Data deci : %d", g.valData);
                    break;

                case R_2B :
                    //2 octets
                    printf("Message de 2 octets\n");
                    printf("Data hexa : 0x%hhx|%hhx\n", (g.valData>>8) , g.valData);
                    printf("Data deci : %d", g.valData);
                    break;

                case R_4B :
                    //4 octets
                    printf("Message de 4 octets\n");
                    printf("Data hexa : 0x%hhx|%hhx|%hhx|%hhx\n",(g.valData>>24) ,(g.valData>>16) ,(g.valData>>8) , g.valData);
                    printf("Data decimal : %d", g.valData); 
                    break;
                    
                default : 
                    printf("Erreur taille du message : printmessage\n");
            }
            printf("\n---------------\n");
                
        }
        else{
            printf("Message confirmation de Index : 0x%hhx-%hhx | Sub-Index : 0x%hhx\n", g.index>>8, g.index, g.subIndex);
        }
    }
    printf("\n+++++++++FIN LISTE+++++++++\n\n");
}

/**
 * @brief Met le message en entrée dans une struct msgRecu
 * 
 * @param received (TPCANMsg) : Message à transcrire
 * @return msgRecu : Message transcrit
 */
msgRecu ControlMoteur::get_message(TPCANMsg received){

    uint32_t result = -1;
    msgRecu msgRecup;

    msgRecup.id = received.ID % 16;
    msgRecup.index = (received.DATA[2]*256) + received.DATA[1];
    msgRecup.subIndex = received.DATA[3];
    
    //Si ce n'est pas un message renvoyé aprés ecriture
    if(received.DATA[0]!=0x60){
        switch(received.DATA[0]){
            case R_1B :
                //1 octet
                msgRecup.taille = R_1B;
                result = received.DATA[4];
                break;

            case R_2B :
                //2 octets
                msgRecup.taille = R_2B;
                result = (received.DATA[5]<<8) | received.DATA[4];
                break;

            case R_4B :
                //4 octets
                msgRecup.taille = R_4B;
                result = (received.DATA[7]<<24) | (received.DATA[6]<<16)| (received.DATA[5]<<8) | received.DATA[4];
                break;
                
            default : 
                result= 0;
        }
    }
    else{
        msgRecup.isConfirmReception = true;
        return msgRecup;
    }

    msgRecup.valData = result;
    msgRecup.isConfirmReception = false;
    return msgRecup;
}

/**
 * @brief Lis dans les messages disponible dans le PEAK 
 * 
 * @return vector<msgRecu> : Renvois un array de msgRecu (venant de la liste d'attente du PEAK)
 */
vector<msgRecu> ControlMoteur::read_message(){

    TPCANMsg received;
    TPCANStatus result;
    TPCANTimestamp timestamp;
    vector<msgRecu> get;

    do
    {
        // Check the receive queue for new messages
        //
        result = CAN_Read(channelUsed,&received,&timestamp);
        //Si ce n'est pas vide 
        if(result != PCAN_ERROR_QRCVEMPTY)
        {
            //Un message et reçue et on l'affiche :
            //print_message(received);
            get.push_back(get_message(received));
        }
    //Jusqu'a ce que la file d'attente soit vide
    }while((result & PCAN_ERROR_QRCVEMPTY) != PCAN_ERROR_QRCVEMPTY);
    return get;
}


/**
 * @brief Ecrit le message spécifié
 * 
 * @param msg (TPCANMsg) : message à écrire
 */
void ControlMoteur::write_message(TPCANMsg msg){

    TPCANStatus result;
    char strMsg[256];

    // The message is sent using the PCAN-USB Channel
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
        read_message();
    }

    //En micro seconde :
    usleep(600);
}


//==================POSITION MODE==================



//Init
/**
 * @brief Réinitialise la position absolue du moteur (calibration)
 * 
 * @param id (int) : COB-ID à spécifier
 */
void ControlMoteur::def_positionAbsolue(int id){

    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    // /!\ a faire avec moteurs éteints
    //Modes of operation (Homing Mode)
    msg_data[0] = 0x06;
    init_msg_SDO(&msg, id, W_1B, MODES_OF_OPERATION, 0x00, msg_data);
    write_message(msg);

    //Homing method
    msg_data[0] = 0x25; //=37d
    init_msg_SDO(&msg, id, W_1B, HOMING_METHOD, 0x00, msg_data);
    write_message(msg);

    //Controlword (Shutdown)
    msg_data[0] = 0x00;
    msg_data[1] = 0x06;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    usleep(1000);
    //std::cout << "Mid calibration" << std::endl;
    //Controlword (Switch on & Enable)
    msg_data[0] = 0x00;
    msg_data[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    usleep(1000);

    //Controlword (Start homing)
    msg_data[0] = 0x00;
    msg_data[1] = 0x1F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    //======================================== REVOIR
    //sleep(1);
    usleep(100);
    //======================================== REVOIR
    //std::cout << "Aprés calibration" << std::endl;
    //Controlword (Halt homing)
    msg_data[0] = 0x01;
    msg_data[1] = 0x1F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    //Controlword (shutdown)
    msg_data[0] = 0x00;
    msg_data[1] = 0x06;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);
}

/**
 * @brief Initialisation du mode Position du moteur
 * 
 * @param id (int) : COB-ID à spécifier
 * @return true : n'est jamais renvoyé
 * @return false : la fonction est arrivé à la fin
 */
bool ControlMoteur::init_asservissementPosition(int id){

    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    //---------Modes of operation
    msg_data[0] = 0x01; //profil position mode
    init_msg_SDO(&msg, id, W_1B, MODES_OF_OPERATION, 0x00, msg_data);
    write_message(msg);

    //---------Set parameter : fait avec EPOS STUDIO
    //
    //
    //---------Enable device
    //Controlword (shutdown)
    msg_data[0] = 0x00;
    msg_data[1] = 0x06;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    usleep(1000);

    //Controlword (Switch on and Enable)
    msg_data[0] = 0x00;
    msg_data[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    //Si tout va bien
    return false;
}

/**
 * @brief Déplace relativement le moteur d'un nombre de pas donné
 * 
 * @param id (int) : COB-ID à spécifier
 * @param uInput (int) : nombre de pas de déplacement du moteur
 */
void ControlMoteur::set_relativePosition(int id, int uInput){
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    if(uInput>0xFFFF){ 
        uInput = 0xFFFF;
    }

    printf("uInput n°%d : %hhx\n %d\n",id, uInput, uInput);
    
    //Target position
    msg_data[0] = (uInput>>24)%256;
    msg_data[1] = (uInput>>16)%256;
    msg_data[2] = (uInput>>8)%256;
    msg_data[3] = uInput;

    printf("uInput : %hhx | %hhx\n", msg_data[2], msg_data[3]);
    //ATTENTION : W_4B EST ESSENTIEL
    init_msg_SDO(&msg, id, W_4B, TARGET_POSITION, 0x00, msg_data);
    write_message(msg);

    //Controlword (relative position)
    msg_data[0] = 0;
    msg_data[1] = 0x7F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    //Attente obligé 
    usleep(1000);
    
    //Controlword (New Position)
    msg_data[0] = 0x00;
    msg_data[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);
}


void ControlMoteur::set_absolutePosition(int id, int uInput){

    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    if(uInput>0xFFFF){ 
        uInput = 0xFFFF;
    }

    //printf("uInput n°%d : %hhx\n %d\n",id, uInput, uInput);
    
    //Target position
    msg_data[0] = (uInput>>24)%256;
    msg_data[1] = (uInput>>16)%256;
    msg_data[2] = (uInput>>8)%256;
    msg_data[3] = uInput;

    //printf("uInput : %hhx | %hhx\n", msg_data[2], msg_data[3]);
    //ATTENTION : W_4B EST ESSENTIEL
    init_msg_SDO(&msg, id, W_4B, TARGET_POSITION, 0x00, msg_data);
    write_message(msg);


    //ControlWord (absolute position)
    msg_data[0] = 0x00;
    msg_data[1] = 0x1F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    usleep(1000);

    //Controlword (New Position)
    msg_data[0] = 0x00;
    msg_data[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    //Attente obligé 
}


//Autres 
/**
 * @brief Regarde si le moteur id à atteint sa position demandé et revois status pour le vérifier
 * 
 * @param status (uint8_t*) : Valeur modifiable informant quels moteurs sont arrivé à destination
 * @param id (motId) : id du moteur
 */
void ControlMoteur::checkEndTarget(uint8_t* status, int motId){
    vector<msgRecu> get;
    uint8_t msg_data[4];
    TPCANMsg msg;

    init_msg_SDO(&msg, motId, R, STATUSWORD, 0x00, msg_data);
    get = get_value(msg);

    //printf("Pour la carte N°%d\n", get[0].id);
    for(msgRecu g : get){
        //Bit 10 = Target Reached
        if(g.index == STATUSWORD && ( ((g.valData>>10)%2) == 1)){ //on regarde le bit numero 10
            switch(g.id){
                case 1 :
                    *status = *status | 0b001;
                    break;

                case 2 :
                    *status = *status | 0b010;
                    break;
                
                case 3 :
                    *status = *status | 0b100;
                    break;
                
                default : 
                    std::cout << "Erreur sur le COB-ID\n";
            }
        }
        else{
            //printf("Message en question : %lx\n", g.valData);
        }
    }
}

/**
 * @brief Regarde et attends que toutes les cartes ai fini leurs déplacement
 * 
 */
void ControlMoteur::checkAllEndTarget(){
    uint8_t status = 0;

    //Check si toutes les cartes sont arrivé à destination sinon recheck
    while(status != 0b111){

        // std::cout << "Status : "<< status << std::endl;
        switch(status){
            case 0b000 :
                //Demander à chaques cartes
                checkEndTarget(&status, COBID_CAN1_SDO);
                checkEndTarget(&status, COBID_CAN2_SDO);
                checkEndTarget(&status, COBID_CAN3_SDO);
                //status = status | 0b100;
                break;

            case 0b001 :
                checkEndTarget(&status, COBID_CAN2_SDO);
                checkEndTarget(&status, COBID_CAN3_SDO);
                break;

            case 0b010 :
                checkEndTarget(&status, COBID_CAN1_SDO);
                checkEndTarget(&status, COBID_CAN3_SDO);
                break;

            case 0b100 :
                checkEndTarget(&status, COBID_CAN1_SDO);
                checkEndTarget(&status, COBID_CAN2_SDO);
                break;

            case 0b011 :
                checkEndTarget(&status, COBID_CAN3_SDO);
                break;

            case 0b101 :
                checkEndTarget(&status, COBID_CAN2_SDO);
                break;

            case 0b110 :
                checkEndTarget(&status, COBID_CAN1_SDO);
                break;

            default :
                std::cout << "Erreur dans le recheck des valeurs de STATUSWORD\n";
        }
    }
}

void ControlMoteur::check23EndTarget(){

    uint8_t status = 0;
    //Check si toutes les cartes sont arrivé à destination sinon recheck
    while(status != 0b111){

        // std::cout << "Status : "<< status << std::endl;
        switch(status){
            case 0b000 :
                //Demander à chaques cartes
                // checkEndTarget(&status, COBID_CAN1_SDO);
                // std::cout << "One" << std::endl;
                checkEndTarget(&status, COBID_CAN2_SDO);
                checkEndTarget(&status, COBID_CAN3_SDO);
                status = status | 0b001;
                break;

            case 0b001 :
                checkEndTarget(&status, COBID_CAN2_SDO);
                checkEndTarget(&status, COBID_CAN3_SDO);
                break;

            case 0b010 :
                checkEndTarget(&status, COBID_CAN1_SDO);
                checkEndTarget(&status, COBID_CAN3_SDO);
                break;

            case 0b100 :
                checkEndTarget(&status, COBID_CAN1_SDO);
                checkEndTarget(&status, COBID_CAN2_SDO);
                break;

            case 0b011 :
                checkEndTarget(&status, COBID_CAN3_SDO);
                break;

            case 0b101 :
                checkEndTarget(&status, COBID_CAN2_SDO);
                break;

            case 0b110 :
                checkEndTarget(&status, COBID_CAN1_SDO);
                break;

            default :
                std::cout << "Erreur dans le recheck des valeurs de STATUSWORD\n";
        }
    }
}

//--------------MANUEL---------------
/**
 * @brief Demande a l'utilisateur quelle position pour l'effecteur il souhaite 
 * 
 * @param wantPosX (float) : Position voulue en X 
 * @param wantPosY (float) : Position voulue en Y
 */
void ControlMoteur::get_manualWantedPos(double *wantPosX , double *wantPosY){
    std::cout << "Position effecteur actuelle en X : " << positionX << " et en Y : " << positionY << std::endl;  
    do{
        std::cout << "Donnez une valeur X entre 0 et 800mm" << std::endl;
        std::cin >> *wantPosX;
    }while((0 > *wantPosX || *wantPosX> 800));
    
    do{
        std::cout << "Donnez une valeur Y entre 0 et 600mm" << std::endl;
        std::cin >> *wantPosY;
    }while((0 > *wantPosY || *wantPosX> 600));
}

/**
 * @brief Fonction demandant à l'utilisateur une position et la mettant
 * 
 * @param id (int) : COB-ID à spécifier
 */
void ControlMoteur::set_manualUserPosition(int id){
    uint32_t userInput = 1;
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);
    bool wait= true;
    vector<msgRecu> get;

    wait = init_asservissementPosition(id);
    while(wait);

    while(userInput != 0){
        std::cout << "Donnez une valeur en decimal pour la position (0 pour quitter): \n";
        std::cin >> userInput;
        set_relativePosition(id, userInput);

        //ENDROIT A VERIFIER ==========================================
        init_msg_SDO(&msg, id, R, CONTROLWORD, 0x00, msg_data);
        get = get_value(msg);
        usleep(10);
        //=============================================================
    }

    msg_data[0] = 0x01;
    msg_data[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    std::cout << "Fin du programme : \n";
}

//==================TORQUE MODE==================

/**
 * @brief Initialisation du mode Couple du moteur
 * 
 * @param id (int) : COB-ID à spécifier
 */
bool ControlMoteur::init_asservissementForce(int id){
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    //Modes of operation (Cyclic Synchronous Torque Mode)
    msg_data[0] = 0x0A;
    init_msg_SDO(&msg, id, W_1B, MODES_OF_OPERATION, 0x00, msg_data);
    write_message(msg);

    //Controlword (Shutdown)
    msg_data[0] = 0x00;
    msg_data[1] = 0x06;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    usleep(1000);

    //Controlword (Switch On & Enable)
    msg_data[0] = 0x00;
    msg_data[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    return false;
}

void ControlMoteur::set_force(double force, int id) {
    int target_torque = 1000 * (double)(Model::RAPPORT_REDUCTON * Model::RAYON_ROUE*force)/(double)MOTOR_RATED_TORQUE;
    if (target_torque<0) {
        target_torque+=65536;
    }
    set_torque(target_torque,id);
}


/**
 * @brief Demande à l'utilisateur un Couple et le met sur le moteur
 * 
 * @param id (int) : COB-ID à spécifier
 */
void ControlMoteur::set_manual_torque(int id){

    uint32_t userInput = 1;
    uint32_t userInput2 = 1;
    
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    init_asservissementForce(id);

    printf("\nDonnez une valeur en decimal pour le offset du couple (en pourcent) : \n");
    cin >> userInput;
    

    msg_data[0] = userInput;
    msg_data[1] = userInput>>8;
    init_msg_SDO(&msg, id, W_2B, TORQUE_OFFSET, 0x00, msg_data);
    write_message(msg);

    while(userInput != 0){
        printf("\nDonnez une valeur en decimal pour le couple (en pourcent) \n(0 pour quitter): \n");
        cin >> userInput2;

        msg_data[0] = userInput2>>8;
        msg_data[1] = userInput2;
        init_msg_SDO(&msg, id, W_2B, TARGET_TORQUE, 0x00, msg_data);
        write_message(msg);

        usleep(1000);
    }

    //QuickStop
    msg_data[0] = 0x00;
    msg_data[1] = 0x0B;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    printf("Fin du programme : \n");
}


void ControlMoteur::set_torque(uint16_t userInput, int id){

    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    /*
    msg_data[0] = 0x00;
    msg_data[1] = 0x00;
    init_msg_SDO(&msg, id, W_2B, TORQUE_OFFSET, 0x00, msg_data);
    write_message(msg);*/


    //std::cout << "UserInput : " << userInput << endl; // en pour 1000 , negatif 2**16-nombre
    //printf("userinput : %hhx\n", userInput);
    msg_data[0] = userInput>>8;
    msg_data[1] = userInput;

    //printf("msg_data[0] : %hhx, msg_data[1] : %hhx\n", msg_data[0],msg_data[1]);
    init_msg_SDO(&msg, id, W_2B, TARGET_TORQUE, 0x00, msg_data);
    write_message(msg);
}



//==================ECRITURE/LECTURE FICHIER==================

/**
 * @brief Va lire la valeur de Pos absolue dans le fichier pos_effecteur.txt
 * 
 */
void ControlMoteur::readPos_fichier(){
    string s;
    string delimiter = "|";
    ifstream myfile;
    myfile.open ("pos_effecteur.txt");
    getline(myfile, s);

    size_t pos = 0;
    string token[2];

    pos = s.find(delimiter);
    token[0]= s.substr(0, pos);
    s.erase(0, pos + delimiter.length());
    pos = s.find(delimiter);
    token[1] = s.substr(0, pos);

    positionX = stof(token[0]);
    positionY =stof(token[1]);
}

/**
 * @brief Ecrit la valeur locale de la position de l'effecteur dans le fichier pos_effecteur.txt
 * 
 */
void ControlMoteur::writePos_fichier(){
    ofstream myfile;
    myfile.open ("pos_effecteur.txt");
    myfile << positionX << '|' << positionY << endl;
    myfile.close();
}

//==================QUIT==================

void ControlMoteur::shutdown(int id){
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    //printf("Carte N°%d\n", id);

    usleep(1000);

    //8 -> Shutdown 
    msg_data[0] = 0b0110;
    msg_data[1] = 0b0000;
    
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    usleep(1000);

    /*init_msg_SDO(&msg, id, R, STATUSWORD, 0x00, msg_data);
    print_vectorMessage( get_value(msg) );*/
}

void ControlMoteur::shutdown_all(){
   shutdown(COBID_CAN1_SDO);
   shutdown(COBID_CAN2_SDO);
   shutdown(COBID_CAN3_SDO);
   //cout << "fin du programme" << endl;
}


//==================READ DATA==================

int ControlMoteur::read_torque(int id) {

    uint8_t msg_data[4];
    TPCANMsg msg;
    
    init_msg_SDO(&msg, id, R, ACTUALTORQUE, 0x00, msg_data);
    vector<msgRecu> get = get_value(msg);

    int increment =0;
    for(msgRecu g : get) {
        //Si ce n'est pas un message renvoyé aprés ecriture
        if(!g.isConfirmReception){
            switch(g.taille){
                case R_2B :
                    //2 octets
                    //std::cout << "Lecture du torque : " << g.valData << std::endl;
                    increment=g.valData;
                    break;
                    
                default : 
                    printf("Erreur taille du message : readTorque\n");
            }
            //printf("\n---------------\n");
        }
    }

    //En valeur /1000*Motor rated torque
    return increment;
}

int ControlMoteur::read_velocity(int id) {

    uint8_t msg_data[4];
    TPCANMsg msg;
    
    //Sub index = 0x01
    init_msg_SDO(&msg, id, R, ACTUALVELOCITY, 0x01, msg_data);
    vector<msgRecu> get = get_value(msg);

    int increment =0;
    for(msgRecu g : get) {
        //Si ce n'est pas un message renvoyé aprés ecriture
        if(!g.isConfirmReception){
            switch(g.taille){
                case R_4B :
                    //4 octets
                    //std::cout << "Lecture de la position : " << std::endl;
                    increment=g.valData;
                    break;
                    
                default : 
                    printf("Erreur taille du message : readVelocity\n");
            }
            //printf("\n---------------\n");
        }
    }

    //En rpm
    return increment;
}

/**
 * 
*/
int ControlMoteur::read_position(int id) {

    uint8_t msg_data[4];
    TPCANMsg msg;
    
    init_msg_SDO(&msg, id, R, ACTUALPOSITION, 0x00, msg_data);
    vector<msgRecu> get = get_value(msg);

    int increment =0;
    for(msgRecu g : get) {
        //Si ce n'est pas un message renvoyé aprés ecriture
        if(!g.isConfirmReception){
            switch(g.taille){
                case R_4B :
                    //4 octets
                    // std::cout << "Lecture de la position : " << g.valData << std::endl;
                    increment=g.valData;
                    break;
                    
                default : 
                    printf("Erreur taille du message : readPosition\n");
            }
            //printf("\n---------------\n");
        }
    }
    
    double position = increment;

    //std::cout << "Position de " << id << " est " << position << std::endl;
    return position;
}

/**
 * @brief Va chercher et renvoie la valeur de l'index demandé
 * 
 * @param toSend (TPCANMsg) : Valeur de l'index 
 * @return vector<msgRecu> : Array des messages lus 
 */
vector<msgRecu> ControlMoteur::get_value(TPCANMsg toSend){
    //----------------------------------
    //DATA[0] Toujours égale à 0x40 si on veut read une valeur
    toSend.DATA[0] = R;
    write_message(toSend);

    //-------------------------------------------
    usleep(1000);  //En micro-secondes
    //-------------------------------------------

    return (read_message());
}


//==================HOMING MODE==================


bool ControlMoteur::init_homingMode(int id){
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    //Modes of operation (Homing Mode)
    msg_data[0] = 0x06;
    init_msg_SDO(&msg, id, W_1B, MODES_OF_OPERATION, 0x00, msg_data);
    write_message(msg);

    //Mettre config ici 


    //Select homing method
    msg_data[0] = 0x01;
    init_msg_SDO(&msg, id, W_1B, HOMING_METHOD, 0x00, msg_data);
    write_message(msg);


    //Controlword (Shutdown)
    msg_data[0] = 0x00;
    msg_data[1] = 0x06;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    usleep(1000);

    //Controlword (Switch On & Enable)
    msg_data[0] = 0x00;
    msg_data[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    usleep(1000);

    //ControlWord (Start Homing)
    msg_data[0] = 0x00;
    msg_data[1] = 0x1F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    return false;
}


void ControlMoteur::readEndHoming(uint8_t *status, int id){
    vector<msgRecu> get;
    uint8_t msg_data[4];
    TPCANMsg msg;

    init_msg_SDO(&msg, id, R, STATUSWORD, 0x00, msg_data);
    get = get_value(msg);

    //printf("Pour la carte N°%d\n", get[0].id);
    for(msgRecu g : get){
        //Bit 12 = Homing attained
        // std::cout << "StatusWord : " <<  ((g.valData)) << std::endl;
        if(g.index == STATUSWORD && ( ((g.valData>>12)%2) == 1)){ //on regarde le bit numero 12
                switch(g.id){
                    case 1 :
                        *status = *status | 0b001;
                        break;

                    case 2 :
                        *status = *status | 0b010;
                        break;
                    
                    case 3 :
                        *status = *status | 0b100;
                        break;
                    
                    default : 
                        std::cout << "Erreur sur le COB-ID\n";
                }
        }
        else{
            //printf("Message en question : %lx\n", g.valData);
        }
    }
    // usleep(1000);
}

void ControlMoteur::readAllEndHoming(){
    uint8_t status = 0;

    //Check si toutes les cartes sont arrivé à destination sinon recheck
    while(status != 0b111){
        //std::cout << "Status : "<< status << std::endl;
        switch(status){
            case 0b000 :
                //Demander à chaques cartes
                readEndHoming(&status, COBID_CAN1_SDO);
                readEndHoming(&status, COBID_CAN2_SDO);
                readEndHoming(&status, COBID_CAN3_SDO);
                //status = status | 0b100;
                break;

            case 0b001 :
                readEndHoming(&status, COBID_CAN2_SDO);
                readEndHoming(&status, COBID_CAN3_SDO);
                break;

            case 0b010 :
                readEndHoming(&status, COBID_CAN1_SDO);
                readEndHoming(&status, COBID_CAN3_SDO);
                break;

            case 0b100 :
                readEndHoming(&status, COBID_CAN1_SDO);
                readEndHoming(&status, COBID_CAN2_SDO);
                break;

            case 0b011 :
                readEndHoming(&status, COBID_CAN3_SDO);
                break;

            case 0b101 :
                readEndHoming(&status, COBID_CAN2_SDO);
                break;

            case 0b110 :
                readEndHoming(&status, COBID_CAN1_SDO);
                break;

            default :
                std::cout << "Erreur dans le recheck des valeurs de STATUSWORD\n";
        }
    }
}

void ControlMoteur::startHoming(int id){
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    // Modes of operation 
    msg_data[0] = 0x06; // homing mode
    init_msg_SDO(&msg, id, W_1B, MODES_OF_OPERATION, 0x00, msg_data);
    write_message(msg);
    usleep(1000000);
    // printf("Modes of operation\n ");

    // Speed for switch search (to the mechanical end stop)
    msg_data[0] = 0x00; // 100 rpm
    msg_data[1] = 0x10; 
    msg_data[2] = 0x00; 
    msg_data[3] = 0x00; 
    init_msg_SDO(&msg, id, W_4B, 0x6099, 0x01, msg_data);
    // write_message(msg);

    // printf("debut Current threshold for homing mode\n");
    // Current threshold for homing mode
    msg_data[0] = 0x03; // = 1010 (mA) = 50% Nominal current
    msg_data[1] = 0xED;
    init_msg_SDO(&msg, id, W_2B, 0x30B2, 0x00, msg_data);
    write_message(msg);

    // printf("debut Current threshold for homing mode\n");

    // Homing method 
    // printf("debut Homing method \n");
    msg_data[0] = 0xFF; // -1 = "Current Threshold Positive Speed & Index"
    init_msg_SDO(&msg, id, W_1B, 0x6098, 0x00, msg_data);
    write_message(msg);
    // printf("fin Homing method \n");

    usleep(10000);

    //Controlword (shutdown)
    msg_data[0] = 0x00;
    msg_data[1] = 0x06;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    // printf("Controlword (shutdown)\n");

    usleep(100000);

    //Controlword (Switch on and Enable)
    msg_data[0] = 0x00;
    msg_data[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    // printf("Controlword (Switch on and Enable)\n");

    usleep(100000);

    //Controlword (Start homing)
    msg_data[0] = 0x00;
    msg_data[1] = 0x1F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);
    // printf("Controlword (Start homing)\n");

}

//==================PARTIE GRAPHIQUE==================

ControlMoteur::ControlMoteur() {
    if (nombreInstance>0) {
        throw std::runtime_error("ControlMoteur::ControlMoteur() -> impossible d'instancier plus d'une fois un ControlMoteur\n");
    }
    nombreInstance++;


    asservissement = POSITION;
    powerOn = false;

    // [!] POUR TESTS :
    this->forceX = 15;
    this->forceY = -9.6;

    this->positionX = 0;
    this->positionY = 0;

    this->raideur = 0.1; // [!]
    this->viscosite = 0.1; //[!]

    //Initialisation du PEAK
    initialise_CAN_USB();
    
    enDeplacement = false;

    controlMoteurThread = new std::thread(&ControlMoteur::runControlMoteur,this);
}


void ControlMoteur::runControlMoteur() {
    // FAIRE ICI L'ASSERVISSEMENT ~100 Hz ?
    // actualiser dans l'asservissement forceX, forceY, positionX, positionY
    while (true)
    {
        switch(asservissement){
            case POSITION :
                updateValeurs();
                if(enDeplacement){
                    // std::cout << "Debug: runControlMoteur" << std::endl;
                    control_allPosition(wantedPositionX, wantedPositionY);
                }
                break;
            
            case HAPTIC :
                updateValeurs();
                control_haptique();
                break;

            case ETALONNAGE : 
                // std::cout << "Debug: Etalonnage" << std::endl;
                readAllEndHoming(); 
                
                //On eteint tous les moteurs et on met à jours toutes les valeurs 
                shutdown_all();


                setAllAbsolutePosition();
                asservissement = POSITION;
                break;

            default : 
                break;
        }
        //printf("Debug : ControlMoteur\n");
    }
} 

void ControlMoteur::waitEnd() {
    controlMoteurThread->join();
}

void ControlMoteur::getForce(double &forceX, double &forceY) {
    //std::cout << "ControlMoteur::getForce" << std::endl;
    forceX = this->forceX;
    forceY = this->forceY;
}

void ControlMoteur::getCoupleMoteur(double coupleMoteur[3]){
    coupleMoteur[0] = this->torque[0];
    coupleMoteur[1] = this->torque[1];
    coupleMoteur[2] = this->torque[2];
}

void ControlMoteur::changeAsservissement(){
    bool precPower = powerOn;

    setPowerToOff();
    //On change le mode
    if (asservissement==POSITION) {

        
        asservissement=HAPTIC;
    } else {
        asservissement=POSITION;
    }
    setPowerToOn();
}

void ControlMoteur::getPosition(double &positionX, double &positionY) {
    //std::cout << "ControlMoteur::getPosition" << std::endl;
    positionX = this->positionX;
    positionY = this->positionY;
}

//==============ETEINDRE/ALUMER MOTEURS=================== 

void ControlMoteur::changePower() {
    if (powerOn) {
        setPowerToOff();

    } else {
        setPowerToOn();
    }
}

void ControlMoteur::setPowerToOff() {
    powerOn=false;
    wantedPositionX = positionX;
    wantedPositionY = positionY;

    shutdown_all();
    printf("ControlMoteur : moteur eteint\n");
}

void ControlMoteur::setPowerToOn() {
    bool wait = true;
    powerOn=true;

    switch(asservissement){
        case POSITION :
            //Mode position
            init_asservissementPosition(COBID_CAN1_SDO);
            init_asservissementPosition(COBID_CAN2_SDO);
            wait = init_asservissementPosition(COBID_CAN3_SDO);
            while(wait);
            break;

        case HAPTIC :
            //Mode force 
            init_asservissementForce(COBID_CAN1_SDO);
            init_asservissementForce(COBID_CAN2_SDO);
            wait = init_asservissementForce(COBID_CAN3_SDO);
            while(wait);
            usleep(1000);
            break;

        default:
            break;
    }

    printf("ControlMoteur : moteur allume\n");
}


//==============UTILISER LES MOTEURS=================== 

void ControlMoteur::goTo(double positionX,double positionY) {
    wantedPositionX = positionX;
    wantedPositionY = positionY;

    if (asservissement==POSITION) {
        // printf("ControlMoteur::Debug : va vers la position %lf mm %lf mm\n",positionX,positionY);
        enDeplacement = true;
    } else {
        printf("[!] impossible d'aller a une position en mode haptique\n");
    }
}

void ControlMoteur::setVitesse(double vitesse) {
    printf("ControlMoteur::Debug : vitesse affecte a %lf mm/s\n",vitesse);
    this->vitesse = vitesse;
}


void ControlMoteur::setRaideur(double raideur) {
    // printf("ControlMoteur::Debug : raideur affecte a %lf N/mm\n",raideur);
    this->raideur = raideur;
}


void ControlMoteur::setViscosite(double viscosite) {
    // printf("ControlMoteur::Debug : viscosite affecte a %lf kg/s\n",viscosite); 
    this->viscosite = viscosite;
}

// /!\ A VERIFIER
void ControlMoteur::reset() {
    //Etalonner
    // printf("ControlMoteur::Debug : reset\n");
    asservissement = NONE;

    startHoming(COBID_CAN1_SDO);
    asservissement = ETALONNAGE;
    startHoming(COBID_CAN2_SDO);
    startHoming(COBID_CAN3_SDO);

}

//
void ControlMoteur::disco() {
    // printf("ControlMoteur::Debug : disco\n"); 
    // init_asservissementPosition(COBID_CAN1_SDO);
    // init_asservissementPosition(COBID_CAN2_SDO);
    // init_asservissementPosition(COBID_CAN3_SDO);
    // usleep(100000);

    // set_relativePosition(COBID_CAN1_SDO, 19000);
    // set_relativePosition(COBID_CAN2_SDO, 3000);
    // set_relativePosition(COBID_CAN3_SDO, 3000);

    // checkAllEndTarget();
    // setAllAbsolutePosition();
    // usleep(10000);
}

// X : 414 [] Y : 298 
void ControlMoteur::techno() {
    // printf("ControlMoteur::Debug : techno\n"); 
    // init_asservissementPosition(COBID_CAN1_SDO);
    // init_asservissementPosition(COBID_CAN2_SDO);
    // init_asservissementPosition(COBID_CAN3_SDO);

    // usleep(100000);

    // set_absolutePosition(COBID_CAN1_SDO, 0);
    // set_absolutePosition(COBID_CAN2_SDO, 0);
    // set_absolutePosition(COBID_CAN3_SDO, 0);
}

//==================FONCTIONS AVANCEE==================


//Main du position Mode
/**
 * @brief envoyer la position de l'effecteur a la position voulu
 * 
 * @param nb_points (int) : nombre de points voulu entre la position actuelle et l'arrivée
 */
void ControlMoteur::control_allPosition(double wantPosX, double wantPosY){

    float deplacementIncrX, deplacementIncrY;
    int nb_points;
    int val_motor1 = 0; 
    int val_motor2 = 0;
    int val_motor3 = 0;

    double read_value = 0;
    // std::cout << "Start Control all position" << std::endl;

    //On recupére la valeur en points de chaque incrémentation en mm
    int ptsDeplacementX = abs(wantPosX-positionX)*vitesse;
    int ptsDeplacementY = abs(wantPosY-positionY)*vitesse;

    //On trouve quel est le deplacement le plus long selon X ou Y
    if(ptsDeplacementX>ptsDeplacementY){
        nb_points = (ptsDeplacementX);
    }
    else{
        nb_points = (ptsDeplacementY);
    }
     
    // printf("MAX POINTS : %d points\n", nb_points);
    // printf("pts X : %d et pts Y : %d\n", ptsDeplacementX, ptsDeplacementY);
    
    //Calcul du deplacement à chaque incrémentation [mm]
    // deplacementIncrX = ptsDeplacementX*MM_PAR_POINTS/nb_points;
    // deplacementIncrY = ptsDeplacementY*MM_PAR_POINTS/nb_points;


    deplacementIncrX = (wantPosX-positionX)/nb_points;
    deplacementIncrY = (wantPosY-positionY)/nb_points;

    //Valeur de base de la position de l'effecteur
    wantPosX = positionX;
    wantPosY = positionY;

    //std::cout<< "deplacementX " << deplacementIncrX << " et Y : " << deplacementIncrY << std::endl;
    // printf("En %d points\n", nb_points);
    
    for(int i=0; i<nb_points; i++){
            updateValeurs();
        //On incrémente en fonction du nombre de points
        // if(i<= abs(ptsDeplacementX)){
            wantPosX += deplacementIncrX;
        //}

        // if(i<=abs(ptsDeplacementY)){
            wantPosY +=deplacementIncrY;
        //}
        
        //Calcul des positions voulue avec wantPosX et wantPosY :
        
        //===========================================================
        // std::cout<< "wantX : " << wantPosX << " et Y : " << wantPosY << std::endl;
        Model::increment_moteur_from_pos(wantPosX, wantPosY, &val_motor1, &val_motor2, &val_motor3);
        // std::cout<<"mot1 : " << val_motor1 <<" mot2 : " << val_motor2 << " et 3 : " << val_motor3 << std::endl;
        //===========================================================
        //Envoi les données dans les moteurs
        
        set_absolutePosition(COBID_CAN1_SDO, val_motor1);
        set_absolutePosition(COBID_CAN2_SDO, val_motor2);
        set_absolutePosition(COBID_CAN3_SDO, val_motor3);


        //================================ 
        //Attente que tous les moteurs soit arrivé 
        checkAllEndTarget();
        // usleep(1000);

        //================================ 
        read_value = read_position(COBID_CAN1_SDO);
        // std::cout << "Valeur atteinte : " << read_value << std::endl;
        
        //On met à jour la position de l'effecteur
        positionX = wantPosX;
        positionY = wantPosY;
    }

    enDeplacement = false;
}

void ControlMoteur::miseDeplacementManuelForce(){
    bool wait = true;
    shutdown_all();

    init_asservissementForce(COBID_CAN1_SDO);
    init_asservissementForce(COBID_CAN2_SDO);
    wait = init_asservissementForce(COBID_CAN3_SDO);

    usleep(100000);

    set_torque(210, COBID_CAN1_SDO);
    set_torque(210, COBID_CAN2_SDO);
    set_torque(210, COBID_CAN3_SDO);
}

void ControlMoteur::mise_en_position0_effecteur(){


    uint8_t status = 0;
    bool wait = true;
    powerOn = false;

    miseDeplacementManuelForce();
    usleep(500000);

    init_asservissementForce(COBID_CAN1_SDO);
    init_asservissementPosition(COBID_CAN2_SDO);
    wait = init_asservissementPosition(COBID_CAN3_SDO);
    while(wait);

    usleep(100000);

    set_relativePosition(COBID_CAN2_SDO, -6400);
    set_relativePosition(COBID_CAN3_SDO, -6400);
    // std::cout << "Checkpoint1" << std::endl;
    check23EndTarget();
    // std::cout << "Checkpoint2" << std::endl;
    // sleep(1);

    //On tire vers le haut l'effecteur
    shutdown(COBID_CAN2_SDO);
    shutdown(COBID_CAN3_SDO);

    usleep(100000);
    
    set_torque(350, COBID_CAN1_SDO);

    usleep(1500000);

    init_asservissementPosition(COBID_CAN1_SDO);
    usleep(100000);

    set_relativePosition(COBID_CAN1_SDO, -32000);

    do{
        checkEndTarget(&status, COBID_CAN1_SDO);
    }while(status!=0b001);
    
    std::cout << "Checkpoint" << std::endl;

    // //CHECKPOINT : L'effecteur est en haut 

    init_asservissementPosition(COBID_CAN2_SDO);
    wait = init_asservissementPosition(COBID_CAN3_SDO);
    while(wait);

    usleep(100000);

    set_relativePosition(COBID_CAN2_SDO, 20000);
    set_relativePosition(COBID_CAN3_SDO, 20000);

    check23EndTarget();
    
    setAllAbsolutePosition();

}


void ControlMoteur::control_haptique(){
    double couple_moteur[3];
    findEffectorSpeed(couple_moteur);
    //std::cout << "Couple 1 : " << couple_moteur[0] <<  " Couple 2 : " << couple_moteur[2] << " Couple 3 : " << couple_moteur[2] << std::endl;

    set_torque(couple_moteur[0], COBID_CAN1_SDO);
    set_torque(couple_moteur[1], COBID_CAN2_SDO);
    set_torque(couple_moteur[2], COBID_CAN3_SDO);
    
}


void ControlMoteur::findEffectorSpeed(double couple_moteur[3]){
    double position[2] = {positionX, positionY}; 

    //Mise a jour des incréments des moteurs
    increment[0] = read_position(COBID_CAN1_SDO);
    increment[1] = read_position(COBID_CAN2_SDO);
    increment[2] = read_position(COBID_CAN3_SDO);

    //Mise a jour de la position de l'effecteur
    Model::incrementToPostion(increment[0], increment[1], increment[2], positionX, positionY);

    // u_int8_t x4
    printf("increments : %d %d %d",increment[0],increment[1],increment[2]);

    
    Model::coupleMoteurAsservissemnt(increment, velocity, raideur, viscosite, couple_moteur);

    printf("torque : %.2f %.2f %.2f\n",couple_moteur[0],couple_moteur[1],couple_moteur[2]);

    couple_moteur[0] = Model::torque2TargetTorque(couple_moteur[0]);
    couple_moteur[1] = Model::torque2TargetTorque(couple_moteur[1]);
    couple_moteur[2] = Model::torque2TargetTorque(couple_moteur[2]);
}

void ControlMoteur::updateValeurs(){
    double position[2] = {positionX, positionY}; 
    
    //Mise a jour de la vitesse des moteurs
    velocity[0] = read_velocity(COBID_CAN1_SDO)*0;
    velocity[1] = read_velocity(COBID_CAN2_SDO)*0;
    velocity[2] = read_velocity(COBID_CAN3_SDO)*0;



    //Mise a jour de la couple des moteurs
    torque[0] = ((double)Int16ToSingnedInt(read_torque(COBID_CAN1_SDO)))/1000*Model::NOMINAL_TORQUE;
    torque[1] = ((double)Int16ToSingnedInt(read_torque(COBID_CAN2_SDO)))/1000*Model::NOMINAL_TORQUE;
    torque[2] = ((double)Int16ToSingnedInt(read_torque(COBID_CAN3_SDO)))/1000*Model::NOMINAL_TORQUE;
    


    //Mise a jour de la vitesse de l'effecteur 
    Model::vitesseMoteur2effecteur(velocity, position, vitesseEffecteur);

    double position_effecteur[2];
    position_effecteur[0] = positionX;
    position_effecteur[1] = positionY;

    
    //Mise a jour de la force de l'effecteur
    double forceXY[2]; 
    Model::torqueMoteur2ForceXY(torque,position_effecteur,forceXY);
    forceX = forceXY[0];
    forceY = forceXY[1];
}


void ControlMoteur::setAllAbsolutePosition(){

    def_positionAbsolue(COBID_CAN1_SDO);
    usleep(1000);
    def_positionAbsolue(COBID_CAN2_SDO);
    usleep(1000);
    def_positionAbsolue(COBID_CAN3_SDO);

    usleep(100000);


    //Mettre les positions ici :
    positionX = DEFAULTPOSITIONX;
    positionY = DEFAULTPOSITIONY;

    //Eteindre les moteurs pour eviter tous problémes
    shutdown_all();
    powerOn = false;

    asservissement = POSITION;
}

int ControlMoteur::Int16ToSingnedInt(uint16_t value) {
    // sortie [−32768, 32767]
    int sortie=value;
    if (sortie>32767) {
        sortie-=65536;
    }
    return sortie;
}
