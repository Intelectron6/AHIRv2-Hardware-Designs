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
    
    srand(420);
	int idx;
	for(idx = 0; idx < 500; idx++)
	{
		float x1, x2;
        float y;

        if (idx % 2 == 0)
        {   
            x1 = ((float) (rand() % 10))/((float) 5000);
            x2 = ((float) (rand() % 1000))/((float) 50) + 0.1;
        }
		
        else
        {   
            x1 = ((float) (rand() % 1000))/((float) 50);
            x2 = ((float) (rand() % 10))/((float) 5000) + 0.1;
        }
        
		#ifdef SW
		fpd(x1,x2,&y);
        #else
		y = fpd(x1,x2);
        #endif

        float z = (y >= x1/x2) ? (y - x1/x2) : (x1/x2 - y);
 
		if (z > 0.00001) 
			fprintf(stderr, "Imperfect: %f / %f = %f; expected %f.\n", x1,x2,y,x1/x2);

        else
            fprintf(stderr, "Valid: %f / %f = %f; expected %f.\n", x1,x2,y,x1/x2);
	}
	
	return(0);
}
