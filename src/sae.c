/*
 * sae.c
 *
 *  Created on: 12/apr/2016
 *      Author: ronga
 */

#include "sae.h"
#include <stdlib.h>
#include <time.h>


void saeFunction(struct inputData *datiIn, struct outputData *datiOut){
     struct messageFields messageReceived = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

     if(datiIn->txRequest==0) parseMessage(datiIn->message, &messageReceived); // message need to be parsed only in use case 1, when txRequest = 0
     applyLogic(&messageReceived, datiIn, datiOut);
}

//*****************************************************************************
//                     APPLY LOGIC
//*****************************************************************************
int applyLogic(struct messageFields *messageReceived, struct inputData *datiIn, struct outputData *datiOut){

    int OutMsgLenght = 0;
    if(messageReceived->messageType == 0){
        OutMsgLenght = 500;
    }
    if(messageReceived->messageType == 4){
        OutMsgLenght = 1500;
    }
    //Use Case 2
    if(datiIn->txRequest==1){
        //printf("Tx Request received\n");
        if(datiIn->valEndTimestamp > 0 && datiIn->valEndTimestamp <= datiIn->onBoardTime){ //-> Check if message is still valid and the previous was not a cancel msg

            //TEST CASE 2.1
            //printf("TEST CASE 2.1 \n onboard tx queue need to be erased \n %i > 0 && %i <= %i\n", datiIn->valEndTimestamp, datiIn->valEndTimestamp, datiIn->onBoardTime);
            datiOut->eraseTxQueue = 1; // onboard tx queue need to be erased
            datiOut->messageId = -1;
            datiOut->messagePriority = -1;
            datiOut->valEndTimestamp = -1;
            datiOut->doNothing = 0;
        } else {
            //TEST CASE 2.2
            //printf("TEST CASE 2.2 \n nothing to do, tx queue is still valid \n $i <= 0 || %i > %i\n", datiIn->valEndTimestamp, datiIn->valEndTimestamp, datiIn->onBoardTime);
            datiOut->doNothing = 1; // nothing to do, tx queue is still valid
            datiOut->eraseTxQueue = 0;
        }
    } else {
        //printf("Message received\n");
        //Use Case 1
        if(messageReceived->messageReference>0){//in case of cancel message
            if(datiIn->messageId == messageReceived->messageReference) { // check if messagerefernce of cancel message is really the one in the tx queue
                 //TEST CASE 1.4
                 //printf("TEST CASE 1.4 \n onboard tx queue need to be erased \n %i > 0 && %i == %i\n", messageReceived->messageReference, datiIn->messageId, messageReceived->messageReference);
                 datiOut->eraseTxQueue = 1; // onboard tx queue need to be erased
                 datiOut->messageId = -1;
                 datiOut->messagePriority = -1;
                 datiOut->valEndTimestamp = -1;
                 datiOut->doNothing = 0;
            } else {//stored message is not the one to be deleted
                  //TEST CASE 1.5
                  //printf("TEST CASE 1.5 \n stored message is not the one to be deleted \n %i > 0 && %i != %i\n", messageReceived->messageReference, datiIn->messageId, messageReceived->messageReference);
                  datiOut->doNothing = 1;
                  datiOut->eraseTxQueue = 0;
            }
        } else {
            if(messageReceived->messagePriority >= datiIn->messagePriority){ // new uploaded message has higher priority than previous stored message
              //TEST CASE 1.3
              //printf("TEST CASE 1.3 \n new uploaded message has higher priority than previous stored message \n %i <= 0 && %i >= %i\n", messageReceived->messageReference, messageReceived->messagePriority, datiIn->messagePriority);
              datiOut->message = datiIn->message;
              datiOut->messageId = messageReceived->messageId;
              datiOut->messagePriority = messageReceived->messagePriority;
              datiOut->valEndTimestamp = messageReceived->valEndTimestamp;
              datiOut->doNothing = 0;
              datiOut->eraseTxQueue = 0;
            } else {
              if(datiIn->valEndTimestamp <= datiIn->onBoardTime){ // if previous stored message has higher priority but it is no more valid, new message will be saved
                  //TEST CASE 1.1
                  //printf("TEST CASE 1.1 \n previous stored message has higher priority but it is no more valid, new message will be saved \n %i<=0 && %i < %i && %i <= %i\n", messageReceived->messageReference, messageReceived->messagePriority, datiIn->messagePriority, datiIn->valEndTimestamp, datiIn->onBoardTime);
                  datiOut->message = datiIn->message;
                  datiOut->messageId = messageReceived->messageId;
                  datiOut->messagePriority = messageReceived->messagePriority;
                  datiOut->valEndTimestamp = messageReceived->valEndTimestamp;
                  datiOut->doNothing = 0;
                  datiOut->eraseTxQueue = 0;
              } else {//  previous stored message has higher priority and it is still valid
                  //TEST CASE 1.2
                  //printf("TEST CASE 1.2 \n previous stored message has higher priority and it is still valid \n %i<=0 && %i < %i && %i > %i\n", messageReceived->messageReference, messageReceived->messagePriority, datiIn->messagePriority, datiIn->valEndTimestamp, datiIn->onBoardTime);
                  datiOut->doNothing = 1;
                  datiOut->eraseTxQueue = 0;
                  OutMsgLenght = 0;

              }
            }
        }
        //printf("OutMsgLenght = %i\n", OutMsgLenght);
        datiOut->outputMsgLenght = OutMsgLenght;
    }
    return 0;
}


