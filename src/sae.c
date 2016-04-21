/*
 * sae.c
 *
 *  Created on: 12/apr/2016
 *      Author: ronga
 */

#include "sae.h"
#include <stdlib.h>
#include <time.h>

//
// timegm
//

inline int32_t is_leap(int32_t year)
{
  if(year % 400 == 0)
  return 1;
  if(year % 100 == 0)
  return 0;
  if(year % 4 == 0)
  return 1;
  return 0;
}

inline int32_t days_from_0(int32_t year)
{
  year--;
  return 365 * year + (year / 400) - (year/100) + (year / 4);
}

inline int32_t days_from_1970(int32_t year)
{
  int days_from_0_to_1970 = days_from_0(1970);
  return days_from_0(year) - days_from_0_to_1970;
}

inline int32_t days_from_1jan(int32_t year,int32_t month,int32_t day)
{
  static const int32_t days[2][12] =
  {
    { 0,31,59,90,120,151,181,212,243,273,304,334},
    { 0,31,60,91,121,152,182,213,244,274,305,335}
  };
  return days[is_leap(year)][month-1] + day - 1;
}

inline time_t internal_timegm(struct tm const *t)
{
  int year = t->tm_year + 1900;
  int month = t->tm_mon;
  if(month > 11)
  {
    year += month/12;
    month %= 12;
  }
  else if(month < 0)
  {
    int years_diff = (-month + 11)/12;
    year -= years_diff;
    month+=12 * years_diff;
  }
  month++;
  int day = t->tm_mday;
  int day_of_year = days_from_1jan(year,month,day);
  int days_since_epoch = days_from_1970(year) + day_of_year;

  time_t seconds_in_day = 3600 * 24;
  time_t result = seconds_in_day * days_since_epoch + 3600 * t->tm_hour + 60 * t->tm_min + t->tm_sec;

  return result;
}



void saeFunction(struct inputData *datiIn, struct outputData *datiOut){
     struct messageFields messageReceived = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

     if(datiIn->txRequest==0) parseMessage(datiIn->message, &messageReceived); // message need to be parsed only in use case 1, when txRequest = 0
     applyLogic(&messageReceived, datiIn, datiOut);
}

