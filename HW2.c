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

    if(munmap(pchFile, sb.st_size) < 0){
	perror("Could not uPOOP memory");
	exit(1);
    }
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
        char* pchFile;
        mmapFunction(fdIn,pchFile, desired, &counter);
        printf("MMAP CODE HERE BABY\n");
    }

    if (fdIn > 0){
        close(fdIn);
    }

    printf("occurebnces is %i\n", counter);
}