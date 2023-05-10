//        IT'S A SIMPLE SHELL

#include<stdio.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include<string.h>
#include<dirent.h>

void shell_loop();
char* read_cmd();
char** parse(char*);
int execute(char** cmd_table);

int cmd_cd(char** args);
int cmd_help(char** args);
int cmd_exit(char** args);
int pushd(char** args);
int popd(char** args);
int dir(char** args);
int path(char** args);
int path_plus(char* args);
int path_minus(char* args);

struct dir_stack
{
	int top;
	int capacity;
	char** dirc;
}STACK;
struct dir_stack *s;

typedef struct pathnode
{
	int no_of_paths;
	int max_paths;
	char** path_list;
}PATHS;
PATHS *p;

int main(int argc, char** argv)
{
 //A SHELL FUNCTIONS IN THREE STEPS	
 //STEP1:- INITIALIZATION
	//LOAD THE CONFIGURATION FILES
	s = (struct dir_stack *)malloc(sizeof(struct dir_stack));
	if(!s)
	{
		printf("stack memory error\n");
	}
	s->top = -1;
	s->capacity = 5;
	s->dirc = (char **)malloc(sizeof(char *)*s->capacity);
	
	p = (PATHS *)malloc(sizeof(PATHS));
	p->no_of_paths = -1;
	p->max_paths = 5;
	p->path_list = (char **)malloc(sizeof(char *)*p->max_paths);
	


 //STEP2:- INTERPRETATION
	//IT'S AN RPEL 
	  shell_loop();


 //STEP3:- TERMINATION
	//FREE ALL THE ALLOCATED MEMORIES AND EXIT
	  free(s);
	  free(p);
 return 0;	
}

void shell_loop()
{
	char* cmd_line;
	char** cmd_table;
	int status=1;

	//IT'S A DO WHILE RPEL 
	do
	{
	   printf("\nmyshell@cmd>  ");
     //STEP1:- READ
        //READ THE INPUT AS STRING FROM STDIN 
	      cmd_line = read_cmd();
	      if(strcmp(cmd_line, "\n")==0)
	      	continue;

     //STEP2:- PARSE
	    //HERE WE WILL TOKENIZE THE CMD_LINE FOR CREATING A COMMAND TABLE AS ARRAY OF STRINGS         
          cmd_table = parse(cmd_line);
         /* printf("\nGenerated tokens are : \n");
          int i=0;
          while(cmd_table[i] != NULL)
          {
          	printf("%s\n", cmd_table[i]);
          	i++;
          }*/

     //STEP3:- EXECUTE
        //HERE WE WILL EXECUTE THE COMMANDS FORM COMMAND TABLE
          status = execute(cmd_table);
     
    }while(status);  
}


char* read_cmd()
{
	char* cmd_line = NULL;
	unsigned long bufsize = 0;
	getline(&cmd_line, &bufsize, stdin);
	return cmd_line;
}

char** parse(char* cmd_line)
{
	char** cmd_table_tokens;
	char* token;
	int i = 0;
	int max_tokens = 10;
    cmd_table_tokens = (char **)malloc(sizeof(char *)*max_tokens);

    token = strtok(cmd_line, " \t\r\n\a");

    while(token != NULL)
    {
    	cmd_table_tokens[i] = token;
    	i++;
        //printf("%d", i);
    	if(i >= max_tokens-1)
    	{
    		printf("\nNo. of tokens has exceeded the maximum limit\n");
            
            char **old_a = cmd_table_tokens;
            cmd_table_tokens = (char** )malloc(sizeof(char *)*(2*max_tokens));
            for(int i = 0 ; i<p->max_paths ; i++)
	            cmd_table_tokens[i] = old_a[i]; 
            free(old_a);	
            max_tokens = max_tokens*2;
            printf("max_tokens doubled\n");


    	}
    	token = strtok(NULL, " \t\r\n\a");
    }
    cmd_table_tokens[i] = NULL;
	return cmd_table_tokens;
}



