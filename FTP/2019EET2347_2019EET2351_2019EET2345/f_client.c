#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>///
#include<sys/wait.h>///
#include<netinet/in.h>
#include<sys/stat.h>  //for getting file size using stat()
#include<sys/sendfile.h> /*for sendfile()*/
#include<fcntl.h>
#include<pthread.h>
#include<arpa/inet.h>

#define MAXBYTES 32*128
#define MAXARGS 32
#define safe_free(it)   if (it) {free(it);it=NULL;}

void flush_stdin()
{
        int a = getchar();

        while (a != '\n' && a != EOF){

                a = getchar();
        }
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


int internal_cmd(char **argv){

        if (!strcmp(argv[0],"lls")){
		argv[0] = "ls";
		execute_cmd(argv);
                return 1;
        } else if (!strcmp(argv[0],"lchmod")){
		
		if (argv[1] == NULL){
			printf("Too few arguments\n");
		}

		if (argv[3] != NULL){
			printf("Too many arguments\n");
		}

		/*printf("mode %d\n",atoi(argv[1]));
		if (chmod(argv[2],atoi(argv[1])) < 0){
			printf("Failed to execute\n");
		}*/
		argv[0] = "chmod";
		execute_cmd(argv);

		return 1;
        } else if (!strcmp(argv[0],"lcd")){

		if (argv[2] != NULL){
                        printf("Too many arguments\n");
                        return 1;
                }
                if(chdir(argv[1]) < 0){
                        printf("No Such directory\n");
                }
		return 1;
	} else if (!strcmp(argv[0],"lpwd")){
                argv[0] = "pwd";
                execute_cmd(argv);
                return 1;
        }

	return 0;
}



int remove_space(char **output, size_t len, const char *start)
{
        if(len == 0)
                return 0;

        const char *last;
        int output_size;

        while(isspace((unsigned char)*start)){
                start++;
        }

        if(*start == 0)  // All spaces?
        {
                output = NULL;
                return 0;
        }

        last = start + strlen(start) - 1;
        while(last > start && isspace((unsigned char)*last)) {
                last--;
        }
        last++;

        output_size = (last - start) < len-1 ? (last - start) : len-1;

        *output = (char *)malloc(sizeof(char) * (output_size+1));
        memset(*output, '\0', output_size+1);
        memmove(*output, start, output_size);
        output[output_size+1] = '\0';
        return output_size;
}


int read_input(char *str,char **out){

        memset(str, '\0', MAXBYTES);
        printf("Myftp-$~");
        if(fgets(str,MAXBYTES,stdin) == NULL){
                printf("Error reading input\n");
                flush_stdin();
                return 0;
        }

        if (str[strlen(str) - 1] != '\n'){
                printf("Input size exceeded\n");
                str[strlen(str) - 1] = '\0';
                flush_stdin();
        }

        if (str[0] == '\n'){
                return 0;
        }

        if (!remove_space(out, strlen(str), str)){
                return 0;
        }

        return 1;
}


int execute_server(int sock, char **args, char *buffer){


	char resp[MAXBYTES] = "";
	struct timeval time;
    	time.tv_sec = 5; 
    	time.tv_usec = 0;

	printf("Sending command %s\n",buffer);

	if (!strcmp(args[0],"ls")) {

		char *list = NULL;
		int size = 0;
                int c;

		if(send(sock, buffer, strlen(buffer), 0) < 0){
                        printf("Error sending\n");
                        return 0;
                }
		
		setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&time,sizeof(time));
		if(recv(sock, &size, sizeof(int), 0) < 0){
			printf("Error receiving\n");
			return 0;
		}

		if(!size) {
			printf("No output recieved\n");
			return 0;
		}

		list = (char *)malloc(sizeof(char) * size);
		if(recv(sock, list, sizeof(char) * size, 0) < 0){
                        printf("Error receiving\n");
                        return 0;
                }
		printf("%s\n",list);
		safe_free(list);
		
	} else if (!strcmp(args[0],"close")){

                int i = 0;
                
		if(send(sock, buffer, strlen(buffer), 0) < 0){
                        printf("Error sending\n");
                        return 0;
                }

		if(recv(sock, &i, sizeof(int), 0) < 0){
                        printf("Error receiving\n");
                        return 0;
                }

		if (i){
			printf("Closed \n");
			return 1;
		}
                
		return 1;
	
	} else if (!strcmp(args[0],"chmod")){

                char *list = NULL;
                int size = 0;
                int c;

                if(send(sock, buffer, strlen(buffer), 0) < 0){
                        printf("Error sending\n");
                        return 0;
                }

                setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&time,sizeof(time));
                if(recv(sock, &size, sizeof(int), 0) < 0){
                        printf("Error receiving\n");
                        return 0;
                }

                if(!size) {
                        //printf("No output recieved\n");
                        return 0;
                }

                list = (char *)malloc(sizeof(char) * size);
                if(recv(sock, list, sizeof(char) * size, 0) < 0){
                        printf("Error receiving\n");
                        return 0;
                }
                printf("%s\n",list);
                safe_free(list);

	} else if (!strcmp(args[0],"cd")){

                if(send(sock, buffer, strlen(buffer), 0) < 0){
			printf("Error sending\n");
			return 0;
                }

                int status;
                if(recv(sock, &status, sizeof(int), 0) < 0){
			printf("Error Receiving\n");
			return 0;
		}

                if(status)
                        //printf("Remote directory successfully changed\n")
			;
                else
                        printf("No such file or directory\n");

        } else if (!strcmp(args[0],"pwd")){

		if(send(sock, buffer, strlen(buffer), 0) < 0){
		        printf("Error sending\n");
	         	return 0;
		}

		setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&time,sizeof(time));
		if(recv(sock, resp, sizeof(resp), 0) < 0){
		 	printf("Error receiving\n");
		 	return 0;
		}

		printf("%s\n",resp);

        } else if (!strcmp(args[0],"put")){
	
		FILE *fp = NULL;
		char contents[1024] = "";
		
		if (args[2] != NULL){
			printf("Too many arguments\n");
			return 0;
		}

		if(send(sock, buffer, strlen(buffer), 0) < 0){
                        printf("Error sending\n");
                        return 0;
                }
                
                int size, fd, gr;
                struct stat obj;
                stat(args[1],&obj);
                fd = open(args[1], O_RDONLY);
		size = obj.st_size;
		if(fd == -1){
			size = 0;
			printf("cannot open the file on server \n");
		}
                
		recv(sock, &gr, sizeof(int), 0); //garbage
		if(size)
                {
                	sendfile(sock, fd, NULL, size);
                }
		close(fd);

        } else if (!strcmp(args[0],"get")){

                if (args[2] != NULL){
                        printf("Too many arguments\n");
                        return 0;
		}

                if(send(sock, buffer, strlen(buffer), 0) < 0){
                        printf("Error sending\n");
                        return 0;
                }

                int size, i = 1;
                recv(sock, &size, sizeof(int), 0);
                if(!size)
                {
                        printf("No such file on the server \n");
                        return 0;
                }
                int filehandle,j;
                char *f = malloc(MAXBYTES);
                char filename[sizeof(buffer-4)];
                strcpy(filename, args[1]);
                printf("get_filename: %s \n",filename);
                while(1)
                {
                        filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
                        if(filehandle == -1)
                        {
                                sprintf(filename + strlen(filename), "(copy%d)", i++);//needed only if same directory is used for both server and client
                        }
                        else break;
                }
                setsockopt(sock,SOL_SOCKET,SO_RCVTIMEO,(char *)&time,sizeof(time));
                while(recv(sock, f, sizeof(f), 0)>0)
                {
                        if(!write(filehandle, f, sizeof(f)))
                                break;
                }
                close(filehandle);

        } else {
		printf("Invalid Command\n");
	}

	return 0;

}

