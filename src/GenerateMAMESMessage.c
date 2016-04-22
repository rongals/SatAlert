#include "GenerateMAMESMessage.h"
#include "sae.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int messageCounter = MESSAGE_ID;

static char cap_message_payload[] =
		"<\?xml version = \"1.0\" encoding = \"UTF-8\"\?>"
		"<alert xmlns = \"urn:oasis:names:tc:emergency:cap:1.2\">"
		"<identifier>KSTO1055887203</identifier>"
		"<sender>KSTO@NWS.NOAA.GOV</sender>"
		"<sent>2003-06-17T14:57:00-07:00</sent>"
		"<status>Actual</status>"
		"<msgType>Alert</msgType>"
		"<scope>Public</scope>"
		"<info>"
		"<category>Met</category>"
		"<event>SEVERE THUNDERSTORM</event>"
		"<responseType>Shelter</responseType>"
		"<urgency>Immediate</urgency>"
		"<severity>Severe</severity>"
		"<certainty>Observed</certainty>"
		"<eventCode><valueName>SAME</valueName><value>SVR</value></eventCode>"
		"<expires>2003-06-17T16:00:00-07:00</expires>"
		"<senderName>NATIONAL WEATHER SERVICE SACRAMENTO CA</senderName>"
		"<headline>SEVERE THUNDERSTORM WARNING</headline>"
		"<description> AT 254 PM PDT...NATIONAL WEATHER SERVICE DOPPLER RADAR INDICATED A SEVERE THUNDERSTORM OVER SOUTH CENTRAL ALPINE COUNTY...OR ABOUT 18 MILES SOUTHEAST OF KIRKWOOD...MOVING SOUTHWEST AT 5 MPH. HAIL...INTENSE RAIN AND STRONG DAMAGING WINDS ARE LIKELY WITH THIS STORM.</description>"
		"<instruction>TAKE COVER IN A SUBSTANTIAL SHELTER UNTIL THE STORM PASSES.</instruction>"
		"<contact>BARUFFALDI/JUSKIE</contact>"
		"<area>"
		"<areaDesc>EXTREME NORTH CENTRAL TUOLUMNE COUNTY IN CALIFORNIA, "
		"EXTREME NORTHEASTERN CALAVERAS COUNTY IN CALIFORNIA, SOUTHWESTERN ALPINE COUNTY "
		"IN CALIFORNIA</areaDesc>"
		"<polygon>38.47,-120.14 38.34,-119.95 38.52,-119.74 38.62,-119.89 38.47,-120.14</polygon>"
		"</area></info></alert>";

static char nocap_message_payload[] = "AT 2:54 PM PDT...NATIONAL WEATHER SERVICE DOPPLER RADAR INDICATED"
		" A SEVERE THUNDERSTORM OVER SOUTH CENTRAL ALPINE COUNTY...OR ABOUT 18 MILES SOUTHEAST OF"
		" KIRKWOOD...MOVING SOUTHWEST AT 5 MPH. HAIL...INTENSE RAIN AND STRONG DAMAGING WINDS"
		" ARE LIKELY WITH THIS STORM";


static int cap_message_length = 1435;
static int nocap_message_length = 264;


