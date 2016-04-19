/*
 * GenerateMAMESMessage.h
 *
 *  Created on: 19/apr/2016
 *      Author: jayousi
 */

#ifndef GENERATEMAMESMESSAGE_H_
#define GENERATEMAMESMESSAGE_H_



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


#define TIME_ADVANCE 2 // seconds
#define POFF_NOCAP 28
#define POFF_CAP 25


enum mames_l {MAMES_500, MAMES_1500};


void initMyRandom();
int get_random_int(int limit);
char *int2bin(int a, char *buffer, int buf_size);
int generate_ultrashort(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generate_cancel(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generate_cap(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generate_nocap(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generateMessage(int msg_type, int priority, _Bool msg_nocap, int validity_start, int validity_end, char* generated_message);

int generateMAMES(
		int messageID,
		int priority,
		_Bool msg_nocap,
		int validity_offset,
		char *generated_message,
		int *generated_message_length);


#endif /* GENERATEMAMESMESSAGE_H_ */
