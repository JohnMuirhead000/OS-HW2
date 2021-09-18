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
    if(fstat(fdIn, &sb) < 0)
    {
	perror("Could not stat file to obtain its size");
	exit(1);
    }

    if ((pchFile = (char *) mmap (NULL, sb.st_size, PROT_READ, MAP_SHARED, fdIn, 0)) 
	== (char *) -1)	
    {
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

void forkFunction(int target, int fdIn, char desired, int* counter);
void forkFunction(int target, int fdIn, char desired, int* counter)
{
    struct stat sb;
    if(fstat(fdIn, &sb) < 0)
    {
	perror("Could not stat file to obtain its size");
	exit(1);
    }

    int pages = (sb.st_size / 4096) + 1;
    int pagesPerIteration[2][target];
    if(target > pages)
    {
        printf("Sorry frined, the file is not large enought to run w/ %d processes\n", target);
        exit(1);
    } 
    else 
    {   
        for (int i = 0; i < target; i++)
        {
            pagesPerIteration[0][i] = i+1;
            pagesPerIteration[1][i] = 0;
        }
        int iteration = 0;
        while (pages != 0)
        {
            pagesPerIteration[1][iteration % target] = pagesPerIteration[1][iteration % target] + 1;
            iteration++;
            pages--;
        }
    }
    /*
    BOOM, now pagesPerIteration is a 2 x target matirx with each column holds what number it is
    followed by the number of pages it should read. For anyone who may be reading this, i did think 
    of this mysef. 
    */
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

    printf("the size of the file is %d\n", sb.st_size);
    char *pchFile;

    //this line calculates the size of what we'll be mapping for this process
    int size = (4096) * pagesPerIteration[1][pIndex - 1];
    //this code calcualtes the buffer into the fd we'll start reading from
    int buffer = 0;
    for (int i = 0; i < (pIndex - 1); i++)
    {
        buffer = buffer + (pagesPerIteration[1][i]) * 4096;

    }
    printf("the buffer on this lil dog is goung to be %d \n", buffer);
    if ((pchFile = (char *) mmap (NULL, size, PROT_READ, MAP_PRIVATE, fdIn, buffer))
     == (char *) -1)	
    {
	perror("Could not mmap file");
	exit (1);
    }
    char* pchCopy = pchFile;
    for (int i = 0; i < (sb.st_size/target); i++)
    {
        if (*pchCopy == desired)
        {
            (*counter)++;
        }
	    pchCopy++;
    }
    if(munmap(pchFile, sb.st_size/sb.st_size) < 0)
    {
	perror("Could not unmap memory");
	exit(1);
    }

    printf("Process number %d, found %d instances of the character\n", pIndex, *counter);
    
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
        char* pchFile;
        printf("FORK CODE HERE BABY\n");
        forkFunction(children, fdIn, desired, &counter);
    }

    if (fdIn > 0){
        close(fdIn);
    }
}