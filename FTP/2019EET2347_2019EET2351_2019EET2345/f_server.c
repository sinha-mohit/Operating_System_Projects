#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<sys/types.h>///
#include<netinet/in.h>
#include <arpa/inet.h>
#include<sys/stat.h>
#include<sys/sendfile.h> /*for sendfile()*/
#include<fcntl.h>
#include<pthread.h>
#define MAXARGS 32
#define safe_free(it)   if (it) {free(it);it=NULL;}

char server_message[100] = "This is the main server";
char buf[100], command[5], filename[20];
struct stat obj;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *runner(void *param);


unsigned int parse_string(char *buffer, char *delim, char **array, unsigned int array_len)
{
        unsigned int cnt = 0;
        char *d;
        size_t dlen = strlen(delim);

        array[cnt++] = buffer;

        while (cnt < array_len && array[cnt - 1]) {

                if ((d = strstr(array[cnt - 1], delim))) {

                        *d = '\0';
                        d += dlen;
                        array[cnt++] = d;
                } else
                        break;
        }

        return cnt;
}


char *catstr(char *str1, char *str2){

        size_t len1 = strlen(str1);
        size_t len2 = strlen(str2);
        char *slash = "/";
        char *op_str = NULL;

        op_str = (char *)malloc(sizeof(len1+len2+1));
        memset(op_str, '\0',len1+len2+2);

        memcpy(op_str,str1,len1);
        memcpy(op_str+len1,slash,1);
        memcpy(op_str+len1+1,str2,len2+1);
        return op_str;

}



