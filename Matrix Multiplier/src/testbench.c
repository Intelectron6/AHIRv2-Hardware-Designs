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

#define N 8
#define N2 64

uint32_t matrix1[N][N] = {{12, 5, 142, 59, 301, 583, 78, 981},
					 {23, 115, 784, 58, 391, 102, 718, 912},
					 {120, 592, 482, 52, 636, 230, 805, 108},
					 {89, 100, 8, 64, 11, 10, 37, 902},
					 {503, 1, 48, 403, 323, 923, 758, 901},
					 {403, 205, 289, 483, 602, 19, 328, 712},
					 {177, 978, 4, 20, 31, 793, 598, 126},
					 {39, 54, 796, 585, 781, 402, 668, 777}};
				
uint32_t matrix2[N][N] = {{122, 15, 216, 69, 642, 583, 78, 98},
					 {93, 125, 574, 888, 901, 102, 718, 139},
					 {10, 254, 412, 25, 306, 231, 120, 717},
					 {894, 101, 800, 622, 143, 120, 137, 572},
					 {722, 12, 78, 143, 233, 93, 758, 160},
					 {533, 695, 282, 83, 602, 391, 328, 779},
					 {57, 728, 502, 340, 321, 793, 791, 425},
					 {396, 514, 96, 432, 72, 282, 592, 996}};

uint32_t outputs_hw[N2];

uint32_t outputs_sw[N2];

int all_done = 0;

void SendInputs ()
{
	uint32_t elements[N2];
	int i, j;

	for(i = 0; i < N; i++)
	{
        for(j = 0; j < N; j++) 
            elements[N*i + j] = matrix1[i][j];
	}
	write_uint32_n ("input_data", elements, N2);
	
	for(i = 0; i < N; i++)
	{
        for(j = 0; j < N; j++) 
            elements[N*i + j] = matrix2[i][j];
	}
    write_uint32_n ("input_data", elements, N2);
}

void ReceiveResults ()
{
    int i, j, k;

	read_uint32_n ("output_data", outputs_hw, N2);

	for (i = 0; i < N; i++)
	{
        for (j = 0; j < N; j++)
        {
            outputs_sw[N*i + j] = 0;
            for (k = 0; k < N; k++) 
                outputs_sw[N*i + j] += matrix1[i][k] * matrix2[k][j];
        }
    }

	int correct = 0;
	for(i = 0; i < N; i++)
	{
		for(j = 0; j < N; j++)
		{
        	if (outputs_sw[N*i + j] != outputs_hw[N*i + j])
        		correct++;
        		//fprintf(stderr, "Expected result = %lu\n", outputs_sw[N*i + j]);
        		//fprintf(stderr, "Actual result =  %lu\n", outputs_hw[N*i + j]);
        		//fprintf(stderr, "\n");
        }
	}
	
	if (correct == 0)
		fprintf(stderr, "All good!!! :-) \n");
	else
		fprintf(stderr, "There are errors... :-( \n");
    all_done = 1;
}

DEFINE_THREAD(SendInputs)
DEFINE_THREAD(ReceiveResults)

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

    PTHREAD_DECL(SendInputs);
    PTHREAD_DECL(ReceiveResults);
	
    PTHREAD_CREATE(SendInputs);
	PTHREAD_CREATE(ReceiveResults);

    matmul();

    while(!all_done)
	{
	}

	return(0);
}
