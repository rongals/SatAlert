/*
 ============================================================================
 Name        : SatAlert.c
 Author      : Luca Simone Ronga
 Version     :
 Copyright   : (c) Luca 2016
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "sae.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


////#define FIRST_UPLOAD 1 // 0= non-first generated message, 1= first generated message (no stored message)
#define STAND_VERSION 1
////#define MSG_TYPE 0 //0=ultrashort, 1=mames update, 2=cancel, 3=mames ack, 4=mames alert
////#define MSG_NOCAP 0 // 0=the payload does NOT encapsulate a CAP message, 1= the payload DOES encapsulate a CAP message
#define ULTRASHORT_MSG_TYPE 0 //0=ultrashort, 1=mames update, 2=cancel, 3=mames ack, 4=mames alert // to be removed
#define CANCEL_MSG_TYPE 2 // to be removed
#define ALERT_MSG_TYPE 4 // to be removed
#define MESSAGE_ID 12345
////#define MAMES_PROVIDER_ID 1
////#define ALERT_ISSUER_ID 3
////#define PRIORITY 1
#define NOTIFICATION_AREA1 9809478 //100101011010111001000110
#define NOTIFICATION_AREA2 1457399 //000101100011110011110111
////#define VALIDITY_START 1460642840 // in seconds
////#define VALIDITY_END 1463234801 // in seconds
#define VALIDITY_START_YEAR 14 //001110
#define VALIDITY_START_MONTH 10 //1010
#define VALIDITY_START_DAY 23 //10111
#define VALIDITY_START_HOUR 12 //01100
#define VALIDITY_START_MIN 12 //001100
#define VALIDITY_START_SEC 12 //001100
#define VALIDITY_END_YEAR 14 //001110
#define VALIDITY_END_MONTH 10 //1010
#define VALIDITY_END_DAY 31 //10111
#define VALIDITY_END_HOUR 12 //01100
#define VALIDITY_END_MIN 12 //001100
#define VALIDITY_END_SEC 12 //001100
#define NOHEADER_NHT 0
#define VALIDITY_NHT 1
#define CAP_NHT 2
#define TXT_NHT 3
#define CMH_NHT 4

#define BUF_SIZE 33
#define ULTRASHORT_ARRAY_SIZE 22
#define CANCEL_ARRAY_SIZE 12
#define CAP_ARRAY_SIZE 25
#define NOCAP_ARRAY_SIZE 28



void initMyRandom();
int get_random_int(int limit);
char *int2bin(int a, char *buffer, int buf_size);
int generate_ultrashort(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generate_cancel(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generate_cap(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generate_nocap(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generateMessage(int msg_type, int priority, _Bool msg_nocap, int validity_start, int validity_end, char* generated_message);
////int generateMessage(int msg_type, int id, int ref_id, int length, int priority, _Bool msg_nocap, int validity_start, int validity_end, char* generated_message)


int messageCounter = MESSAGE_ID;

int main(void){
	initMyRandom();
	int i;
	char gen_message[28];
    char messageToBeSent[1500];
    //int msgTypeSequence[15] = {0,4,4,4,4,4,4,4,4,4,4,4,4,4,0};
    //int msgPrioritySequence[15] = {5,0,6,7,6,8,8,6,5,0,0,0,0,0,0};
	struct inputData dataIn = {gen_message, -1, -1, -1, 0};
	struct outputData dataOut = {messageToBeSent, -1, -1, 0, 0};

    for(i=0;i<15;i++){


        time_t now = time(NULL);

    	dataIn.onBoardTime = now;

        ////int first_upload =  FIRST_UPLOAD;
        ////if(first_upload == 1)
        ////dataIn.messageId = MESSAGE_ID;
        ////dataIn.messagePriority = PRIORITY;
        ////dataIn.valEndTimestamp = VALIDITY_END;
        ////dataIn.txRequest = 0;


        printf("\n\n\n\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n Step %i\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n", messageCounter);
    	int status = 0;
        int rand_type = get_random_int(4)*2; //to be removed
        //int rand_type = msgTypeSequence[i];
        //int rand_priority = msgPrioritySequence[i];
        int rand_priority = get_random_int(15);//to be removed
        int rand_bool_nocap = get_random_int(2);//to be removed
        int rand_validityStart = now -1; //to be removed
        int rand_validityEnd; //to be removed

        if (i==0)
        	rand_validityEnd = now + 1;//to be removed
        else
        	rand_validityEnd = rand_validityStart + 2 + get_random_int(5); //to be removed

        if (rand_type == 6)
        	dataIn.txRequest = 1;
        else
        	dataIn.txRequest = 0; //to be removed

        //// NEW MESSAGE MAMES HEADER
        ////int msg_id = MESSAGE_ID;
        ////int msg_type = MSG_TYPE;
        ////int msg_priority = PRIORITY;
        ////int msg_bool_nocap = MSG_NOCAP;
        ////int msg_validityEnd = VALIDITY_END;

        printf("\n-----------------------------------\nInput stored parameters\n-----------------------------------\n");
        printf("messagePriority = %i\n", dataIn.messagePriority);
        printf("valEndTimestamp = %i\n", dataIn.valEndTimestamp);
        printf("txRequest = %i\n", dataIn.txRequest);
        printf("onBoardTime = %i\n", dataIn.onBoardTime);
        printf("-----------------------------------\n");

        ////printf("\n-----------------------------------\nInput new message parameters\n-----------------------------------\n");
        ////printf("msg_id = %i\n", msg_id);
        ////printf("msg_type = %i\n", msg_type);
        ////printf("msg_priority = %i\n", msg_priority);
        ////printf("msg_bool_nocap = %i\n", msg_bool_nocap);
        ////printf("msg_validityEnd = %i\n", msg_validityEnd);
        ////if (msg_type < 6 ) status = generateMessage(msg_id, msg_type, msg_priority, msg_bool_nocap, time(NULL), msg_validityEnd, gen_message); ///to be updated!!!!

        printf("rand_type = %i\n", rand_type);
        printf("rand_priority = %i\n", rand_priority);
        printf("rand_bool_nocap = %i\n", rand_bool_nocap);
        printf("rand_validityEnd = %i\n", rand_validityEnd);

        if (rand_type < 6)

        	status = generateMessage(rand_type, rand_priority, rand_bool_nocap,
        			rand_validityStart, rand_validityEnd, gen_message);

        if (status == 1) printf("Priority not in range [0,15]\n");
        else if(status == 2) printf("Validity timestamps need to be positive\n");
        else if(status == 3) printf("Validity Start time must be greater than Validity End time\n");
        else if(status == 4) printf("Message type can be only in range [0,4]\n0=ultrashort\n1=mames update\n2=cancel\n3=mames ack\n4=mames alert\n");
        else if(status == 5) printf("Kind of message not implemented yet\n");
        else {
        	dataIn.message = gen_message;
        	dataOut.message = messageToBeSent;

             printf("\nSending informations to SAE logic...\n");
             saeFunction(&dataIn, &dataOut);


             printf("\n-----------------------------------\n  Output Parameters \n-----------------------------------\n");

             printf("messagePriority = %i\n", dataOut.messagePriority);
             printf("valEndTimestamp = %i\n", dataOut.valEndTimestamp);
             printf("eraseTxQueue = %i\n", dataOut.eraseTxQueue);
             printf("doNothing = %i\n", dataOut.doNothing);

             if(dataOut.doNothing) printf("\n********************************\nResult:\nTrasmission queue will not be overwrited!\n");
             else if(dataOut.eraseTxQueue) printf("\n********************************\nResult:\nStored message is not valid anymore: trasmission queue will not be erased!\n");
             else printf("\n********************************\nResult:\nIncoming message stored in trasmission queue: priority %i and valid until %i!\n", dataOut.messagePriority, dataOut.valEndTimestamp);

             dataIn.messagePriority = dataOut.messagePriority;
             dataIn.valEndTimestamp = dataOut.valEndTimestamp;
        }
        sleep(1);
    }
//    getchar();
    return EXIT_SUCCESS;
}


//*****************************************************************************
//                     MESSAGE GENERATION
//*****************************************************************************
int generateMessage(int msg_type, int priority, _Bool msg_nocap, int validity_start, int validity_end, char* generated_message){
	int res = 0;
//	messageCounter++;


    if(priority < 0 || priority > 15) return 1; //?
    if(validity_start < 0 || validity_end < 0) return 2; //?
    if(validity_start > validity_end) return 3; //?
    if(msg_type <0 || msg_type>4) return 4; //?

    switch (msg_type) {
    case 0: //ultrashort
       res = generate_ultrashort(priority, validity_start, validity_end, generated_message);
       break;
    case 1: //MAMES update
       return 5;
       break;
    case 2: //cancel
       res = generate_cancel(priority, validity_start, validity_end, generated_message);
       break;
    case 3: // MAMES ack
       return 5;
       break;
    case 4: //MAMES alert
       if (msg_nocap)
    	   res = generate_nocap(priority, validity_start, validity_end, generated_message); // NO CAP
       else
    	   res = generate_cap(priority, validity_start, validity_end, generated_message); // CAP
       break;
    default:
       return 4;
       break;
    }

    //printf("generated_message = %s\n", generated_message);
    return res;
}








//*****************************************************************************
//                     NO_CAP=4 true
//*****************************************************************************
int generate_nocap(int priority, int validity_start, int validity_end, char* bitstream_ptr){
    _Bool verbose = 1;
    printf("********************************\n  NO_CAP MESSAGE generated \n********************************\n");
    int i;
    char buffer[BUF_SIZE];
    buffer[BUF_SIZE - 1] = '\0';
    char buffer8[9];
    buffer8[8] = '\0';


    struct tm * time_start;
    time_t validity_start_time_t = validity_start;
    //time_start = localtime(&validity_start_time_t);
    time_start = gmtime(&validity_start_time_t);
    int val_start_year = time_start->tm_year-100;
    int val_start_month = time_start->tm_mon+1;
    int val_start_day = time_start->tm_mday;
    int val_start_hour = time_start->tm_hour+1;
    int val_start_min = time_start->tm_min;
    int val_start_sec = time_start->tm_sec;



    struct tm * time_end;
    time_t validity_end_time_t = validity_end;
    //time_end = localtime(&validity_end_time_t);
    time_end =gmtime(&validity_end_time_t);
    int val_end_year = time_end->tm_year-100;
    int val_end_month = time_end->tm_mon+1;
    int val_end_day = time_end->tm_mday;
    int val_end_hour = time_end->tm_hour+1;
    int val_end_min = time_end->tm_min;
    int val_end_sec = time_end->tm_sec;

    printf("Validity from [%i] %i/%i/%i %i:%i:%i to [%i] %i/%i/%i %i:%i:%i\n", validity_start, val_start_day,
    		val_start_month, val_start_year, val_start_hour, val_start_min, val_start_sec, validity_end,
    		val_end_day, val_end_month, val_end_year, val_end_hour, val_end_min, val_end_sec);


    //************************
    // version + type
    //************************
    int gen_version = STAND_VERSION;
    int gen_msg_type = ALERT_MSG_TYPE;
    bitstream_ptr[0] = (gen_version &0xf)<<4;
    bitstream_ptr[0] |= gen_msg_type &0xf;

    //********************************
    // messageID + alertProviderID
    //********************************
    int gen_message_id = messageCounter;
    int gen_provider_id = get_random_int(4095);
    //int2bin(gen_message_id, buffer, BUF_SIZE - 1);
    //printf("gen_message_id = %s -> %i\n", buffer, gen_message_id);
    printf("gen_message_id = %i\n", gen_message_id);
    //int2bin(gen_provider_id, buffer, BUF_SIZE - 1);
    //if(verbose) printf("gen_provider_id = %s -> %i\n", buffer, gen_provider_id);
    printf("gen_provider_id = %i\n", gen_provider_id);

    gen_message_id = gen_message_id << 12;
    gen_message_id |= gen_provider_id;
    bitstream_ptr[3] = gen_message_id &0xff;
    bitstream_ptr[2] = (gen_message_id>>8) &0xff;
    bitstream_ptr[1] = (gen_message_id>>16) &0xff;


    //********************************
    // notification area
    //********************************
    bitstream_ptr[4] = (NOTIFICATION_AREA1>>16) &0xff;
    bitstream_ptr[5] = (NOTIFICATION_AREA1>>8) &0xff;
    bitstream_ptr[6] = (NOTIFICATION_AREA1) &0xff;
    bitstream_ptr[7] = (NOTIFICATION_AREA2>>16) &0xff;
    bitstream_ptr[8] = (NOTIFICATION_AREA2>>8) &0xff;
    bitstream_ptr[9] = (NOTIFICATION_AREA2) &0xff;
    int2bin(NOTIFICATION_AREA1, buffer, BUF_SIZE - 1);
    //printf("NOTIFICATION_AREA1 = %s\n", buffer);
    int2bin(NOTIFICATION_AREA2, buffer, BUF_SIZE - 1);
    //printf("NOTIFICATION_AREA2 = %s\n", buffer);


     //********************************
    // priority + NHT
    //********************************
    int gen_priority = priority;
    //int2bin(gen_priority, buffer, BUF_SIZE - 1);
    //printf("gen_priority = %s -> %i\n", buffer, gen_priority);
    printf("gen_priority = %i\n", gen_priority);
    //printf("priority = %i\n",priority);
    int2bin(gen_priority, buffer8, 8);
    //printf("priority = %s\n", buffer8);

    bitstream_ptr[10] = (gen_priority)<<4;
    bitstream_ptr[10] |= CMH_NHT &0xf;


     //********************************
    //  issuer ID
    //********************************
    int gen_issuer = get_random_int(4095);
    //int2bin(gen_issuer, buffer, BUF_SIZE - 1);
    //printf("gen_issuer = %s -> %i\n", buffer, gen_issuer);
    printf("gen_issuer = %i\n", gen_issuer);
    //printf("issuer = %i\n",issuer);
    int2bin(gen_issuer, buffer, 32);
    //printf("issuer = %s\n", buffer);

    bitstream_ptr[11] = (gen_issuer>>8) &0xff;
    bitstream_ptr[12] = gen_issuer &0xff;


     //********************************
    // category + NHT
    //********************************
    //category = 0010
    //Next Header Type = 0001
    int gen_category = 2;
    bitstream_ptr[13] |= gen_category<<4;
    bitstream_ptr[13] |= VALIDITY_NHT &0xf;

     //********************************
    // validity_start
    //********************************
    int gen_validity_start = val_start_year;
    //int2bin(gen_validity_start, buffer, BUF_SIZE - 1);
    //printf("gen_validity_start = %s -> %i\n", buffer, gen_validity_start);
    gen_validity_start = (gen_validity_start << 4) | val_start_month;
    gen_validity_start = (gen_validity_start << 5) | val_start_day;
    gen_validity_start = (gen_validity_start << 5) | val_start_hour;
    gen_validity_start = (gen_validity_start << 6) | val_start_min;
    gen_validity_start = (gen_validity_start << 6) | val_start_sec;
    int2bin(gen_validity_start, buffer, 32);
    //printf("validity_start = %s\n", buffer);
    bitstream_ptr[14] = (gen_validity_start>>24) &0xff;
    bitstream_ptr[15] = (gen_validity_start>>16) &0xff;
    bitstream_ptr[16] = (gen_validity_start>>8) &0xff;
    bitstream_ptr[17] = gen_validity_start &0xff;

    //********************************
    // validity end
    //********************************
    int gen_validity_end = val_end_year;
    gen_validity_end = (gen_validity_end << 4) | val_end_month;
    gen_validity_end = (gen_validity_end << 5) | val_end_day;
    gen_validity_end = (gen_validity_end << 5) | val_end_hour;
    gen_validity_end = (gen_validity_end << 6) | val_end_min;
    gen_validity_end = (gen_validity_end << 6) | val_end_sec;
    int2bin(gen_validity_end, buffer, 32);
    //printf("validity_end = %s\n", buffer);
    bitstream_ptr[18] = (gen_validity_end>>24) &0xff;
    bitstream_ptr[19] = (gen_validity_end>>16) &0xff;
    bitstream_ptr[20] = (gen_validity_end>>8) &0xff;
    bitstream_ptr[21] = gen_validity_end &0xff;


    //********************************
    // NHT + language ID
    //********************************
    int language_id = get_random_int(4095);
    //int2bin(language_id, buffer, BUF_SIZE - 1);
    //printf("language_id = %s -> %i\n", buffer, language_id);
    printf("language_id = %i\n", language_id);

    bitstream_ptr[22] = (TXT_NHT<<4);
    bitstream_ptr[22] |= (language_id>>8) &0xf;
    bitstream_ptr[23] = language_id &0xff;


     //********************************
    // message length + NHT
    //********************************
    int message_length = get_random_int(268435456);
    //int2bin(message_length, buffer, BUF_SIZE - 1);
    //printf("message_length = %s -> %i\n", buffer, message_length);
    printf("message_length = %i\n", message_length);

    bitstream_ptr[24] = message_length>>20;
    bitstream_ptr[25] = (message_length>>12) &0xff;
    bitstream_ptr[26] = (message_length>>4) &0xff;
    bitstream_ptr[27] = (message_length &0xf)<<4;
    bitstream_ptr[27] |= NOHEADER_NHT &0xf;

    if(verbose) {
        for (i = 0; i < NOCAP_ARRAY_SIZE; i++ ){
            int2bin(bitstream_ptr[i], buffer8, 8);
            //printf("bitstream_ptr[%i] = %s\n", i, buffer8);
        }
    }
    return 0;
}

//*****************************************************************************
//                     CAP=4 false
//*****************************************************************************
int generate_cap(int priority, int validity_start, int validity_end, char* bitstream_ptr){
    printf("********************************\n  CAP MESSAGE generated \n ********************************\n");
    int i;
    char buffer[BUF_SIZE];
    buffer[BUF_SIZE - 1] = '\0';
    char buffer8[9];
    buffer8[8] = '\0';


    struct tm * time_start;
    time_t validity_start_time_t = validity_start;
    //time_start = localtime(&validity_start_time_t);
    time_start = gmtime(&validity_start_time_t);
    int val_start_year = time_start->tm_year-100;
    int val_start_month = time_start->tm_mon+1;
    int val_start_day = time_start->tm_mday;
    int val_start_hour = time_start->tm_hour+1;
    int val_start_min = time_start->tm_min;
    int val_start_sec = time_start->tm_sec;

    struct tm * time_end;
    time_t validity_end_time_t = validity_end;
    //time_end = localtime(&validity_end_time_t);
    time_end = gmtime(&validity_end_time_t);
    int val_end_year = time_end->tm_year-100;
    int val_end_month = time_end->tm_mon+1;
    int val_end_day = time_end->tm_mday;
    int val_end_hour = time_end->tm_hour+1;
    int val_end_min = time_end->tm_min;
    int val_end_sec = time_end->tm_sec;

    printf("Validity from [%i] %i/%i/%i %i:%i:%i to [%i] %i/%i/%i %i:%i:%i\n", validity_start, val_start_day, val_start_month, val_start_year, val_start_hour, val_start_min, val_start_sec, validity_end, val_end_day, val_end_month, val_end_year, val_end_hour, val_end_min, val_end_sec);


    //************************
    // version + type
    //************************
    int gen_version = STAND_VERSION;
    int gen_msg_type = ALERT_MSG_TYPE;
    bitstream_ptr[0] = (gen_version &0xf)<<4;
    bitstream_ptr[0] |= gen_msg_type &0xf;

    //********************************
    // messageID + alertProviderID
    //********************************
    int gen_message_id = messageCounter;
    int gen_provider_id = get_random_int(4095);
    int2bin(gen_message_id, buffer, BUF_SIZE - 1);
    printf("gen_message_id = %s -> %i\n", buffer, gen_message_id);
    int2bin(gen_provider_id, buffer, BUF_SIZE - 1);
    printf("gen_provider_id = %s -> %i\n", buffer, gen_provider_id);

    gen_message_id = gen_message_id << 12;
    gen_message_id |= gen_provider_id;
    bitstream_ptr[3] = gen_message_id &0xff;
    bitstream_ptr[2] = (gen_message_id>>8) &0xff;
    bitstream_ptr[1] = (gen_message_id>>16) &0xff;


    //********************************
    // notification area
    //********************************
    bitstream_ptr[4] = (NOTIFICATION_AREA1>>16) &0xff;
    bitstream_ptr[5] = (NOTIFICATION_AREA1>>8) &0xff;
    bitstream_ptr[6] = (NOTIFICATION_AREA1) &0xff;
    bitstream_ptr[7] = (NOTIFICATION_AREA2>>16) &0xff;
    bitstream_ptr[8] = (NOTIFICATION_AREA2>>8) &0xff;
    bitstream_ptr[9] = (NOTIFICATION_AREA2) &0xff;
    int2bin(NOTIFICATION_AREA1, buffer, BUF_SIZE - 1);
    //printf("NOTIFICATION_AREA1 = %s\n", buffer);
    int2bin(NOTIFICATION_AREA2, buffer, BUF_SIZE - 1);
    //printf("NOTIFICATION_AREA2 = %s\n", buffer);


     //********************************
    // priority + NHT
    //********************************
    int gen_priority = priority;
    int2bin(gen_priority, buffer, BUF_SIZE - 1);
    printf("gen_priority = %s -> %i\n", buffer, gen_priority);
    //printf("priority = %i\n",priority);
    int2bin(gen_priority, buffer8, 8);
    //printf("priority = %s\n", buffer8);

    bitstream_ptr[10] = (gen_priority<<4);
    bitstream_ptr[10] |= VALIDITY_NHT &0xf;

     //********************************
    // validity_start
    //********************************
    int gen_validity_start = val_start_year;
    gen_validity_start = (gen_validity_start << 4) | val_start_month;
    gen_validity_start = (gen_validity_start << 5) | val_start_day;
    gen_validity_start = (gen_validity_start << 5) | val_start_hour;
    gen_validity_start = (gen_validity_start << 6) | val_start_min;
    gen_validity_start = (gen_validity_start << 6) | val_start_sec;
    int2bin(gen_validity_start, buffer, 32);
    //printf("validity_start = %s\n", buffer);
    bitstream_ptr[11] = (gen_validity_start>>24) &0xff;
    bitstream_ptr[12] = (gen_validity_start>>16) &0xff;
    bitstream_ptr[13] = (gen_validity_start>>8) &0xff;
    bitstream_ptr[14] = gen_validity_start &0xff;

    //********************************
    // validity end
    //********************************
    int gen_validity_end = val_end_year;
    gen_validity_end = (gen_validity_end << 4) | val_end_month;
    gen_validity_end = (gen_validity_end << 5) | val_end_day;
    gen_validity_end = (gen_validity_end << 5) | val_end_hour;
    gen_validity_end = (gen_validity_end << 6) | val_end_min;
    gen_validity_end = (gen_validity_end << 6) | val_end_sec;
    int2bin(gen_validity_end, buffer, 32);
    //printf("validity_end = %s\n", buffer);
    bitstream_ptr[15] = (gen_validity_end>>24) &0xff;
    bitstream_ptr[16] = (gen_validity_end>>16) &0xff;
    bitstream_ptr[17] = (gen_validity_end>>8) &0xff;
    bitstream_ptr[18] = gen_validity_end &0xff;


    //********************************
    // NHT + language ID
    //********************************
    int language_id = get_random_int(4095);
    int2bin(language_id, buffer, BUF_SIZE - 1);
    printf("language_id = %s -> %i\n", buffer, language_id);

    bitstream_ptr[19] = (CAP_NHT & 0xf)<<4;
    bitstream_ptr[19] |= language_id>>8;
    bitstream_ptr[20] = language_id &0xff;


     //********************************
    // message length + NHT
    //********************************
    int message_length = get_random_int(268435456);
    int2bin(message_length, buffer, BUF_SIZE - 1);
    printf("message_length = %s -> %i\n", buffer, message_length);

    bitstream_ptr[21] = message_length>>20;
    bitstream_ptr[22] = (message_length>>12) &0xff;
    bitstream_ptr[23] = (message_length>>4) &0xff;
    bitstream_ptr[24] = (message_length &0xf)<<4;
    bitstream_ptr[24] |= NOHEADER_NHT &0xf;


    for (i = 0; i < CAP_ARRAY_SIZE; i++ ){
        int2bin(bitstream_ptr[i], buffer8, 8);
        //printf("bitstream_ptr[%i] = %s\n", i, buffer8);
    }
    return 0;
}

//*****************************************************************************
//                     CANCEL=2
//*****************************************************************************
int generate_cancel(int priority, int validity_start, int validity_end, char* bitstream_ptr){
    printf("********************************\n  CANCEL MESSAGE generated \n ********************************\n");
    int i;
    char buffer[BUF_SIZE];
    buffer[BUF_SIZE - 1] = '\0';
    char buffer8[9];
    buffer8[8] = '\0';


    //************************
    // version + type
    //************************
    int gen_version = STAND_VERSION;
    int gen_msg_type = CANCEL_MSG_TYPE;
    bitstream_ptr[0] = (gen_version &0xf)<<4;
    bitstream_ptr[0] |= gen_msg_type &0xf;


    //********************************
    // messageID + alertProviderID
    //********************************
    int gen_message_id = messageCounter;
    int gen_provider_id = get_random_int(4095);
    int2bin(gen_message_id, buffer, BUF_SIZE - 1);
    printf("gen_message_id = %s -> %i\n", buffer, gen_message_id);
    int2bin(gen_provider_id, buffer, BUF_SIZE - 1);
    printf("gen_provider_id = %s -> %i\n", buffer, gen_provider_id);

    gen_message_id = gen_message_id << 12;
    gen_message_id |= gen_provider_id;
    bitstream_ptr[3] = gen_message_id &0xff;
    bitstream_ptr[2] = (gen_message_id>>8) &0xff;
    bitstream_ptr[1] = (gen_message_id>>16) &0xff;


     //********************************
    // reference + notification area
    //********************************
    int gen_id_old_message = MESSAGE_ID;
    int2bin(gen_id_old_message, buffer, BUF_SIZE - 1);
    printf("gen_id_old_message = %s -> %i\n", buffer, gen_id_old_message);
    bitstream_ptr[4] = (gen_id_old_message>>4) &0xff;
    bitstream_ptr[5] = ((gen_id_old_message) &0xf)<<4;
    bitstream_ptr[5] |= (NOTIFICATION_AREA1>>20) &0xff;


    //**************************************
    // notification area
    //**************************************
    bitstream_ptr[6] = (NOTIFICATION_AREA1>>12) &0xff;
    bitstream_ptr[7] = (NOTIFICATION_AREA1>>4) &0xff;
    bitstream_ptr[8] = (NOTIFICATION_AREA1 &0xf)<<4;
    bitstream_ptr [8] |= (NOTIFICATION_AREA2>>20) &0xff;
    bitstream_ptr [9] = (NOTIFICATION_AREA2>>12) &0xff;
    bitstream_ptr [10] = (NOTIFICATION_AREA2>>4) &0xff;
    bitstream_ptr [11] = (NOTIFICATION_AREA2 &0xf)<<4;

    int2bin(NOTIFICATION_AREA1, buffer, BUF_SIZE - 1);
    printf("NOTIFICATION_AREA1 = %s\n", buffer);
    int2bin(NOTIFICATION_AREA2, buffer, BUF_SIZE - 1);
    printf("NOTIFICATION_AREA2 = %s\n", buffer);



     //***********************************
    //  notification area priority + NHT
    //************************************
    int gen_priority = priority;
    int2bin(gen_priority, buffer, BUF_SIZE - 1);
    printf("gen_priority = %s -> %i\n", buffer, gen_priority);
	bitstream_ptr[11] |= gen_priority &0xff;


    for (i = 0; i < CANCEL_ARRAY_SIZE; i++ ){
        int2bin(bitstream_ptr[i], buffer8, 8);
        //printf("bitstream_ptr[%i] = %s\n", i, buffer8);
    }
    return 0;
}

//*****************************************************************************
//                     ULTRASHORT=0
//*****************************************************************************
int generate_ultrashort(int priority, int validity_start, int validity_end, char* bitstream_ptr){
    printf("********************************\n  ULTRASHORT MESSAGE generated \n ********************************\n");
    int i;
    char buffer[BUF_SIZE];
    buffer[BUF_SIZE - 1] = '\0';
    char buffer8[9];
    buffer8[8] = '\0';


    struct tm * time_start;
    time_t validity_start_time_t = validity_start;
    //time_start = localtime(&validity_start_time_t);
    time_start = gmtime(&validity_start_time_t);
    int val_start_year = time_start->tm_year-100;
    int val_start_month = time_start->tm_mon+1;
    int val_start_day = time_start->tm_mday;
    int val_start_hour = time_start->tm_hour+1;
    int val_start_min = time_start->tm_min;
    int val_start_sec = time_start->tm_sec;

    struct tm * time_end;
    time_t validity_end_time_t = validity_end;
    //time_end = localtime(&validity_end_time_t);
    time_end = gmtime(&validity_end_time_t);
    int val_end_year = time_end->tm_year-100;
    int val_end_month = time_end->tm_mon+1;
    int val_end_day = time_end->tm_mday;
    int val_end_hour = time_end->tm_hour+1;
    int val_end_min = time_end->tm_min;
    int val_end_sec = time_end->tm_sec;

    printf("Validity from [%i] %i/%i/%i %i:%i:%i to [%i] %i/%i/%i %i:%i:%i\n", validity_start, val_start_day, val_start_month, val_start_year, val_start_hour, val_start_min, val_start_sec, validity_end, val_end_day, val_end_month, val_end_year, val_end_hour, val_end_min, val_end_sec);


    //************************
    // version + type
    //************************
    int gen_version = STAND_VERSION;
    int gen_msg_type = ULTRASHORT_MSG_TYPE;
    bitstream_ptr[0] = (gen_version &0xf)<<4;
    bitstream_ptr[0] |= gen_msg_type &0xf;

    //********************************
    // messageID + alertProviderID
    //********************************
    int gen_message_id = messageCounter;
    int gen_provider_id = get_random_int(4095);
    int2bin(gen_message_id, buffer, BUF_SIZE - 1);
    printf("gen_message_id = %s -> %i\n", buffer, gen_message_id);
    int2bin(gen_provider_id, buffer, BUF_SIZE - 1);
    printf("gen_provider_id = %s -> %i\n", buffer, gen_provider_id);

    gen_message_id = gen_message_id << 12;
    gen_message_id |= gen_provider_id;
    bitstream_ptr[3] = gen_message_id &0xff;
    bitstream_ptr[2] = (gen_message_id>>8) &0xff;
    bitstream_ptr[1] = (gen_message_id>>16) &0xff;


    //********************************
    // notification area
    //********************************
    bitstream_ptr[4] = (NOTIFICATION_AREA1>>16) &0xff;
    bitstream_ptr[5] = (NOTIFICATION_AREA1>>8) &0xff;
    bitstream_ptr[6] = (NOTIFICATION_AREA1) &0xff;
    bitstream_ptr[7] = (NOTIFICATION_AREA2>>16) &0xff;
    bitstream_ptr[8] = (NOTIFICATION_AREA2>>8) &0xff;
    bitstream_ptr[9] = (NOTIFICATION_AREA2) &0xff;
    int2bin(NOTIFICATION_AREA1, buffer, BUF_SIZE - 1);
    //printf("NOTIFICATION_AREA1 = %s\n", buffer);
    int2bin(NOTIFICATION_AREA2, buffer, BUF_SIZE - 1);
    //printf("NOTIFICATION_AREA2 = %s\n", buffer);


     //********************************
    // priority + issuer ID
    //********************************
    int gen_priority = priority;
    int gen_issuer = get_random_int(4095);
    int2bin(gen_priority, buffer, BUF_SIZE - 1);
    printf("gen_priority = %s -> %i\n", buffer, gen_priority);
    int2bin(gen_issuer, buffer, BUF_SIZE - 1);
    printf("gen_issuer = %s -> %i\n", buffer, gen_issuer);
    //printf("priority = %i\n",priority);
    //printf("issuer = %i\n",issuer);
    int2bin(gen_priority, buffer8, 8);
    //printf("priority = %s\n", buffer8);
    int2bin(gen_issuer, buffer, 32);
    //printf("issuer = %s\n", buffer);

    gen_priority = gen_priority << 12;
    int2bin(gen_priority, buffer, 32);
    //printf("priority = %s\n", buffer);
    gen_priority |= gen_issuer;
    int2bin(gen_priority, buffer, 32);
    //printf("priority = %s\n", buffer);
    bitstream_ptr[10] = (gen_priority>>8) &0xff;
    bitstream_ptr[11] = gen_priority &0xff;

     //********************************
    // category + NHT
    //********************************
    //category = 0010
    //Next Header Type = 0001
    char gen_category = 0;
    gen_category = gen_category|0x21;
    bitstream_ptr[12] = gen_category;

     //********************************
    // validity_start
    //********************************
    int gen_validity_start = val_start_year;
    gen_validity_start = (gen_validity_start << 4) | val_start_month;
    gen_validity_start = (gen_validity_start << 5) | val_start_day;
    gen_validity_start = (gen_validity_start << 5) | val_start_hour;
    gen_validity_start = (gen_validity_start << 6) | val_start_min;
    gen_validity_start = (gen_validity_start << 6) | val_start_sec;
    int2bin(gen_validity_start, buffer, 32);
    //printf("validity_start = %s\n", buffer);
    bitstream_ptr[13] = (gen_validity_start>>24) &0xff;
    bitstream_ptr[14] = (gen_validity_start>>16) &0xff;
    bitstream_ptr[15] = (gen_validity_start>>8) &0xff;
    bitstream_ptr[16] = gen_validity_start &0xff;

    //********************************
    // validity end
    //********************************
    int gen_validity_end = val_end_year;
    gen_validity_end = (gen_validity_end << 4) | val_end_month;
    gen_validity_end = (gen_validity_end << 5) | val_end_day;
    gen_validity_end = (gen_validity_end << 5) | val_end_hour;
    gen_validity_end = (gen_validity_end << 6) | val_end_min;
    gen_validity_end = (gen_validity_end << 6) | val_end_sec;
    int2bin(gen_validity_end, buffer, 32);
    //printf("validity_start = %s\n", buffer);
    bitstream_ptr[17] = (gen_validity_end>>24) &0xff;
    bitstream_ptr[18] = (gen_validity_end>>16) &0xff;
    bitstream_ptr[19] = (gen_validity_end>>8) &0xff;
    bitstream_ptr[20] = gen_validity_end &0xff;

    for (i = 0; i < ULTRASHORT_ARRAY_SIZE; i++ ){
        int2bin(bitstream_ptr[i], buffer8, 8);
        //printf("bitstream_ptr[%i] = %s\n", i, buffer8);
    }
    printf("bitstream = %s\n", bitstream_ptr);
    return 0;
}

void initMyRandom(void){
	time_t t;
	srand((unsigned)time(&t));
}
int get_random_int(int limit){
    int number;
    number = rand()%limit;
   // number2 = number-(rand()%number);
   return number;
}
char *int2bin(int a,char *buffer,int buf_size){
	buffer+=(buf_size-1);
	int i;
	for(i=buf_size;i!=0;i--)
	{
		*buffer--=(a&1)+'0';
		a>>=1;
	}
	return buffer;
}


//int main(void) {
//	puts("Hello World"); /* prints Hello World */
//	return EXIT_SUCCESS;
//}
