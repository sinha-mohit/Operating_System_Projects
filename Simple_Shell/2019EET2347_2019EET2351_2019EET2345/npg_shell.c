#include <stdbool.h>
#include <dirent.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#define MAXARGS 32
#define MAXBYTES 32*128
#define PATHSIZE 1024
#define safe_free(it)   if (it) {free(it);it=NULL;}


typedef struct dirs {
	char path[PATHSIZE];
	struct dirs *next;
} PATHS;


typedef struct stack {
        char path[PATHSIZE];
        struct stack *next;
} DIRS;


void flush_stdin()
{
        int a = getchar();

        while (a != '\n' && a != EOF){

                a = getchar();
        }
}


bool file_exist(char *file){

	if (access(file, X_OK) != -1){
		return true;
	}
	return false;
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

void print_path(PATHS **node)
{
	PATHS *n = *node;
	while (n != NULL) {
		if (n->next == NULL) printf("%s",n->path);
		else printf("%s:",n->path);
		n = n->next;
	}
	printf("\n");
}

void print_dir(DIRS **node)
{
        DIRS *n = *node;
        while (n != NULL) {
                printf("%s\n",n->path);
                n = n->next;
        }
        printf("\n");
}


char *get_cmd_location(char *file, PATHS *n){

	char mypath[PATHSIZE] = "";
	char *path = NULL;
	getcwd(mypath,PATHSIZE);
	//printf("Current working directory : %s\n",mypath);
	
	path = catstr(mypath,file);
	//printf("Current working directory : %s\n",path);
	if (file_exist(path)){
		//printf("File found in current directory\n");
		return path;
		//execute;
	} else {
		safe_free(path);
		while (n != NULL) {
        		//printf(" %s ", n->path);
			path = catstr(n->path,file);
			if (file_exist(path)){
				//printf("File found\n");
				return path;
			}
			safe_free(path);
        		n = n->next;
    		}

	}

	return NULL;
}


void execute_cmd(char **args, PATHS *p){

	char *dir = NULL;
	pid_t pid;
	dir = get_cmd_location(args[0],p);

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
		//printf("In parent process\n");
		wait(NULL);
		//return;
	}
	safe_free(dir);
	return;

}

int path_exist(char **argv, PATHS **p){

	PATHS *ref_node = *p;
	while (ref_node != NULL){
		if(!strcmp(ref_node->path,argv[2])) return 1;
		ref_node = ref_node->next;
	}
	return 0;
}


void add_path(char **argv, PATHS **p){
	
	PATHS *new = (PATHS *)malloc(sizeof(PATHS));
	strcpy(new->path,argv[2]);
	if (*p == NULL){
		*p = new;
		(*p)->next = NULL;
	} else {
		new->next = (*p);
		(*p) = new;
	}
}



void push_path(char *mydir,DIRS **p){

        DIRS *new = (DIRS *)malloc(sizeof(DIRS));
        strcpy(new->path,mydir);
        if (*p == NULL){
                *p = new;
                (*p)->next = NULL;
        } else {
                new->next = (*p);
                (*p) = new;
        }
}


void remove_path(char **argv, PATHS **p){

	PATHS *t = *p, *last;

	if (t != NULL && !strcmp(t->path,argv[2])){
		*p = t->next;
		free(t);
		return;
	}

	while (t != NULL && strcmp(t->path,argv[2])){
		last = t;
		t = t->next;
	}

	if (t == NULL){
		printf("No such directory available\n");
		return;
	}

	last->next = t->next;
	free(t);
	return;
}

char *pop_path(DIRS **p){
	DIRS *t = *p;
	char *path = malloc(sizeof(char)*PATHSIZE);

	if (t == NULL) return NULL;
	strcpy(path,t->path);

	*p = t->next;
	t->next = NULL;
	safe_free(t);
	return path;
}


void manip_path(char **argv, PATHS **p){

	if (argv[1] == NULL){
		print_path(p);
	} else {
		if (!strcmp(argv[1],"+")){
			
			if(path_exist(argv,p)){
				printf("Path already available\n");
				return;
			} else {
				if(opendir(argv[2])){
					add_path(argv,p);
				} else {
					printf("No such directory exist\n");
					return;
				}
			}
		} else if (!strcmp(argv[1],"-")){
                        remove_path(argv,p);
                } else {
			printf("Enter valid command\n");
		}

	}
	return;
}

