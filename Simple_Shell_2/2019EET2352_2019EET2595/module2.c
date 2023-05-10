//     CREATING FILE FO 'X' BYTES USING SYSTEM CALLS

#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<time.h>

int main(int argc, char** argv)
{
	int fd;
	clock_t one_mb_time, ten_mb_time, hundred_mb_time;

	one_mb_time = clock();
	fd = open("file_sys_1MB.txt", O_RDWR | O_CREAT, 0777);
	lseek(fd, 1023, SEEK_CUR);
    write(fd, "\0", 1);
    close(fd);
    one_mb_time = clock()-one_mb_time;
    printf("File of 1 MB created, time: %fms\n",((double)one_mb_time/CLOCKS_PER_SEC)*1000);
    //write(0, "File of 1 MB created\n", 22);


    ten_mb_time = clock();
    fd = open("file_sys_10MB.txt", O_RDWR | O_CREAT, 0777);
	lseek(fd, 10*1024-1, SEEK_CUR);
    write(fd, "\0", 1);
    close(fd);
    ten_mb_time = clock()-ten_mb_time;
    printf("File of 10 MB created, time: %fms\n",((double)ten_mb_time/CLOCKS_PER_SEC)*1000);
    //write(0, "File of 10 MB created\n", 22);


    hundred_mb_time = clock();
    fd = open("file_sys_100MB.txt", O_RDWR | O_CREAT, 0777);
	lseek(fd, 100*1024-1, SEEK_CUR);
    write(fd, "\0", 1);
    close(fd);
    hundred_mb_time = clock()-hundred_mb_time;
    printf("File of 100 MB created, time: %fms\n",((double)hundred_mb_time/CLOCKS_PER_SEC)*1000);
    //write(0, "File of 100 MB created\n\n", 24);
	return 0;
}