#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define BUFSIZE 1024

int main(int argc, char *argv[])
{
    char buf[BUFSIZE];
    int fdIn, cnt, i;
    char desired = *argv[2];

    int counter = 0;

    if (argc < 2) {
	fdIn = 0;  /* just read from stdin */
    }
    // arg3 = * argv[3]
    // if (argv == 3)
    // {
	//     //test if its an int 
    // }
    else if ((fdIn = open(argv[1], O_RDONLY)) < 0) {
	fprintf(stderr, "file open\n");
	exit(1);
    }
    copy input to stdout

    //Run this code if we are reading
    while ((cnt = read(fdIn, buf, BUFSIZE)) > 0) 
    {
	    int size = sizeof(buf);
	    for (int i =0; i < size; i++)
	    {
		    if (buf[i]==desired)
		    {
			    counter++;
		    }
	    }
	    
    }
    if (fdIn > 0)
        close(fdIn);
    printf("occurebnces is %i\n", counter);
}