// PREDEFINING CD, HELP AND CHDIR CMDS IN OUR SHELL


char* array_of_builtin_cmds[] = { "cd", "help", "exit", "pushd", "popd", "dir", "path"};
int (*array_of_builtin_functions[])(char** args) = { &cmd_cd, &cmd_help, &cmd_exit, &pushd, &popd, &dir, &path};
int no_buitin_cmds = 7;

int cmd_cd(char** args)
{
	if(args[0] == NULL)
	{
		printf("cd command expect some argument\n");
	}
	else
	{   
		char dir[100];
		printf("\nCurrent directory is %s\n", getcwd(dir, 100));
		int temp = chdir(args[1]);
		if(temp != 0)
		{
			printf("Not a valid path to change directory\n");
		}
		else
		{
			printf("\nAfter cd now directory is %s\n", getcwd(dir, 100));
		}
	}
	return 1;
}

int cmd_help(char** args)
{
	int i;
	printf("Welcome to help section of my_shell\n");
	printf("my_shell has following builtin commands\n");
    for(i = 0 ; i<no_buitin_cmds ; i++)
    	printf("%d. %s\n", i+1, array_of_builtin_cmds[i]);
    return 1;
}

int cmd_exit(char** args)
{
	return 0;
}

int pushd(char** args)
{
	if(args[1] == NULL)
	{
		printf("argument should not be null\n");
		return 1;
	}
	else if(s->top == s->capacity-1)
	{
		printf("\nDirectory stack overflow\n");
		char **old_a = s->dirc;
		s->dirc = (char** )malloc(sizeof(char *)*(2*s->capacity));
		for(int i = 0 ; i<s->capacity ; i++)
		    s->dirc[i] = old_a[i]; 
		free(old_a);
		s->capacity = s->capacity*2;
		printf("capacity doubled\n");     
	}
		char *cdir;
		cdir = (char *)malloc(sizeof(char)*100);
		getcwd(cdir, 100);
		int temp = chdir(args[1]);
		if(temp == 0)
		{
		    s->top = s->top+1;
		    s->dirc[s->top] = (char *)malloc(sizeof(cdir));
		    s->dirc[s->top] = cdir; 
		}
		else
			printf("directory error\n");
	return 1;
}

int popd(char** args)
{	
  if(s->top == -1)
  {
  	printf("\nDirectory stack is empty\n");
  }
  else
  {
  	chdir(s->dirc[s->top]);
  	s->top--;
  }
  return 1;
}

int dir(char** args)
{
	if(s->top == -1)
	{
		printf("stack is empty\n");
		return 1;
	}
	for(int i = 0 ; i<=s->top ; i++)
	{
		printf("%s/", s->dirc[i]);
	}
	printf("\n");
	return 1;
}


int path_plus(char* args)
{
 int i = 0;	
 while(i<=p->no_of_paths)
 {
 	if(strcmp(p->path_list[i],args)==0)
 	{
 		printf("path already present\n");
        return 1;
 	}
 	i++;
 }	
 if(p->no_of_paths == p->max_paths-1)
 {
  printf("\npath overflow\n");
  char **old_a = p->path_list;
  p->path_list = (char** )malloc(sizeof(char *)*(2*p->max_paths));
  for(int i = 0 ; i<p->max_paths ; i++)
	  p->path_list[i] = old_a[i]; 
  free(old_a);	
  p->max_paths = p->max_paths*2;
  printf("max_paths doubled\n");
 }	
 p->no_of_paths = p->no_of_paths + 1;
 p->path_list[p->no_of_paths] = (char *)malloc(sizeof(args));
 p->path_list[p->no_of_paths] = args;
 printf("path added\n");
 return 1;
}

