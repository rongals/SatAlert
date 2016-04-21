/*
 * sae.h
 *
 *  Created on: 12/apr/2016
 *      Author: ronga
 */

#ifndef SAE_H_
#define SAE_H_

#define MAMES_NOCAP_HEADER_LENGTH 	28
#define MAMES_CAP_HEADER_LENGTH 	25
#define MAMES_USHORT_LENGTH			25

struct inputData{
    char *message;
    int messageId;
    int messagePriority;
    int valEndTimestamp;
    int onBoardTime;
    _Bool txRequest;
};
struct outputData{
    char *message;
    int messageId;
    int messagePriority;
    int valEndTimestamp;
    int outputMsgLenght;
    _Bool eraseTxQueue;
    _Bool doNothing;
};
struct messageFields{
    int messageId;
    int messagePriority;
    int messageType;
    int messageReference;
    int alertProviderId;
    int protocolVersion;
    int messageIssuer;
    int messageCategory;
    int languageId;
    int MessageLength;
    int valStartTimestamp;
    int valStartYear;
    int valStartMonth;
    int valStartDay;
    int valStartHour;
    int valStartMin;
    int valStartSec;
    int valEndTimestamp;
    int valEndYear;
    int valEndMonth;
    int valEndDay;
    int valEndHour;
    int valEndMin;
    int valEndSec;
    int notifAreaLatNS;
    int notifAreaLatDeg;
    int notifAreaLatMin;
    int notifAreaLatSec;
    int notifAreaLonEW;
    int notifAreaLonDeg;
    int notifAreaLonMin;
    int notifAreaLonSec;
    int notifAreaRadius;
};


void saeFunction(struct inputData *datiIn, struct outputData *datiOut);
int parse_ultrashort(char *bitstream_ptr, struct messageFields *messageFields);
int parse_cancel(char *bitstream_ptr, struct messageFields *messageFields);
int parse_alert(char *bitstream_ptr, struct messageFields *messageFields);
int parseMessage(char *received_message, struct messageFields *messageFields);
int applyLogic(struct messageFields *messageReceived, struct inputData *datiIn, struct outputData *datiOut);

#endif /* SAE_H_ */
