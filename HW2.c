#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/mman.h>
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
			    ((*counter)++);
			    printf("The counter is now %d\n",*counter);
		    }   
	    }
    }
} 


void mmapFunction(int fdIn, char *pchFile, char desired, int* counter);
void mmapFunction(int fdIn, char *pchFile, char desired, int* counter)
{
    struct stat sb;
    if(fstat(fdIn, &sb) < 0){
	perror("Could not stat file to obtain its size");
	exit(1);
    }

    if ((pchFile = (char *) mmap (NULL, sb.st_size, PROT_READ, MAP_SHARED, fdIn, 0)) 
	== (char *) -1)	{
	perror("Could not mmap file");
	exit (1);
    }
    char* pchCopy = pchFile;
    for (int i = 0; i < sb.st_size; i++)
    {
        if (*pchCopy == desired)
        {
            (*counter)++;
        }
	    pchCopy++;
    }

    if(munmap(pchFile, sb.st_size) < 0)
    {
	perror("Could not unmap memory");
	exit(1);
    }
}

void forkFunction(int target);
void forkFunction(int target)
{
    int pIndex = 1;

    while(pIndex < target)
    {
        int currentProcess = fork();
        if (currentProcess < 0)
        {
            printf("fork error\n");
            exit(1);
        }

        if (currentProcess == 0)
        {
            //child process, index by 1 and check the loop again
            pIndex++;
        }else{
            //parent prcoess, break out of the loop, pIndex should be what we want, to some computation
            break;
        }
    }
    printf("The pIndex of this process is %d\n",pIndex);
}


int main(int argc, char *argv[])
{
    int fdIn, i, cnt;
    char desired = *argv[2];   
    int counter = 0;
    int buffSize = BUFSIZE;
    int children;

    bool runRead = true;
    bool runMmap = false;
    bool runFork = false;


    //This code does what we need to do based on arg 4
    if (argc == 4)
    {
        if (!(strcmp(argv[3],"mmap")))
        {
            runMmap = true;
	        runRead = false;
            runFork = false; /*should already be false*/
        }
        else if (argv[3][0] == 'p')
        {
            printf("time to run w/ parallelization \n");
            runMmap = false; /*should already be false*/
	        runRead = false;
            runFork = true;
            children = atoi(argv[3]+1);
            if (children > 16)
            {
                children = 16;
                printf("sorry bud, the max is 16\n");
            }
            if (children < 1)
            {
                children = 1;
                printf("Must be at least 1\n");
            }
            printf("we ahve %d children\n",children);
        }
        else
        {
            buffSize = atoi(argv[3]);
            if (buffSize > 8192)
            {
                printf("sorry friend, we're setting it to 8192\n");
                buffSize = 8192;

            }
	        runRead = true;
	        runMmap = false; /*should already be false*/
            runFork = false; /*should already be false*/
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
        printf("occurebnces is %i\n", counter);

    } 
    else if (runMmap)
    {
        char* pchFile;
        mmapFunction(fdIn,pchFile, desired, &counter);
        printf("MMAP CODE HERE BABY\n");
        printf("occurebnces is %i\n", counter);
    } else if (runFork)
    {
        //run forkFunction
        printf("FORK CODE HERE BABY\n");
    }

    if (fdIn > 0){
        close(fdIn);
    }
}