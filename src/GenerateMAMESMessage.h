/*
 * GenerateMAMESMessage.h
 *
 *  Created on: 19/apr/2016
 *      Author: jayousi
 */

#ifndef GENERATEMAMESMESSAGE_H_
#define GENERATEMAMESMESSAGE_H_



#define STAND_VERSION 1

//0=ultrashort, 1=mames update, 2=cancel, 3=mames ack, 4=mames alert
#define ULTRASHORT_MSG_TYPE 0
#define CANCEL_MSG_TYPE 2
#define ALERT_MSG_TYPE 4
#define MESSAGE_ID 0x1e3

#define MAMES_TEST_PROVIDER_ID 1
#define MAMES_TEST_ALERT_ISSUER_ID 3
#define MAMES_LANGUAGE_ENG 43

#define NOTIFICATION_AREA1 9809478 //100101011010111001000110
#define NOTIFICATION_AREA2 1457399 //000101100011110011110111

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

enum test_message_type {CANCEL, ULTRASHORT, CAP, NOCAP};


void initMyRandom();
int get_random_int(int limit);
char *int2bin(int a, char *buffer, int buf_size);
int generate_ultrashort(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generate_cancel(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generate_cap(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generate_nocap(int priority, int validity_start, int validity_end, char* bitstream_ptr);
int generateMessage(int msg_type, int priority, _Bool msg_nocap, int validity_start, int validity_end, char* generated_message);


int generateMAMES(
		enum test_message_type test_message,
		int messageID,
		int priority,
		int validity_offset,
		char *generated_message,
		int *generated_message_length);


#endif /* GENERATEMAMESMESSAGE_H_ */