//*****************************************************************************
//                     APPLY LOGIC
//*****************************************************************************
int applyLogic(struct messageFields *messageReceived, struct inputData *datiIn, struct outputData *datiOut){

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
			datiOut->outputMsgLenght = -1;
			datiOut->doNothing = 0;
		} else {
			//TEST CASE 2.2
			//printf("TEST CASE 2.2 \n nothing to do, tx queue is still valid \n $i <= 0 || %i > %i\n", datiIn->valEndTimestamp, datiIn->valEndTimestamp, datiIn->onBoardTime);
			datiOut->doNothing = 1; // nothing to do, tx queue is still valid
			datiOut->eraseTxQueue = 0;
		}
	} else { // TX Request 0
		//Use Case 1
		if(messageReceived->messageReference>0){ // CANCEL
			if(datiIn->messageId == messageReceived->messageReference) { // check if messagerefernce of cancel message is really the one in the tx queue
				//TEST CASE 1.4
				//printf("TEST CASE 1.4 \n onboard tx queue need to be erased \n %i > 0 && %i == %i\n", messageReceived->messageReference, datiIn->messageId, messageReceived->messageReference);
				datiOut->eraseTxQueue = 1; // onboard tx queue need to be erased
				datiOut->messageId = -1;
				datiOut->messagePriority = -1;
				datiOut->valEndTimestamp = -1;
				datiOut->outputMsgLenght = -1;
				datiOut->doNothing = 0;
			} else {//stored message is not the one to be deleted
				//TEST CASE 1.5
				//printf("TEST CASE 1.5 \n stored message is not the one to be deleted \n %i > 0 && %i != %i\n", messageReceived->messageReference, datiIn->messageId, messageReceived->messageReference);
				datiOut->doNothing = 1;
				datiOut->eraseTxQueue = 0;
			}
		} else { // ALERT | ULTRASHORT
			if(messageReceived->messagePriority >= datiIn->messagePriority){ // new uploaded message has higher priority than previous stored message
				//TEST CASE 1.3
				//printf("TEST CASE 1.3 \n new uploaded message has higher priority than previous stored message \n %i <= 0 && %i >= %i\n", messageReceived->messageReference, messageReceived->messagePriority, datiIn->messagePriority);
				datiOut->messageId = messageReceived->messageId;
				datiOut->messagePriority = messageReceived->messagePriority;
				datiOut->valEndTimestamp = messageReceived->valEndTimestamp;
				datiOut->outputMsgLenght = messageReceived->MessageLength;
				datiOut->doNothing = 0;
				datiOut->eraseTxQueue = 0;
			} else { // new priority less than stored
				if(datiIn->valEndTimestamp <= datiIn->onBoardTime){ // old message not valid
					//TEST CASE 1.1
					datiOut->messageId = messageReceived->messageId;
					datiOut->messagePriority = messageReceived->messagePriority;
					datiOut->valEndTimestamp = messageReceived->valEndTimestamp;
					datiOut->outputMsgLenght = messageReceived->MessageLength;
					datiOut->doNothing = 0;
					datiOut->eraseTxQueue = 0;
				} else {//  old message valid
					//TEST CASE 1.2
					datiOut->doNothing = 1;
					datiOut->eraseTxQueue = 0;
				}
			}
		} // not a cancel
	} // tx request 0
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
    t_start.tm_hour = val_start_hour;
    t_start.tm_min = val_start_min;
    t_start.tm_sec = val_start_sec;
    t_start.tm_isdst = -1;
    //timestamp_start = mktime(&t_start) + t_start.tm_gmtoff;
    timestamp_start = internal_timegm(&t_start);
    messageReceived->valStartTimestamp = timestamp_start;

    struct tm t_end;
    time_t timestamp_end;
    t_end.tm_year = val_end_year + 100;
    t_end.tm_mon = val_end_month-1;
    t_end.tm_mday = val_end_day;
    t_end.tm_hour = val_end_hour;
    t_end.tm_min = val_end_min;
    t_end.tm_sec = val_end_sec;
    t_start.tm_isdst = -1;
    //timestamp_end = mktime(&t_end) + t_end.tm_gmtoff;
    timestamp_end = internal_timegm(&t_end);
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
    message_length |= (bitstream_ptr[(22+messageIndexShift)]&0xff)<<12;
    message_length |= (bitstream_ptr[(23+messageIndexShift)]&0xff)<<4;
    message_length |= bitstream_ptr[(24+messageIndexShift)] &0xf0;

    if (nht1 == 4)  // NO CAP MESSAGE
    	messageReceived->MessageLength = message_length + MAMES_NOCAP_HEADER_LENGTH;
    else
    	messageReceived->MessageLength = message_length + MAMES_CAP_HEADER_LENGTH;

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
    t_start.tm_hour = val_start_hour;
    t_start.tm_min = val_start_min;
    t_start.tm_sec = val_start_sec;
    t_start.tm_isdst = -1;
    // timestamp_start = mktime(&t_start) + t_start.tm_gmtoff;
    timestamp_start = internal_timegm(&t_start);

    messageReceived->valStartTimestamp = timestamp_start;

    struct tm t_end;
    time_t timestamp_end;
    t_end.tm_year = val_end_year + 100;
    t_end.tm_mon = val_end_month-1;
    t_end.tm_mday = val_end_day;
    t_end.tm_hour = val_end_hour;
    t_end.tm_min = val_end_min;
    t_end.tm_sec = val_end_sec;
    t_start.tm_isdst = -1;
    //timestamp_end = mktime(&t_end) + t_end.tm_gmtoff;
    timestamp_end = internal_timegm(&t_end);
    messageReceived->valEndTimestamp = timestamp_end;

	messageReceived->MessageLength = MAMES_USHORT_LENGTH;

    return 0;
}