int internal_cmd(char **argv, PATHS **p,DIRS **s){

	if (!strcmp(argv[0],"path")){
		manip_path(argv,p);
		return 1;
	} else if (!strcmp(argv[0],"exit")){
		exit(0);
		return 1;
	} else if (!strcmp(argv[0],"cd")){
		if (argv[2] != NULL){
			printf("Too many arguments\n");
			return 1;
		}
		if(chdir(argv[1]) < 0){
			printf("No Such directory\n");
		}
                return 1;
        } else if (!strcmp(argv[0],"pushd")){
		if (argv[2] != NULL){
                        printf("Too many arguments\n");
                        return 1;
                }
		
		char mypath[PATHSIZE] = "";
	        memset(mypath,'\0',PATHSIZE);
        	getcwd(mypath,PATHSIZE);
		//printf("Current working directory : %s\n",mypath);

		if (chdir(argv[1]) >= 0 ){
			push_path(mypath,s);
		} else {
			printf("No such directory\n");
		}	
                return 1;
        } else if (!strcmp(argv[0],"popd")){
		char *pop_dir = NULL;
		pop_dir = pop_path(s);
		if (pop_dir == NULL){
			printf("No directory in dirs\n");
			return 1;
		} else {
			if(chdir(pop_dir) < 0){
        	                printf("Invalid directory popped\n");
	                }
		}
		safe_free(pop_dir);
                return 1;
        } else if (!strcmp(argv[0],"dir")){
		print_dir(s);
                return 1;
        }

	return 0;
}

unsigned int parse_string(char *buffer, char *delim, char **array, unsigned int array_len)
{
        unsigned int cnt = 0;
        char *d;
        size_t dlen = strlen(delim);
        //size_t dlen = 1;

	//printf("Length of delim %ld\n",dlen);
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
	//printf("Output before memmove  %s\n",*output);
	memset(*output, '\0', output_size+1);
	//memcpy(*output, start, output_size);
	memmove(*output, start, output_size);
	output[output_size+1] = '\0';
	//printf("Output size is %d\n",output_size);
	//printf("Start %s .... output %s\n",start,*output);

	return output_size;
}



int read_input(char *str,char *usr, char **out){

	//flush_stdin();
	memset(str, '\0', MAXBYTES);
	//printf("%s-$~",usr);
	printf("MyShell-$~");
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
		//printf("No Input detected\n");
		//flush_stdin();
		return 0;
	}

	if (!remove_space(out, strlen(str), str)){
		//printf("Only whitespaces detected\n");
		return 0;
	}
	

	//printf("Input read is %s %ld\n", str, strlen(str));
	//printf("Input read is %s \n", *out);
	return 1;
}


int main (int argc, char **argv){

	char input_line[MAXBYTES] = ""; //array of char for input line
	char *op_line = NULL;
	char *user = "myshell";
	char *sep_arr[MAXARGS] = {0};
	char space[] = " ";
	int arg_cnt = 0;
	PATHS *head = NULL;
	DIRS *node = NULL; //stack
	/*PATHS *second = NULL;

	head = (PATHS *)malloc(sizeof(PATHS));
	second = (PATHS *)malloc(sizeof(PATHS));
	strcpy(head->path,"/bin");
	head->next = second;
	strcpy(second->path,"/usr/bin");
	second->next = NULL;*/

	//user = getlogin();
	
	while(1){
		//printf("\nInitializing Shell for user %s \n",user);
		if(!read_input(input_line,user,&op_line)) 
			continue;
		//printf("Final Input string %s... %ld\n",op_line,strlen(op_line));
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
		}


		if (internal_cmd(sep_arr,&head,&node)){
			//printf("Executed Internal command\n");
			safe_free(op_line);
			continue;
		}

		execute_cmd(sep_arr,head);

		safe_free(op_line);
	}

	/*safe_free(head);
	safe_free(second);*/
	return 0;
}
