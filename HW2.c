#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#define BUFSIZE 1024

void readFunction(int buffSize, int fdIn, int* counter, char desired);
void readFunction(int buffSize, int fdIn, int* counter, char desired)
{
    char buf[buffSize];
    int cnt;
    while ((cnt=read(fdIn, buf, buffSize)) > 0)
    { 
        int size = sizeof(buf);
		printf("The cnt is %d\n", cnt);
        printf("the size of the buff is %d\n",size);
	    for (int i = 0; i < cnt; i++)
	    {
		    if (buf[i]==desired)
		    {
			    (*counter)++;
			    printf("The counter is now %d\n",*counter);
		    }   
	    }
    }
}


void mmapFunction();
void mmapFunction()
{

}

int main(int argc, char *argv[])
{
    int fdIn, i, cnt;
    char desired = *argv[2];   
    int counter = 0;
    int buffSize = BUFSIZE;

    bool runRead = true;


    //This code does what we need to do based on arg 4
    if (argc == 4)
    {
        if (!(strcmp(argv[3],"mmap")))
        {
            runRead = false;
        }
        else
        {
            buffSize = atoi(argv[3]);
            if (buffSize > 8192)
            {
                printf("sorry friend, we're setting it to 8192\n");
                buffSize = 8192;

            }
        }
    }


    if (argc < 2) 
    { fdIn = 0;  /* just read from stdin */ }
    else if ((fdIn = open(argv[1], O_RDONLY)) < 0) 
    {
	fprintf(stderr, "file open\n");
	exit(1);
    }

    //Run this code if we are reading
    if (runRead)
    {
        readFunction(buffSize, fdIn, &counter, desired);

    } 
    else /* we will be using the mmap code */
    {
        //Code for mmap
        printf("MMAP CODE HERE BABY\n");
    }

    if (fdIn > 0){
        close(fdIn);
    }

    printf("occurebnces is %i\n", counter);
}
