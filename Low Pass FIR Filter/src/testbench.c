#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
//-----------------------------
#include <pthreadUtils.h>
#include <Pipes.h>
#include <pipeHandler.h>
#ifndef SW
#include "SockPipes.h"
#include "vhdlCStubs.h"
#else
#include "aa_c_model.h"
#endif

#define ORDER 16

float in_buffer[2*ORDER];
float out_buffer[ORDER];

void Exit(int sig)
{
	fprintf(stderr, "## Break! ##\n");
	exit(0);
}

void Sender()
{
	// send filter coefficients.
	//   moving average across ORDER samples.
	int I;
	for(I = 0; I < ORDER; I++)
	{
		write_float32 ("in_data", 1.0/ORDER);	
#ifdef VERBOSE
		fprintf(stderr,"Sent coefficient %d %f\n", I, 1.0/ORDER);
#endif
	
		// collect data samples:  ORDER/4 1's followed by zeros.
		in_buffer[I] = ((I < (ORDER/4)) ? 1.0 : 0.0);
		in_buffer[I + ORDER] = 0.0;
	}

	for(I = 0; I < 2*ORDER; I++)
	{
		write_float32 ("in_data", in_buffer[I]);
#ifdef VERBOSE
		fprintf(stderr,"Sent in_data %d %f\n", I, in_buffer[I]);
#endif
	}
}
DEFINE_THREAD(Sender);
void Receiver()
{

	int I = 0;
	for(I = 0; I < 2*ORDER; I++)
	{
		float Y = read_float32("out_data");
		fprintf(stdout, "%d %f %f\n", I, in_buffer[I], Y);
	}
}
DEFINE_THREAD(Receiver);




	
int main(int argc, char* argv[])
{
	int _err_ = 0;
	signal(SIGINT,  Exit);
  	signal(SIGTERM, Exit);

	int I;
#ifdef SW
#ifdef VERBOSE
	start_daemons(stdout, 0);	
#else
	start_daemons(NULL, 0);	
#endif
#endif

	PTHREAD_DECL(Receiver);
	PTHREAD_CREATE(Receiver);

	PTHREAD_DECL(Sender);
	PTHREAD_CREATE(Sender);

	PTHREAD_JOIN(Receiver);
	PTHREAD_JOIN(Sender);

	return(0);
}
