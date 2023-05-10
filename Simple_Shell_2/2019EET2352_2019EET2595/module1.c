//THIS PROGRAM IS GOING TO APPEND THE CONTENT OF A FILE TO ANOTHER FILE AND DELETE THE FIRST FILE

#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<string.h>
#include<time.h>

void copy_file(char* read_file, char* write_file);

int main(int argc, char** argv)
{
	clock_t copy_time;
	copy_time = clock();
    copy_file(argv[1], argv[2]);
    //remove(argv[1]);
    unlink(argv[1]);
    copy_time = clock() - copy_time;
    printf("Time taken for copy : %fms\n",((double)copy_time/CLOCKS_PER_SEC)*1000);
	return 0;
}

void copy_file(char* read_file, char* write_file)
{
	int rd_fd, wr_fd, bytes_read, bytes_written;
	char* buffer;

	rd_fd = open(read_file, O_RDONLY, 0777);
	//printf("%d\n", rd_fd);
	if(rd_fd == -1)
	{
		write(0, "Read file not found.\n", 23);
		exit(0);
	}

	else
	{
		int count = 0;
		
		struct stat read_file_stat;

		if(stat(read_file, &read_file_stat)<0)
			exit(1);
	    count = read_file_stat.st_size;
	    //printf("%d",count);

		// ALLOCATING MEMORY TO BUFFER ACCORDINGLY*/
		buffer = (char *)malloc(sizeof(char)*count);

        bytes_read = read(rd_fd, buffer, count);
        
        char* tempstr = (char *)malloc(sizeof(write_file)+sizeof(read_file)+1);
        strcat(tempstr, write_file);
        strcat(tempstr,"/");
        strcat(tempstr,read_file);
        //wr_fd = open(write_file, O_EXCL | O_WRONLY | O_CREAT, 0777);
        wr_fd = open(tempstr, O_EXCL | O_WRONLY | O_CREAT, 0777);
        if(wr_fd == -1)
	    {
		 write(0, "Write file not found.\n", 24);
		 exit(0);
	    }

        bytes_written = write(wr_fd, buffer, bytes_read);
 
        free(buffer);
        close(wr_fd);

	}
	close(rd_fd);
	write(0, "File copied successfully\n", 26);
}