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
    
    srand(69);
	int idx;
	for(idx = 0; idx < 1000; idx++)
	{
		uint8_t x1, x2;
        uint8_t y, z;
		x1 = rand() % 255;
        x2 = (rand() % 254) + 1;
        
		ssd(x1,x2,&y,&z);

		if (y != x1/x2 || z != x1%x2)
		{
			fprintf(stderr, "Error: %d / %d = %d and %d; expected %d and %d.\n", x1,x2,y,z,x1/x2,x1%x2);
			_err_ = 1;
		}
	}
	
	fprintf(stderr,"All done: %s\n", _err_ ? "Failure :-(" : "Success :-)");
	return(0);
}