void execute_cmd(char **args){

        char *dir = NULL;
        pid_t pid;
        dir = catstr("/bin",args[0]);

        if (dir == NULL){
                printf("Command does not exist\n");
                return;
        }

        pid = fork();

        if (pid == -1){
                printf("Failed to fork child process\n");
                return;
        } else if (pid == 0){
		int fd = open("/tmp/temps.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

		dup2(fd, 1);
		dup2(fd, 2);

		close(fd);

                if(execvp(dir,args) == -1){
                        printf("Execution of file failed\n");
                }
                exit(0);
        } else {
                wait(NULL);
        }
        safe_free(dir);
        return;

}



int main()
{

	int server_socket = socket(AF_INET, SOCK_STREAM, 0);///create the socket
	int client_socket;
	if(server_socket == -1){
		printf("Error: Failed socket creation");
		exit(1);
	}

	//define the server address
	struct sockaddr_in s_address, c_address;
	s_address.sin_family = AF_INET;
	s_address.sin_port = htons(55056); // short integer from host byte order to network byte order. 
	s_address.sin_addr.s_addr = INADDR_ANY; ///trying to connect to 0.0.0.0(shortcut library)

	int k, len;
	k = bind(server_socket, (struct sockaddr*) &s_address, sizeof(s_address));///above wrong, should be this
	if(k == -1){
		printf("Error: Binding failed");
		exit(1);
	}
	k = listen(server_socket, 50);///second arguments defines how many connections waiting in backlog for a given time. 
	///for multiple client, it should not be 1.
	
	if(k == -1){
		printf("Error: Failed in listening");
		exit(1);
	}
	len = sizeof(c_address);
	pthread_t tid[60];
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	int i = 0;
	while(1){
		client_socket = accept(server_socket, NULL, NULL);///doubt here, might won't be null for multiple client.
		printf("Connection accepted\n");

		if(client_socket >= 0){
			pthread_create(&tid[i++], &attr, runner, &client_socket);
		}
		if( i >= 50)
        {
            i = 0;
            while(i < 50)
            {
                pthread_join(tid[i++],NULL);
            }
            i = 0;
        }
	}

	close(server_socket);
	return 0;
}

void *runner(void *param)
{
	int client_socket = *((int *)param);
	int c, size;
	char pwd[100] = "";
	printf("thread created\n");
	getcwd(pwd, sizeof(pwd));
	printf("pwd is: %s \n\n", pwd);

	while(1){
		int fd, i;

		//send(client_socket, server_message, sizeof(server_message), 0);
		if(recv(client_socket, buf, 100, 0) <= 0){
			printf("Closing thread\n");	
			pthread_exit(0);
		}
		pthread_mutex_lock(&lock);
		printf("%s\n",buf);
		sscanf(buf, "%s", command);

		if(!strcmp(command, "ls")){
			char *buf_bkp = NULL;
			int arg_cnt = 0;
			char *sep_arr[MAXARGS] = {0};
		        char space[] = " ";


			buf_bkp = strdup(buf);
			arg_cnt = parse_string(buf_bkp,space,sep_arr,(sizeof(sep_arr)/sizeof(sep_arr[0])));

                	if (arg_cnt > 0){
				if (arg_cnt >= MAXARGS){
                        	        sep_arr[MAXARGS - 1] = NULL;
	                        } else {
        	                        sep_arr[arg_cnt] = NULL;
                	        }
	
				if(chdir(pwd) == 0)
        	                        c = 1;
                	        else
                        	        c = 0;

				execute_cmd(sep_arr);

			}
			/*if(chdir(pwd) == 0)
				c = 1;
			else
				c = 0;
			system("ls >temps.txt");*/
			stat("/tmp/temps.txt",&obj);
			size = obj.st_size; // gives the size of the file
			fd = open("/tmp/temps.txt", O_RDONLY, 0777);
			pthread_mutex_unlock(&lock);
			send(client_socket, &size, sizeof(int),0);//sanket size bhej raha hu taki issi size ka buffer banake receive karna
			sendfile(client_socket,fd,NULL,size);
			close(fd);
			remove("/tmp/temps.txt");
			safe_free(buf_bkp);

		} else if(!strcmp(command, "chmod")){
                        char *buf_bkp = NULL;
                        int arg_cnt = 0;
                        char *sep_arr[MAXARGS] = {0};
                        char space[] = " ";


                        buf_bkp = strdup(buf);
                        arg_cnt = parse_string(buf_bkp,space,sep_arr,(sizeof(sep_arr)/sizeof(sep_arr[0])));

                        if (arg_cnt > 0){
                                if (arg_cnt >= MAXARGS){
                                        sep_arr[MAXARGS - 1] = NULL;
                                } else {
                                        sep_arr[arg_cnt] = NULL;
                                }

                                if(chdir(pwd) == 0)
                                        c = 1;
                                else
                                        c = 0;

				execute_cmd(sep_arr);
			}
                        stat("/tmp/temps.txt",&obj);
                        size = obj.st_size; // gives the size of the file
                        fd = open("/tmp/temps.txt", O_RDONLY, 0777);
                        pthread_mutex_unlock(&lock);
                        send(client_socket, &size, sizeof(int),0);//sanket size bhej raha hu taki issi size ka buffer banake receive karna
                        sendfile(client_socket,fd,NULL,size);
                        close(fd);
                        remove("/tmp/temps.txt");
                        safe_free(buf_bkp);


		} else if(!strcmp(command,"get")){
			if(chdir(pwd) == 0)
				c = 1;
			else
				c = 0;
			sscanf(buf, "%s%s", filename, filename);
			printf("file name: %s \n", filename);
			stat(filename, &obj);
			fd = open(filename, O_RDONLY);
			size = obj.st_size;
			if(fd == -1){
				size = 0;
				printf("cannot open the file on server \n");
			}

			pthread_mutex_unlock(&lock);
			send(client_socket, &size, sizeof(int), 0);//dusre side size compare karlena
			if(size)
				sendfile(client_socket, fd, NULL, size);
			close(fd);
		}
		else if(!strcmp(command, "put")){
			int c1 = 0, len;
			char *f;
			struct timeval time;
			time.tv_sec = 5; 
			time.tv_usec = 0;

			if(chdir(pwd) == 0)
				c = 1;
			else
				c = 0;
			printf("c = %d \n", c);
			sscanf(buf, "%s%s", filename, filename);
			//strcpy(filename,buf+4);
			i = 1;
			while(1){
				fd = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
				if(fd == -1){
					sprintf(filename + strlen(filename), "%d", i);
				}
				else
					break;
			}
			f = malloc(4096);
			pthread_mutex_unlock(&lock);
			send(client_socket, &i, sizeof(int), 0);//garbage
			setsockopt(client_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&time,sizeof(time));
			while(recv(client_socket, f, sizeof(f), 0)>0)
			{
				if(!write(fd, f, sizeof(f)))
					break;
			}
			time.tv_sec = 0;
			time.tv_usec = 0;
			setsockopt(client_socket,SOL_SOCKET,SO_RCVTIMEO,(char *)&time,sizeof(time));
			close(fd);
		}

		else if(!strcmp(command, "pwd")){
			pthread_mutex_unlock(&lock);
			send(client_socket, pwd, sizeof(pwd), 0);
		}

		else if(!strcmp(command, "cd"))
		{
			if(chdir(buf+3) == 0)
				c = 1;
			else
				c = 0;
			getcwd(pwd, sizeof(pwd));
			pthread_mutex_unlock(&lock);
			send(client_socket, &c, sizeof(int), 0);
		}

		else if(!strcmp(command, "close")){
			printf("FTP server closing..\n");
			i = 1;
			pthread_mutex_unlock(&lock);
			send(client_socket, &i, sizeof(int), 0);
			close(client_socket);
			bzero(buf, sizeof(buf));
			bzero(command, sizeof(command));
			pthread_exit(0);
		}

		else {
			pthread_mutex_unlock(&lock);
			printf("Command Not found \n");
		}

		bzero(buf, sizeof(buf));
		bzero(command, sizeof(command));

	}
}