int path_minus(char* args)
{
 int i = 0, found = 0;	
 while(i<=p->no_of_paths)
 {
 	if(strcmp(p->path_list[i],args) == 0)
 	{
 		found = 1;
 		for(int j = i+1 ; j<=p->no_of_paths ; j++)
 		{
 			p->path_list[j-1] = p->path_list[j];
 		}
 		p->no_of_paths = p->no_of_paths - 1;
 		printf("path removed\n");
 		return 1;
 	}
 	i++;
 }
 if(found == 0)
 	printf("path is not present\n");
 return 1;
}

int path(char** args)
{
	if(args[1] == NULL)
	{	
	 int i = 0;
	 for(i = 0 ; i<=p->no_of_paths ; i++)
	 {
      printf("%s:",p->path_list[i]);
	 }
	 if(i == 0)
	 {
	  printf("Path list is empty\n");
	 }
    }

    else if(strcmp(args[1], "+")==0)
    {
    	int i = 2; 
    	while(args[i]!=NULL)
    	{
    	 path_plus(args[i]);
    	 i++;
    	}
    	return 1;
    }

    else if(strcmp(args[1], "-")==0)
    {
    	int i = 2;
    	while(args[i]!=NULL)
    	{
    	 path_minus(args[i]);
    	 i++;
    	}
    	return 1;
    }
    return 1;
}

int search_dir(char** args) 
{ //google to search a directory
 DIR *dr;
 int match = 0, i=0,n;
 struct dirent *de;

 dr = opendir("."); //. matlb current directory
 if(dr == NULL)
 {
  printf("Directory doesn't exist\n");
 }
 else
 {
  while((de = readdir(dr)) != NULL)
  {
   if(strcmp(de->d_name, args[0])==0)
   {
    printf("file found in current dir : %s\n",de->d_name);
    match++;
    return -3;
   }
  }
  closedir(dr);
  if(match == 0)
  {
   printf("file not found in current directory\n");
   //printf("We have following paths too\n");
   printf("looking in path locations\n");
   i=0;
   while((i<=p->no_of_paths) && (match == 0))
   {
    dr = opendir(p->path_list[i]);
    if(dr != NULL)
    {	
	while((de = readdir(dr)) != NULL)
	{
	 if(strcmp(de->d_name, args[0])==0)
	 {
	  printf("file found in %s path\n",p->path_list[i]);
	  match++;
	  return i;
	 }
    }
	closedir(dr);
    }
    i++;
   }
  }
  if(match == 0)
  {
  printf("file not found in current or any path directory\n");	
  return -2;
  }
 }
 return -1;
}

int new_cmd_launch(char** args)
{
	pid_t pid, wait_pid;
	int status;
	pid = fork();

	if(pid == 0)
	{
		//it's child process

		int key = search_dir(args);
		if(key == -3)
		{
		 if(execv(args[0], args) == -1)
		 {
			printf("Not a valid command\n");
		 }
		 exit(0);
		}
		else if(key == -1 || key == -2)
		{
			printf("%s Not a valid command\n", args[0]);
			exit(0);
		}
		else
		{
			char *tempstr = (char *)malloc(sizeof(char)*100);
			//printf("\n%s\n",tempstr);
			strcat(tempstr, p->path_list[key]);
			//printf("\n%s\n",tempstr);
			strcat(tempstr, "/");
			//printf("\n%s\n",tempstr);
			strcat(tempstr, args[0]);
			//printf("\n%s\n",tempstr);
			if(execv(tempstr, args) == -1)
		    {
			printf("Not a valid command\n");
		    }
		    exit(0);
		}
		return 1;
	}
	else if(pid < 0)
	{
		printf("Fork error");
	}
	else
	{
		//parent process
		wait(NULL);
	}
	return 1;
}

int execute(char** cmd_table)
{
	int i;
	if(cmd_table == NULL)
		return 1;

	for(i = 0 ; i<no_buitin_cmds ; i++)
	{
		if(strcmp(cmd_table[0], array_of_builtin_cmds[i])==0)
		{
			int temp;
			temp = (*array_of_builtin_functions[i])(cmd_table);
			return temp;			
		}
	}
	return new_cmd_launch(cmd_table);
}