//*****************************************************************************
//                     MESSAGE PARSING
//*****************************************************************************
int parseMessage(char *received_message, struct messageFields *messageReceived){

    //**********************************
    // protocol_version + message_type
    //**********************************
    int message_type = received_message[0] & 0x0F;
    int protocol_version = received_message[0] >> 4;
    messageReceived->messageType = message_type;
    messageReceived->protocolVersion = protocol_version;

    switch(message_type){
        case 0: //ultrashort
           parse_ultrashort(received_message, messageReceived);
           break;
        case 1: //MAMES update
           break;
        case 2: //cancel
           parse_cancel(received_message, messageReceived);
           break;
        case 3: // MAMES ack
           break;
        case 4: //MAMES alert
           parse_alert(received_message, messageReceived);
           break;
    }
    return 0;
}

//*****************************************************************************
//                     PARSING ALERT MESSAGE
//*****************************************************************************
int parse_alert(char *bitstream_ptr, struct messageFields *messageReceived){

    //**********************************
    // message_id + alert_provider_id
    //**********************************
    int messageId = 0;
    messageId |= (bitstream_ptr[1] &0xff);
    messageId =  messageId<<4;
    messageId |= ((bitstream_ptr[2] &0xff)>>4 &0xf);
    messageReceived->messageId = messageId;

    int alertProviderId = 0;
    alertProviderId |= (bitstream_ptr[2] &0x0f);
    alertProviderId = alertProviderId << 8;
    alertProviderId |= (bitstream_ptr[3] &0xff);
    messageReceived->alertProviderId = alertProviderId;


    //**********************************
    // notification_area
    //**********************************
    int lat_ns=0, lat_deg=0, lat_min=0, lat_sec=0, lon_ew=0, lon_deg=0, lon_min=0, lon_sec=0, radius=0;

    lat_ns |= (bitstream_ptr[4] &0x80)>>7;
    messageReceived->notifAreaLatNS = lat_ns;

    lat_deg |= (bitstream_ptr[4] &0x7f)<<1;
    lat_deg |= (bitstream_ptr[5] &0x80)>>7;
    messageReceived->notifAreaLatDeg = lat_deg;


    lat_min |= (bitstream_ptr[5] &0x7f);
    messageReceived->notifAreaLatMin = lat_min;

    lat_sec |= (bitstream_ptr[6] &0xfc)>>2;
    messageReceived->notifAreaLatSec = lat_sec;

    lon_ew |= (bitstream_ptr[6] &0x2)>>1;
    messageReceived->notifAreaLonEW = lon_ew;

    lon_deg |= (bitstream_ptr[6] &0x1)<<7;
    lon_deg |= (bitstream_ptr[7] &0xfe)>>1;
    messageReceived->notifAreaLonDeg = lon_deg;

    lon_min |= (bitstream_ptr[7] &0x1)<<7;
    lon_min |= (bitstream_ptr[8] &0xfc)>>2;
    messageReceived->notifAreaLonMin = lon_min;

    lon_sec |= (bitstream_ptr[8] &0x3)<<4;
    lon_sec |= (bitstream_ptr[9] &0xf0)>>4;
    messageReceived->notifAreaLonSec = lon_sec;

    radius |= (bitstream_ptr[9] &0xf);
    messageReceived->notifAreaRadius = radius;
    //**********************************
    // priority
    //**********************************
    int priority = 0;
    priority |= (bitstream_ptr[10] &0xf0)>>4;
    messageReceived->messagePriority = priority;

     //**********************************
    // NHT
    //**********************************
    int nht1 = 0;
    nht1 |= (bitstream_ptr[10] &0xf);

    int messageIndexShift = 0;
    if(nht1 == 4){
        messageIndexShift = 3;

        //**********************************
        // issuer
        //**********************************
        int issuer = 0;
        issuer |= (bitstream_ptr[11] &0xff);
        issuer = issuer << 8;
        issuer |= (bitstream_ptr[12] &0xff);

        //**********************************
        // category + NHT
        //**********************************
        int category = 0;
        category |= (bitstream_ptr[13] &0xf0)>>4;


        int nht2 = 0;
        nht2 |= (bitstream_ptr[13] &0xf);

    }


    //**********************************
    // validity start
    //**********************************
    int val_start_year=0,val_start_month=0,val_start_day=0,val_start_hour=0,val_start_min=0,val_start_sec=0;

    val_start_year |= (bitstream_ptr[(11+messageIndexShift)] &0xfc)>>2;
    messageReceived->valStartYear = val_start_year;

    val_start_month |= (bitstream_ptr[(11+messageIndexShift)] &0x3)<<2;
    val_start_month |= (bitstream_ptr[(12+messageIndexShift)] &0xc0)>>6;
    messageReceived->valStartMonth = val_start_month;

    val_start_day |= (bitstream_ptr[(12+messageIndexShift)] &0x3E)>>1;
    messageReceived->valStartDay = val_start_day;

    val_start_hour |= (bitstream_ptr[(12+messageIndexShift)] &0x1)<<4;
    val_start_hour |= (bitstream_ptr[(13+messageIndexShift)] &0xf0)>>4;
    messageReceived->valStartHour = val_start_hour;

    val_start_min |= (bitstream_ptr[(13+messageIndexShift)] &0xf)<<2;
    val_start_min |= (bitstream_ptr[(14+messageIndexShift)] &0xc0)>>6;
    messageReceived->valStartMin = val_start_min;

    val_start_sec |= (bitstream_ptr[(14+messageIndexShift)] &0x3f);
    messageReceived->valStartSec = val_start_sec;


    //**********************************
    // validity end
    //**********************************
    int val_end_year=0,val_end_month=0,val_end_day=0,val_end_hour=0,val_end_min=0,val_end_sec=0;

    val_end_year |= (bitstream_ptr[(15+messageIndexShift)] &0xfc)>>2;
    messageReceived->valEndYear = val_end_year;

    val_end_month |= (bitstream_ptr[(15+messageIndexShift)] &0x3)<<2;
    val_end_month |= (bitstream_ptr[(16+messageIndexShift)] &0xc0)>>6;
    messageReceived->valEndMonth = val_end_month;

    val_end_day |= (bitstream_ptr[(16+messageIndexShift)] &0x3E)>>1;
    messageReceived->valEndDay = val_end_day;

    val_end_hour |= (bitstream_ptr[(16+messageIndexShift)] &0x1)<<4;
    val_end_hour |= (bitstream_ptr[(17+messageIndexShift)] &0xf0)>>4;
    messageReceived->valEndHour = val_end_hour;

    val_end_min |= (bitstream_ptr[(17+messageIndexShift)] &0xf)<<2;
    val_end_min |= (bitstream_ptr[(18+messageIndexShift)] &0xc0)>>6;
    messageReceived->valEndMin = val_end_min;

    val_end_sec |= (bitstream_ptr[(18+messageIndexShift)] &0x3f);
    messageReceived->valEndSec = val_end_sec;


    //converting date data to timestamp format
    struct tm t_start;
    time_t timestamp_start;
    t_start.tm_year = val_start_year + 100;
    t_start.tm_mon = val_start_month-1;
    t_start.tm_mday = val_start_day;
    t_start.tm_hour = val_start_hour+2;
    t_start.tm_min = val_start_min;
    t_start.tm_sec = val_start_sec;
    t_start.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    timestamp_start = mktime(&t_start);
    messageReceived->valStartTimestamp = timestamp_start;
    struct tm t_end;
    time_t timestamp_end;
    t_end.tm_year = val_end_year + 100;
    t_end.tm_mon = val_end_month-1;
    t_end.tm_mday = val_end_day;
    t_end.tm_hour = val_end_hour+2;
    t_end.tm_min = val_end_min;
    t_end.tm_sec = val_end_sec;
    t_end.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    timestamp_end = mktime(&t_end);
    messageReceived->valEndTimestamp = timestamp_end;

     //**********************************
    // NHT + language ID
    //**********************************
    int nht3 = 0;
    nht3 |= (bitstream_ptr[(19+messageIndexShift)] &0xf0)>>4;

    int language_id = 0;
    language_id |= (bitstream_ptr[(19+messageIndexShift)] &0xf)<<8;
    language_id |= (bitstream_ptr[(20+messageIndexShift)] &0xff);


	//**********************************
    // message length
    //**********************************
    int message_length = 0;
	message_length |= (bitstream_ptr[(21+messageIndexShift)] &0xff)<<20;
    message_length |= (bitstream_ptr[(22+messageIndexShift)]<<12 &0xff000);
    message_length |= (bitstream_ptr[(23+messageIndexShift)]<<4 &0xff0);
    message_length |= bitstream_ptr[(24+messageIndexShift)] &0xf0;

    //**********************************
    // NHT
    //**********************************
    int nht4 = 0;
    nht4 |= (bitstream_ptr[(24+messageIndexShift)] &0xf);
    return 0;
}
//*****************************************************************************
//                     PARSING CANCEL MESSAGE
//*****************************************************************************
int parse_cancel(char *bitstream_ptr, struct messageFields *messageReceived){
    //********************************
    // messageID + alertProviderID
    //********************************
    int messageId = 0;
    messageId |= (bitstream_ptr[1] &0xff);
    messageId =  messageId<<4;
    messageId |= (bitstream_ptr[2]>>4 &0xf);
    messageReceived->messageId = messageId;

    int alertProviderId = 0;
    alertProviderId |= (bitstream_ptr[2] &0x0f);
    alertProviderId = alertProviderId << 8;
    alertProviderId |= (bitstream_ptr[3] &0xff);
    messageReceived->alertProviderId = alertProviderId;


     //********************************
    // reference
    //********************************
    int reference = 0;
    reference = (bitstream_ptr[4] &0xff)<<4;
    reference |= (bitstream_ptr[5]&0xf0)>>4;
    messageReceived->messageReference = reference;


    //**************************************
    // notification area
    //**************************************
    int lat_ns=0, lat_deg=0, lat_min=0, lat_sec=0, lon_ew=0, lon_deg=0, lon_min=0, lon_sec=0, radius=0;

    lat_ns |= (bitstream_ptr[5] &0x8)>>3;
    messageReceived->notifAreaLatNS = lat_ns;

    lat_deg |= (bitstream_ptr[5] &0x7)<<5;
    lat_deg |= (bitstream_ptr[6] &0xf8)>>3;
    messageReceived->notifAreaLatDeg = lat_deg;


    lat_min |= (bitstream_ptr[6] &0x7)<<4;
    lat_min |= (bitstream_ptr[7] &0xf0)>>4;
    messageReceived->notifAreaLatMin = lat_min;

    lat_sec |= (bitstream_ptr[7] &0xf)<<2;
    lat_sec |= (bitstream_ptr[8] &0xc0)>>6;
    messageReceived->notifAreaLatSec = lat_sec;

    lon_ew |= (bitstream_ptr[8] &0x20)>>5;
    messageReceived->notifAreaLonEW = lon_ew;

    lon_deg |= (bitstream_ptr[8] &0x1f)<<3;
    lon_deg |= (bitstream_ptr[9] &0xe0)>>5;
    messageReceived->notifAreaLonDeg = lon_deg;

    lon_min |= (bitstream_ptr[9] &0x1f)<<2;
    lon_min |= (bitstream_ptr[10] &0xc0)>>6;
    messageReceived->notifAreaLonMin = lon_min;

    lon_sec |= (bitstream_ptr[10] &0x3f);
    messageReceived->notifAreaLonSec = lon_sec;

    radius |= (bitstream_ptr[11] &0xf0)>>4;
    messageReceived->notifAreaRadius = radius;


     //***********************************
    //  priority
    //************************************
    int priority = 0;
    priority |= (bitstream_ptr[11] &0xf);
    messageReceived->messagePriority = priority;

    return 0;
}
//*****************************************************************************
//                     PARSING ULTRASHORT MESSAGE
//*****************************************************************************
int parse_ultrashort(char *bitstream_ptr, struct messageFields *messageReceived){
    //**********************************
    // message_id + alert_provider_id
    //**********************************
    int messageId = 0;
    messageId |= (bitstream_ptr[1] &0xff);
    messageId =  messageId<<4;
    messageId |= (bitstream_ptr[2]>>4 &0xf);
    messageReceived->messageId = messageId;

    int alertProviderId = 0;
    alertProviderId |= (bitstream_ptr[2] &0x0f);
    alertProviderId = alertProviderId << 8;
    alertProviderId |= (bitstream_ptr[3] &0xff);
    messageReceived->alertProviderId = alertProviderId;


    //**********************************
    // notification_area
    //**********************************
    int lat_ns=0, lat_deg=0, lat_min=0, lat_sec=0, lon_ew=0, lon_deg=0, lon_min=0, lon_sec=0, radius=0;

    lat_ns |= (bitstream_ptr[4] &0x80)>>7;
    messageReceived->notifAreaLatNS = lat_ns;

    lat_deg |= (bitstream_ptr[4] &0x7f)<<1;
    lat_deg |= (bitstream_ptr[5] &0x80)>>7;
    messageReceived->notifAreaLatDeg = lat_deg;


    lat_min |= (bitstream_ptr[5] &0x7f);
    messageReceived->notifAreaLatMin = lat_min;

    lat_sec |= (bitstream_ptr[6] &0xfc)>>2;
    messageReceived->notifAreaLatSec = lat_sec;

    lon_ew |= (bitstream_ptr[6] &0x2)>>1;
    messageReceived->notifAreaLonEW = lon_ew;

    lon_deg |= (bitstream_ptr[6] &0x1)<<7;
    lon_deg |= (bitstream_ptr[7] &0xfe)>>1;
    messageReceived->notifAreaLonDeg = lon_deg;

    lon_min |= (bitstream_ptr[7] &0x1)<<7;
    lon_min |= (bitstream_ptr[8] &0xfc)>>2;
    messageReceived->notifAreaLonMin = lon_min;

    lon_sec |= (bitstream_ptr[8] &0x3)<<4;
    lon_sec |= (bitstream_ptr[9] &0xf0)>>4;
    messageReceived->notifAreaLonSec = lon_sec;

    radius |= (bitstream_ptr[9] &0xf);
    messageReceived->notifAreaRadius = radius;



    //**********************************
    // priority
    //**********************************
    int priority = 0;
    priority |= (bitstream_ptr[10] &0xf0)>>4;
    messageReceived->messagePriority = priority;

    //**********************************
    // issuer
    //**********************************
    int issuer = 0;
    issuer |= (bitstream_ptr[10] &0xf);
    issuer = issuer << 8;
    issuer |= (bitstream_ptr[11] &0xff);
    messageReceived->messageIssuer = issuer;

    //**********************************
    // category + NHT
    //**********************************
    int category = 0;
    category |= (bitstream_ptr[12] &0xf0)>>4;
    messageReceived->messageCategory = category;

    int nht = 0;
    nht |= (bitstream_ptr[12] &0xf);


    //**********************************
    // validity start
    //**********************************
    int val_start_year=0,val_start_month=0,val_start_day=0,val_start_hour=0,val_start_min=0,val_start_sec=0;

    val_start_year |= (bitstream_ptr[13] &0xfc)>>2;
    messageReceived->valStartYear = val_start_year;

    val_start_month |= (bitstream_ptr[13] &0x3)<<2;
    val_start_month |= (bitstream_ptr[14] &0xc0)>>6;
    messageReceived->valStartMonth = val_start_month;

    val_start_day |= (bitstream_ptr[14] &0x3E)>>1;
    messageReceived->valStartDay = val_start_day;

    val_start_hour |= (bitstream_ptr[14] &0x1)<<4;
    val_start_hour |= (bitstream_ptr[15] &0xf0)>>4;
    messageReceived->valStartHour = val_start_hour;

    val_start_min |= (bitstream_ptr[15] &0xf)<<2;
    val_start_min |= (bitstream_ptr[16] &0xc0)>>6;
    messageReceived->valStartMin = val_start_min;

    val_start_sec |= (bitstream_ptr[16] &0x3f);
    messageReceived->valStartSec = val_start_sec;


    //**********************************
    // validity end
    //**********************************
    int val_end_year=0,val_end_month=0,val_end_day=0,val_end_hour=0,val_end_min=0,val_end_sec=0;

    val_end_year |= (bitstream_ptr[17] &0xfc)>>2;
    messageReceived->valEndYear = val_end_year;

    val_end_month |= (bitstream_ptr[17] &0x3)<<2;
    val_end_month |= (bitstream_ptr[18] &0xc0)>>6;
    messageReceived->valEndMonth = val_end_month;

    val_end_day |= (bitstream_ptr[18] &0x3E)>>1;
    messageReceived->valEndDay = val_end_day;

    val_end_hour |= (bitstream_ptr[18] &0x1)<<4;
    val_end_hour |= (bitstream_ptr[19] &0xf0)>>4;
    messageReceived->valEndHour = val_end_hour;

    val_end_min |= (bitstream_ptr[19] &0xf)<<2;
    val_end_min |= (bitstream_ptr[20] &0xc0)>>6;
    messageReceived->valEndMin = val_end_min;

    val_end_sec |= (bitstream_ptr[20] &0x3f);
    messageReceived->valEndSec = val_end_sec;



    //converting date data to timestamp format
    struct tm t_start;
    time_t timestamp_start;
    t_start.tm_year = val_start_year + 100;
    t_start.tm_mon = val_start_month-1;
    t_start.tm_mday = val_start_day;
    t_start.tm_hour = val_start_hour+2;
    t_start.tm_min = val_start_min;
    t_start.tm_sec = val_start_sec;
    t_start.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    timestamp_start = mktime(&t_start);
    messageReceived->valStartTimestamp = timestamp_start;

    struct tm t_end;
    time_t timestamp_end;
    t_end.tm_year = val_end_year + 100;
    t_end.tm_mon = val_end_month-1;
    t_end.tm_mday = val_end_day;
    t_end.tm_hour = val_end_hour+2;
    t_end.tm_min = val_end_min;
    t_end.tm_sec = val_end_sec;
    t_end.tm_isdst = -1;        // Is DST on? 1 = yes, 0 = no, -1 = unknown
    timestamp_end = mktime(&t_end);
    messageReceived->valEndTimestamp = timestamp_end;

    return 0;
}