//
// GENERATE MAMES
//
int generateMAMES(
		enum test_message_type test_message,
		int messageID,
		int priority,
		int validity_offset,
		char *generated_message,
		int *generated_message_length) {


	messageCounter = messageID;
	time_t validity_start = time(NULL) - TIME_ADVANCE;
	time_t validity_end = validity_start + validity_offset;

	switch (test_message) {
	case CANCEL:
		generateMessage(
				CANCEL_MSG_TYPE,
				priority,
				0, // no cap
				validity_start,
				validity_end,
				generated_message);

		break;
	case ULTRASHORT:
		generateMessage(
				ULTRASHORT_MSG_TYPE,
				priority,
				0, // no cap
				validity_start,
				validity_end,
				generated_message);
		break;
	case CAP: // Alert CAP 1500 bytes long
		generateMessage(
				ALERT_MSG_TYPE,
				priority,
				0, // cap!
				validity_start,
				validity_end,
				generated_message);
		memcpy(generated_message + POFF_CAP, cap_message_payload, cap_message_length);
		*generated_message_length = POFF_CAP + cap_message_length;
		break;
	default: // Non CAP alert of 500 bytes
		generateMessage(
				ALERT_MSG_TYPE,
				priority,
				1, // no cap
				validity_start,
				validity_end,
				generated_message);
		memcpy(generated_message + POFF_NOCAP, nocap_message_payload, nocap_message_length);
		*generated_message_length = POFF_NOCAP + nocap_message_length;
		break;
	}


	return 0;


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


    struct tm time_start;
    time_t validity_start_time_t = validity_start;
    //time_start = localtime(&validity_start_time_t);
    gmtime_r(&validity_start_time_t,&time_start);
    int val_start_year = time_start.tm_year-100;
    int val_start_month = time_start.tm_mon+1;
    int val_start_day = time_start.tm_mday;
    int val_start_hour = time_start.tm_hour;
    int val_start_min = time_start.tm_min;
    int val_start_sec = time_start.tm_sec;

    struct tm time_end;
    time_t validity_end_time_t = validity_end;
    //time_end = localtime(&validity_end_time_t);
    gmtime_r(&validity_end_time_t,&time_end);
    int val_end_year = time_end.tm_year-100;
    int val_end_month = time_end.tm_mon+1;
    int val_end_day = time_end.tm_mday;
    int val_end_hour = time_end.tm_hour;
    int val_end_min = time_end.tm_min;
    int val_end_sec = time_end.tm_sec;




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
    int gen_provider_id = MAMES_TEST_PROVIDER_ID;
    //int2bin(gen_message_id, buffer, BUF_SIZE - 1);
    //printf("gen_message_id = %s -> %i\n", buffer, gen_message_id);
    printf("gen_message_id = %i\n", gen_message_id);
    //int2bin(gen_provider_id, buffer, BUF_SIZE - 1);
    //if(verbose) printf("gen_provider_id = %s -> %i\n", buffer, gen_provider_id);
    printf("gen_provider_id = %i\n", gen_provider_id);

    gen_message_id = gen_message_id << 12;
    gen_message_id |= (gen_provider_id &0xfff);
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
    int gen_issuer = MAMES_TEST_ALERT_ISSUER_ID;
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
    int language_id = MAMES_LANGUAGE_ENG;
    //int2bin(language_id, buffer, BUF_SIZE - 1);
    //printf("language_id = %s -> %i\n", buffer, language_id);
    printf("language_id = %i\n", language_id);

    bitstream_ptr[22] = (TXT_NHT<<4);
    bitstream_ptr[22] |= (language_id>>8) &0xf;
    bitstream_ptr[23] = language_id &0xff;


     //********************************
    // payload length + NHT
    //********************************
    int payload_length = nocap_message_length;
    //int2bin(payload_length, buffer, BUF_SIZE - 1);
    //printf("payload_length = %s -> %i\n", buffer, payload_length);
    printf("payload_length = %i\n", payload_length);

    bitstream_ptr[24] = payload_length>>20;
    bitstream_ptr[25] = (payload_length>>12) &0xff;
    bitstream_ptr[26] = (payload_length>>4) &0xff;
    bitstream_ptr[27] = (payload_length &0xf)<<4;
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
//                     NO_CAP false (so it is CAP)
//*****************************************************************************
int generate_cap(int priority, int validity_start, int validity_end, char* bitstream_ptr){
    printf("********************************\n  CAP MESSAGE generated \n ********************************\n");
    int i;
    char buffer[BUF_SIZE];
    buffer[BUF_SIZE - 1] = '\0';
    char buffer8[9];
    buffer8[8] = '\0';


    struct tm time_start;
     time_t validity_start_time_t = validity_start;
     //time_start = localtime(&validity_start_time_t);
     gmtime_r(&validity_start_time_t,&time_start);
     int val_start_year = time_start.tm_year-100;
     int val_start_month = time_start.tm_mon+1;
     int val_start_day = time_start.tm_mday;
     int val_start_hour = time_start.tm_hour;
     int val_start_min = time_start.tm_min;
     int val_start_sec = time_start.tm_sec;

     struct tm time_end;
     time_t validity_end_time_t = validity_end;
     //time_end = localtime(&validity_end_time_t);
     gmtime_r(&validity_end_time_t,&time_end);
     int val_end_year = time_end.tm_year-100;
     int val_end_month = time_end.tm_mon+1;
     int val_end_day = time_end.tm_mday;
     int val_end_hour = time_end.tm_hour;
     int val_end_min = time_end.tm_min;
     int val_end_sec = time_end.tm_sec;


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
    int gen_provider_id = MAMES_TEST_PROVIDER_ID;
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
    int language_id = MAMES_LANGUAGE_ENG;
    int2bin(language_id, buffer, BUF_SIZE - 1);
    printf("language_id = %s -> %i\n", buffer, language_id);

    bitstream_ptr[19] = (CAP_NHT & 0xf)<<4;
    bitstream_ptr[19] |= language_id>>8;
    bitstream_ptr[20] = language_id &0xff;


     //********************************
    // message length + NHT
    //********************************
    int payload_length = cap_message_length;
    printf("payload_length = %s -> %i\n", buffer, payload_length);

    bitstream_ptr[21] = payload_length>>20;
    bitstream_ptr[22] = (payload_length>>12) &0xff;
    bitstream_ptr[23] = (payload_length>>4) &0xff;
    bitstream_ptr[24] = ((payload_length &0xf)<<4) &0xf0;
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
    int gen_provider_id = MAMES_TEST_PROVIDER_ID;
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

    struct tm time_start;
     time_t validity_start_time_t = validity_start;
     //time_start = localtime(&validity_start_time_t);
     gmtime_r(&validity_start_time_t,&time_start);
     int val_start_year = time_start.tm_year-100;
     int val_start_month = time_start.tm_mon+1;
     int val_start_day = time_start.tm_mday;
     int val_start_hour = time_start.tm_hour;
     int val_start_min = time_start.tm_min;
     int val_start_sec = time_start.tm_sec;

     struct tm time_end;
     time_t validity_end_time_t = validity_end;
     //time_end = localtime(&validity_end_time_t);
     gmtime_r(&validity_end_time_t,&time_end);
     int val_end_year = time_end.tm_year-100;
     int val_end_month = time_end.tm_mon+1;
     int val_end_day = time_end.tm_mday;
     int val_end_hour = time_end.tm_hour;
     int val_end_min = time_end.tm_min;
     int val_end_sec = time_end.tm_sec;



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
    int gen_provider_id = MAMES_TEST_PROVIDER_ID;
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
    int gen_issuer = MAMES_TEST_ALERT_ISSUER_ID;
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
    //printf("bitstream = %s\n", bitstream_ptr);
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
