#include<unistd.h> 
#include<stdio.h> 
#include<stdlib.h>
#include<fcntl.h> 
#include<errno.h> 
#include<time.h>
#include<limits.h>

#define buffer 1000000

int main(int argc, char *argv[])
{
    clock_t t; 
    int fd1 = open(argv[1], O_RDONLY); 
    int fd2 = open(argv[2], O_RDWR | O_CREAT | O_EXCL, 0666); 

    //printf("fd2 = %d\n", fd2);
    //printf("fd1 = %d\n", fd1);

    char *c = (char*)calloc(buffer, sizeof(char));
    int sz1, sz2; //sz3;
    int er1, er2; //er3;

    if (fd1 < 0) 
    { 
        perror("Open error for file 1"); 
        exit(1);
    } 

    if (fd2 < 0) 
    { 
        perror("Open error for file 2"); 
        exit(1);
    } 

    t = clock(); // CLOCK start
    
    while((sz1 = read(fd1, c, buffer)))
    { 
        if(sz1<0)
        {
            perror("Read Error");
        }
        //printf("c1 = %c \n", c1); 
        if(sz1 == buffer)
            sz2 = write(fd2, c, buffer);
        else
        {
            sz2 = write(fd2, c, sz1);
        }

        if(sz2<0)
        {
            perror("Write Error");
        }
    }

    t = clock() - t; // CLOCK ends
    double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 
    printf("Copying took %f seconds to execute \n", time_taken); 

    er1 = close(fd1);
    if (er1 < 0)
    {
        perror("Close error for file 1");
    }

    er2 = close(fd2);
    if (er2 < 0)
    {
        perror("Close error for file 2");
    }  

    if (remove(argv[1]) == 0) 
        printf("File 1 Deleted successfully \n"); 
    else
        printf("Unable to delete the file \n"); 
    
    return 0;
}
