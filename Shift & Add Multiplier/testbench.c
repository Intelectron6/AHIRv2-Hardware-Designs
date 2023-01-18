#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <pthreadUtils.h>
#include <Pipes.h>
#include <pipeHandler.h>
#ifndef SW
#include "SockPipes.h"
#include "vhdlCStubs.h"
#else
#include "aa_c_model.h"
#endif

void Exit(int sig)
{
	fprintf(stderr, "## Break! ##\n");
	exit(0);
}
	
int main(int argc, char* argv[])
{
	int _err_ = 0;
	signal(SIGINT,  Exit);
  	signal(SIGTERM, Exit);

    #ifdef SW
	start_daemons(stdout, 0);	
    #endif

	uint8_t idx1, idx2;
    for(idx1 = 0; idx1 < 7; idx1++)
	for(idx2 = 0; idx2 < 7; idx2++)
	{
		uint8_t x1, x2;
        uint16_t y;
		x1 = idx1;
        x2 = idx2;
        
        #ifdef SW
		sam(x1,x2,&y);
        #else
		y = sam(x1,x2);
        #endif

		if (y != x1*x2)
		{
			fprintf(stderr, "Error: %d * %d = %d, expected %d.\n", x1,x2,y,x1*x2);
			_err_ = 1;
		}
	}

    uint8_t x1, x2;
    uint16_t y;
    x1 = 255;
    x2 = 255;
        
    #ifdef SW
    sam(x1,x2,&y);
    #else
	y = sam(x1,x2);
    #endif

	if (y != x1*x2)
	{
		fprintf(stderr, "Error: %d * %d = %d, expected %d.\n", x1,x2,y,x1*x2);
		_err_ = 1;
	}
	
	fprintf(stderr,"All done: %s\n", _err_ ? "Failure :-(" : "Success :-)");
	return(0);
}
