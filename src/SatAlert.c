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


int main(int argc, char *argv[]){


	initMyRandom();
	int i;
	char c;
	char gen_message[28];
	//char messageToBeSent[1500];
	//int msgTypeSequence[15] = {0,4,4,4,4,4,4,4,4,4,4,4,4,4,0};
	//int msgPrioritySequence[15] = {5,0,6,7,6,8,8,6,5,0,0,0,0,0,0};
	struct inputData dataIn = {gen_message, -1, -1, -1, -1, 0};
	struct outputData dataOut = {-1, -1, -1, -1, 0, 0};
	int isMAMES = 0;

	if (argc != 2 ) {
		printf("usage: SatAlert 0=mames 1=isae\n");
		exit(1);
	}

	switch (atoi(argv[1])) {
	case 0:
		isMAMES = 1;
		break;
	default:
		break;
	}

	if (isMAMES) {

		char outbuf[1500];
		int outbuf_l;

		generateMAMES(101,10,1,10,outbuf,&outbuf_l);

		for (i=0;i<1500;i++) {
			c=outbuf[i];
			if (isprint(c))
					printf("%c",c);
				else
					printf("0x%x ",c);
		}
		printf("\n");


	}
	else {
		for(i=0;i<15;i++){

			time_t now = time(NULL);

			dataIn.onBoardTime = now;

			////int first_upload =  FIRST_UPLOAD;
			////if(first_upload == 1)
			////dataIn.messageId = MESSAGE_ID;
			////dataIn.messagePriority = PRIORITY;
			////dataIn.valEndTimestamp = VALIDITY_END;
			////dataIn.txRequest = 0;


			printf("\n\n\n\n\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n Step %i\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n", i);
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


			printf("\n-----------------------------------\ninputData struct\n-----------------------------------\n");
			printf("messageId = %i\n", dataIn.messageId);
			printf("messagePriority = %i\n", dataIn.messagePriority);
			printf("valEndTimestamp = %i\n", dataIn.valEndTimestamp);
			printf("onBoardTime = %i\n", dataIn.onBoardTime);
			printf("txRequest = %i\n", dataIn.txRequest);
			printf("-----------------------------------\n");


//			printf("rand_type = %i\n", rand_type);
//			printf("rand_priority = %i\n", rand_priority);
//			printf("rand_bool_nocap = %i\n", rand_bool_nocap);
//			printf("rand_validityEnd = %i\n", rand_validityEnd);

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

				if(dataOut.doNothing)
					printf("\n********************************\nResult:\nTrasmission queue will not be overwrited!\n");
				else {

					if (dataOut.eraseTxQueue) { // ERASE

						printf("\n********************************\nResult:\nStored message is not valid anymore:"
								" trasmission queue will not be erased!\n");

					} else {// STORE

						printf("\n********************************\n"
								"Result:\nIncoming message stored in "
								"trasmission queue: priority %i and valid until %i!\n",dataOut.messagePriority,
								dataOut.valEndTimestamp);
					}

					dataIn.messageId = dataOut.messageId;
					dataIn.messagePriority = dataOut.messagePriority;
					dataIn.valEndTimestamp = dataOut.valEndTimestamp;
				}


			}
			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}



//int main(void) {
//	puts("Hello World"); /* prints Hello World */
//	return EXIT_SUCCESS;
//}
