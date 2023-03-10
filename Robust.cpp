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

#include "Robust.h"

//Valeur de base de la sortie des messages CanOpen
TPCANHandle channelUsed = PCAN_USBBUS1;
TPCANBaudrate baudrateUsed = PCAN_BAUD_1M;

//Position de l'effecteur (absolue)
float abs_posX, abs_posY;

//================INITIALISATION_PEAK================

/**
 * @brief Va chercher un channel disponible du PEAK automatiquement
 * 
 * @return TPCANHandle : Channel trouvé
 */
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


/**
 * @brief Va chercher le baudrate de la liaison CANOpen automatiquement 
 * 
 * @return TPCANBaudrate : Baudrate trouvé
 */
TPCANBaudrate init_baudrate_doc(){

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
void initialise_CAN_USB(){
    
    char strMsg[256];
    TPCANStatus result;
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
void init_msg_SDO(TPCANMsg* msg, int id, uint8_t data_length, uint16_t index, uint8_t subIndex, uint8_t data[4]){
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
void print_message(TPCANMsg received){

    uint32_t result = -1;

    //Si ce n'est pas un message renvoyé aprés ecriture
    if(received.DATA[0]!=0x60){
        printf("\n+++++++++++++++++\n");
        printf("Index : 0x%hhx-%hhx | Sub-Index : 0x%hhx\n", received.DATA[2], received.DATA[1], received.DATA[3]);

        switch(received.DATA[0]){
            case R_1B :
                //1 octet
                result = received.DATA[4];
                printf("Data hexa : 0x%hhx\n",received.DATA[4]);
                printf("Data deci : %d", result);
                break;

            case R_2B :
                //2 octets
                result = (received.DATA[5]<<8) | received.DATA[4];
                printf("Data hexa : 0x%hhx | %hhx\n",received.DATA[5],received.DATA[4]);
                printf("Data deci : %d", result);
                break;

            case R_4B :
                //4 octets
                result = (received.DATA[7]<<24) | (received.DATA[6]<<16)| (received.DATA[5]<<8) | received.DATA[4];
                printf("Data hexa : 0x%hhx | %hhx | %hhx | %hhx\n",received.DATA[7],received.DATA[6],received.DATA[5],received.DATA[4]);
                printf("Data decimal : %d", result); 
                break;
                
            default : 
                printf("Erreur taille du message : 0x%hhx\n", received.DATA[0]);
        }
        printf("\n+++++++++++++++++\n");
            
    }
    else{
        printf("Message confirmation de Index : 0x%hhx-%hhx | Sub-Index : 0x%hhx\n", received.DATA[2], received.DATA[1], received.DATA[3]);
        //printf("Data : %hhx | %hhx | %hhx | %hhx", received.DATA[7], received.DATA[6], received.DATA[5], received.DATA[4]);
    }
}


/**
 * @brief Met le message en entrée dans une struct msgRecu
 * 
 * @param received (TPCANMsg) : Message à transcrire
 * @return msgRecu : Message transcrit
 */
msgRecu get_message(TPCANMsg received){

    uint32_t result = -1;
    msgRecu msgRecup;

    msgRecup.id = received.ID % 16;
    msgRecup.index = 8 << received.DATA[2] && received.DATA[1];
    msgRecup.subIndex = received.DATA[3];
    
    //Si ce n'est pas un message renvoyé aprés ecriture
    if(received.DATA[0]!=0x60){
        switch(received.DATA[0]){
            case R_1B :
                //1 octet
                result = received.DATA[4];
                break;

            case R_2B :
                //2 octets
                result = (received.DATA[5]<<8) | received.DATA[4];
                break;

            case R_4B :
                //4 octets
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
vector<msgRecu> read_message(){

    TPCANMsg received;
    TPCANStatus result;
    TPCANTimestamp timestamp;
    char strMsg[256];

    uint8_t i=0;
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
            print_message(received);
            get.push_back(get_message(received));
            //printf("Le message est arrivé en %dms\n", timestamp.millis);
            //printf("msg type : %hhx\n", received.MSGTYPE);
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
void write_message(TPCANMsg msg){

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

/**
 * @brief Va chercher et renvoie la valeur de l'index demandé
 * 
 * @param toSend (TPCANMsg) : Valeur de l'index 
 * @return vector<msgRecu> : Array des messages lus 
 */
vector<msgRecu> get_value(TPCANMsg toSend){
    //----------------------------------
    //DATA[0] Toujours égale à 0x40 si on veut read une valeur
    toSend.DATA[0] = R;
    write_message(toSend);

    //-------------------------------------------
    usleep(1000);  //En micro-secondes
    //-------------------------------------------

    return (read_message());
}



//==================POSITION MODE==================

/**
 * @brief Réinitialise la position absolue du moteur (calibration)
 * 
 * @param id (int) : COB-ID à spécifier
 */
void def_positionAbsolue(int id){
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

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

    //Controlword (Halt homing)
    msg_data[0] = 0x01;
    msg_data[1] = 0x1F;
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
bool init_asservissementPosition(int id){

    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    def_positionAbsolue(id);

    //---------Modes of operation
    msg_data[0] = 0x01; //profil position mode
    init_msg_SDO(&msg, id, W_1B, MODES_OF_OPERATION, 0x00, msg_data);
    write_message(msg);

    //---------Set parameter : fait avec EPOSSTUDIO
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

    return false;
}

/**
 * @brief Déplace relativement le moteur d'un nombre de pas donné
 * 
 * @param id (int) : COB-ID à spécifier
 * @param uInput (int) : nombre de pas de déplacement du moteur
 */
void set_relativePosition(int id, int uInput){
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    if(uInput>0xFFFF){ 
        uInput = 0xFFFF;
    }

    printf("uInput : %hhx\n %d\n", uInput, uInput);
    
    //Target position
    //On limite à 2 Bytes
    msg_data[0] = 0x00;
    msg_data[1] = 0x00;
    msg_data[2] = 0x0;//(uInput>>8)%256;
    msg_data[3] = uInput;

    printf("uInput : %hhx | %hhx\n", msg_data[2], msg_data[3]);
    //ATTENTION : W_4B EST ESSENTIEL
    init_msg_SDO(&msg, id, W_4B, TARGET_POSITION, 0x00, msg_data);
    write_message(msg);

    //Controlword (New Position)
    msg_data[0] = 0x00;
    msg_data[1] = 0x0F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    //Attente obligé 
    usleep(1000);

    //Controlword (relative position)
    msg_data[0] = 0;
    msg_data[1] = 0x5F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);

    /*//ABSOLUE
    msg_data[0] = 0x00;
    msg_data[1] = 0x1F;
    init_msg_SDO(&msg, id, W_2B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);*/

}


/**
 * @brief Fonction contrôlant toutes les EPOS4 en fonction de leur position relative
 * 
 * @param nb_points (int) : nombre de points voulu entre la position actuelle et l'arrivée
 */
void control_allPosition(int nb_points){
    float wantPosX, wantPosY;
    float deplacementX, deplacementY;
    int val_motor1, val_motor2, val_motor3;
    uint8_t quit = 1;

    do{
        //On va chercher la valeur voulue
        get_manualWantedPos(&wantPosX, &wantPosY);

        //On recupére la valeur de chaque incrémentation
        deplacementX = (wantPosX-abs_posX)/nb_points;
        deplacementY = (wantPosY-abs_posY)/nb_points;

        wantPosX = abs_posX;
        wantPosY = abs_posY;

        for(int i=0; i<nb_points; i++){
            //On incrémente en fonction du nombre de points
            wantPosX += deplacementX;
            wantPosY +=deplacementY;
            
            //Calcul des positions voulue avec wantPosX et wantPosY :
            
            //===========================================A FAIRE

            //Envoie des données dans les moteurs
            set_relativePosition(COBID_CAN1_SDO, val_motor1);
            set_relativePosition(COBID_CAN2_SDO, val_motor2);
            set_relativePosition(COBID_CAN3_SDO, val_motor3);
            
            //Attente que tous les moteurs soit arrivé 
            checkAllEndTarget();
            
            //On met à jour la position de l'effecteur
            abs_posX = wantPosX;
            abs_posY = wantPosY;
        }
        cout << "0. Pour quitter";
        cin >> quit;
    }while(quit!= 0);
}


/**
 * @brief Regarde si le moteur id à atteint sa position demandé et revois status pour le vérifier
 * 
 * @param status (uint8_t*) : Valeur modifiable informant quels moteurs sont arrivé à destination
 * @param id (motId) : id du moteur
 */
void checkEndTarget(uint8_t* status, int motId){
    vector<msgRecu> get;
    uint8_t msg_data[4];
    TPCANMsg msg;

    init_msg_SDO(&msg, motId, R, STATUSWORD, 0x00, msg_data);
    get = get_value(msg);

    for(msgRecu g : get){
        //Bit 10 = Target Reached
        if(g.index == STATUSWORD && ((g.valData>>9 & 0b1)==1)){
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
                    cout << "Erreur sur le COB-ID\n";
            }
        }
    }
}

/**
 * @brief Regarde et attends que toutes les cartes ai fini leurs déplacement
 * 
 */
void checkAllEndTarget(){
    uint8_t status = 0;
    
    
    //Demander à chaques cartes
    checkEndTarget(&status, COBID_ALL_CAN_SDO);
    
    //Check si toutes les cartes sont arrivé à destination sinon recheck
    while(status != 0b111){
        switch(status){
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
                cout << "Erreur dans le recheck des valeurs de STATUSWORD\n";
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
void get_manualWantedPos(float *wantPosX , float *wantPosY){
    cout << "Effecteur au niveau de X : " << abs_posX << " et Y : " << abs_posY <<"\n";  
    do{
        cout << "Donnez une valeur X entre 0 et 400mm\n";
        cin >> *wantPosX;
    }while(!(0 > *wantPosX > 400));

    do{
        cout << "Donnez une valeur Y entre 0 et 240mm\n";
        cin >> *wantPosY;
    }while(!(0 > *wantPosY > 240));
}

/**
 * @brief Fonction demandant à l'utilisateur une position et la mettant
 * 
 * @param id (int) : COB-ID à spécifier
 */
void set_manualUserPosition(int id){
    uint32_t userInput = 1;
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);
    bool wait= true;
    vector<msgRecu> get;

    wait = init_asservissementPosition(id);
    while(wait);

    while(userInput != 0){
        cout << "Donnez une valeur en decimal pour la position (0 pour quitter): \n";
        cin >> userInput;
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

    cout << "Fin du programme : \n";
}

//==================TORQUE MODE==================

/**
 * @brief Initialisation du mode Couple du moteur
 * 
 * @param id (int) : COB-ID à spécifier
 */
void init_Torque(int id){
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
    init_msg_SDO(&msg, id, W_1B, CONTROLWORD, 0x00, msg_data);
    write_message(msg);
}


/**
 * @brief Demande à l'utilisateur un Couple et le met sur le moteur
 * 
 * @param id (int) : COB-ID à spécifier
 */
void set_torque(int id){

    uint32_t userInput = 1;
    uint32_t userInput2 = 1;
    
    TPCANMsg msg;
    uint8_t msg_data[4];
    bzero(msg_data, 4);

    init_Torque(id);

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



//==================MENU==================

/**
 * @brief Menu de selection des différents modes du programme
 * 
 * @param id (int) : COB-ID à spécifier
 */
void mode_selection(int id){
    int userInput = 8;
    bool wait;
    do{
        printf("Quel mode voulez vous lancer ?\n 0.Quitter\n 1.Controle en Position\n 2.Controle en Couple\n 3.Controle en Vitesse\n");
        cin >> userInput;
    }while(userInput>3);

    switch (userInput){
        case 0 : 
            printf("Quitte\n"); 
            break;

        case 1 : 
            //Initialisation de tous les cartes EPOS
            wait = init_asservissementPosition(COBID_ALL_CAN_SDO);
            while(wait);
            //Control en position de toutes les cartes EPOS
            control_allPosition(100);
            break;

        case 2 : 
            set_torque(id);
            break;

        case 3 :
            printf("Pas encore implémenté\n");
            break;

        default : 
            printf("Valeur invalide\n");
    }
}

//==================AUTRES==================

/**
 * @brief Va lire la valeur de Pos absolue dans le fichier pos_effecteur.txt
 * 
 */
void readPos(){
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

    abs_posX = stof(token[0]);
    abs_posY =stof(token[1]);
}

/**
 * @brief Ecrit la valeur locale de la position de l'effecteur dans le fichier pos_effecteur.txt
 * 
 */
void writePos(){
    ofstream myfile;
    myfile.open ("pos_effecteur.txt");
    myfile << abs_posX << '|' << abs_posY << endl;
    myfile.close();
}

//==================MAIN==================
int main(){
    //Initialisation du PEAK
    initialise_CAN_USB();

    readPos();
    mode_selection(COBID_CAN3_SDO);
    writePos();
}