void send_rec(int sock){

	char input_line[MAXBYTES] = "";
        char *op_line = NULL;
        char *op_line_o = NULL;
	char *sep_arr[MAXARGS] = {0};
        char space[] = " ";
        int arg_cnt = 0;
	int op = 0;


	while(1){
		if(!read_input(input_line,&op_line)) continue;
		op_line_o = strdup(op_line);
                arg_cnt = parse_string(op_line,space,sep_arr,(sizeof(sep_arr)/sizeof(sep_arr[0])));
		if (arg_cnt > 0){
			/*for (int i=0;i<arg_cnt;i++){
			  printf("%s\n",sep_arr[i]);
			  }*/
			if (arg_cnt >= MAXARGS){
				sep_arr[MAXARGS - 1] = NULL;
			} else {
				sep_arr[arg_cnt] = NULL;
			}

			if (internal_cmd(sep_arr)){
				safe_free(op_line);
				safe_free(op_line_o);
				continue;
			}

			op = execute_server(sock,sep_arr,op_line_o);
			if (op) {
				safe_free(op_line);
				safe_free(op_line_o);
				break;
			}
		}
		safe_free(op_line);
		safe_free(op_line_o);
	}
}



int main(int argc, char **argv){


	if (argc != 3){
		printf("Please provide Valid Connection information\n");
		return 0;
	}

	int client_socket = socket(AF_INET, SOCK_STREAM, 0);///create the socket
	if(client_socket == -1){
		printf("Error: Failed socket creation");
		exit(1);
	}
	
	printf("Connetion address %s:%s\n",argv[1],argv[2]);

	///define the client address
	struct sockaddr_in client_address;
	client_address.sin_family = AF_INET;
	client_address.sin_port = htons(atoi(argv[2]));
	client_address.sin_addr.s_addr = inet_addr(argv[1]);

	int k = connect(client_socket, (struct sockaddr *) &client_address, sizeof(client_address));
	if(k == -1){
		printf("Error: connection failed");
		exit(1);
	}

	send_rec(client_socket);
	
	close(client_socket);

	return 0;
}
