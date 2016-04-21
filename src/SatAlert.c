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
#include "GenerateMAMESMessage.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>


#define VALIDITY_OFFSET 10
#define PRIORITY 1

int print_message(char *buffer, int buffer_l) {
	char c;
	int i;
	for (i=0;i<buffer_l;i++) {
		c=buffer[i];
		if (isprint(c))
			printf("%c",c);
		else
			printf("0x%x ",c);
	}
	printf("\n");

	return EXIT_SUCCESS;
}

int main(int argc, char *argv[]){

	char outbuf[1500];
	int outbuf_l;
	initMyRandom();
	int i;
	char gen_message[28];
	struct inputData dataIn = {gen_message, -1, -1, -1, -1, -1, 0};
	struct outputData dataOut = {-1, -1, -1, -1, 0, 0};

	if (argc != 2 ) {
		printf("usage: SatAlert <mode>\nModes:\n"
				"0(default)=auto sequence\n"
				"1=CANCEL\n"
				"2=ULTRASHORT\n"
				"3=CAP\n"
				"4=NO CAP\n\n");
		exit(1);
	}


	switch (atoi(argv[1])) {
	case 1: //CANCEL
		generateMAMES(CANCEL,MESSAGE_ID,PRIORITY,VALIDITY_OFFSET,outbuf,&outbuf_l);
		print_message(outbuf, outbuf_l);
		break;
	case 2: //US
		generateMAMES(ULTRASHORT,MESSAGE_ID,PRIORITY,VALIDITY_OFFSET,outbuf,&outbuf_l);
		print_message(outbuf, outbuf_l);
		break;
	case 3: //CAP
		generateMAMES(CAP,MESSAGE_ID,PRIORITY,VALIDITY_OFFSET,outbuf,&outbuf_l);
		print_message(outbuf, outbuf_l);
		break;
	case 4: //NOCAP
		generateMAMES(NOCAP,MESSAGE_ID,PRIORITY,VALIDITY_OFFSET,outbuf,&outbuf_l);
		print_message(outbuf, outbuf_l);
		break;
	default: // 0

		for(i=0;i<15;i++){

			time_t now = time(NULL);

			dataIn.onBoardTime = now;

			printf("\n\n\n\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n Step %i\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n", i);
			int status = 0;
			int rand_type = get_random_int(4)*2; //to be removed
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


			printf("\n-----------------------------------\ninputData struct\n-----------------------------------\n");
			printf("messageId = %i\n", dataIn.messageId);
			printf("messagePriority = %i\n", dataIn.messagePriority);
			printf("valEndTimestamp = %i\n", dataIn.valEndTimestamp);
			printf("onBoardTime = %i\n", dataIn.onBoardTime);
			printf("storedMsgLength = %i\n", dataIn.storedMsgLength);
			printf("txRequest = %i\n", dataIn.txRequest);
			printf("-----------------------------------\n");

			if (rand_type < 6)
				status = generateMessage(rand_type, rand_priority, rand_bool_nocap,
						rand_validityStart, rand_validityEnd, gen_message);

			if (status == 1) printf("Priority not in range [0,15]\n");
			else if(status == 2) printf("Validity timestamps need to be positive\n");
			else if(status == 3) printf("Validity Start time must be greater than Validity End time\n");
			else if(status == 4) printf("Message type can be only in range [0,4]\n0=ultrashort\n1=mames update\n2=cancel\n3=mames ack\n4=mames alert\n");
			else if(status == 5) printf("Kind of message not implemented yet\n");
			else { // status not 1,2,3,4 or 5

				dataIn.message = gen_message;

				printf("\nSending informations to SAE logic...\n");
				saeFunction(&dataIn, &dataOut);


				printf("\n-----------------------------------\noutputData struct\n-----------------------------------\n");

				printf("messageId = %i\n", dataOut.messageId);
				printf("messagePriority = %i\n", dataOut.messagePriority);
				printf("valEndTimestamp = %i\n", dataOut.valEndTimestamp);
				printf("outputMsgLenght = %i\n", dataOut.outputMsgLenght);
				printf("eraseTxQueue = %i\n", dataOut.eraseTxQueue);
				printf("doNothing = %i\n", dataOut.doNothing);

				if (dataOut.doNothing) {

					if (dataIn.storedMsgLength == NOT_APPLICABLE) {
						printf("\n********************************\n"
								"Result:\nEmpty storage");
						printf("\n********************************\n");

					} else {
						printf("\n********************************\n"
								"Result:\nStored message maintained");

						if (dataIn.txRequest)
							printf(" and transmitted\n");
						else
							printf("\n");

						printf("********************************\n");
					}
				}
				else {

					if (dataOut.eraseTxQueue) { // ERASE

						printf("\n********************************\n"
								"Result:\nStored message erased");
						printf("\n********************************\n");


					} else {// STORE

						printf("\n********************************\n"
								"Result:\nIncoming message stored\n"
								"Priority: %i\n"
								"Valid until %i\n"
								"Length %i",
								dataOut.messagePriority,
								dataOut.valEndTimestamp,
								dataOut.outputMsgLenght);
						printf("\n********************************\n");
					}

					dataIn.messageId = dataOut.messageId;
					dataIn.messagePriority = dataOut.messagePriority;
					dataIn.valEndTimestamp = dataOut.valEndTimestamp;
					dataIn.storedMsgLength = dataOut.outputMsgLenght;
				}


			} // else status not 12345
			sleep(1);
		} // for loop
	} // switch
	return EXIT_SUCCESS;
}



//int main(void) {
//	puts("Hello World"); /* prints Hello World */
//	return EXIT_SUCCESS;
//}
