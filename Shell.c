#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define inpLen 512
#define maxtoken 10

char *redonitor();
int executioner(char **);
char **tokenizer(char *);
void handler(int sig);
int detectpipe(char**);
int pipeexec(char**  );
int andExecution(char**);
int redirectcheck(char** );
int ii = 0;
int piip = 0;
int status;


int main()
{
    char *input;
    char **tokenarray=malloc(500*sizeof(char *));
    int  count,statusand;

    signal(SIGCHLD, handler);

    do
    {

        printf("madhurjain@12040820>");

        input = redonitor();

        tokenarray = tokenizer(input);
        statusand=andExecution(tokenarray);
         ii=0;

        free(tokenarray);
        free(input);
    } while (statusand);

    return 0;
}

char *redonitor()
{
    char *inp = (char *)malloc(inpLen + 1);

    fgets(inp, inpLen + 1, stdin);
    return inp;
}

void handler(int sig)
{
    
    waitpid(-1, &status, WNOHANG);
}


char **tokenizer(char *inp)
{
    

    char delim[] = " \t\r\n";
    char **tokenList = malloc(maxtoken * sizeof(char *));
    char *token;

    if (!tokenList)
    {
        perror("no tokens\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(inp, delim);
    while (token != NULL)
    {
        tokenList[ii] = token;
        ii++;
        token = strtok(NULL, delim);
    }

    tokenList[ii] = NULL;
    return tokenList;
}



int executioner(char **args)
{
    pid_t pid;
    
    int dtpipe;
    char* outFile;
    FILE* outfp;
    int outRedirect;
    dtpipe=detectpipe(args);
    if (dtpipe==0){
        pipeexec(args);
        return 1;
    };
    outRedirect=redirectcheck(args);
    if (args[0] == NULL)
    {
        return 1;
    }

    if (strcmp(args[0], "exit") == 0)
    {
        return 0;
    }
    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
        {
            fprintf(stderr, "Folder name was expected \"cd\"\n");
        }
        else
        {
            if (chdir(args[1]) != 0)
            {
                perror("This directory doesn't exist");
            }
        }

        return 1;
    }

    pid = fork();
    if (pid == 0)
    {   
        if (outRedirect >= 0) {
            outFile = args[outRedirect + 1];
            args[outRedirect] = NULL;

            outfp = freopen(outFile, "w", stdout);
        };
        
        if (execvp(args[0], args) !=0)
        {
            perror("System call was unsucessful");
            return -2;
        }

        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {
        perror("child process error");
    }

    else
    {
        {
            waitpid(pid, &status, WUNTRACED);
            
        }
    }

    return 1;
}

int detectpipe(char **args)
{

    for ( piip ; args[piip] != NULL; piip++)
    {
        if (strcmp(args[piip], "|") == 0)
        {
            return 0;
        }
        
    };
     return -1;
}

void execArgsPiped(char** parsed, char** parsedpipe)
{
    int pipefd[2]; 
    pid_t p1, p2;
  
    if (pipe(pipefd) < 0) {
        printf("\nPipe could not be initialized");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCould not fork");
        return;
    }
  
    if (p1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);
  
        if (execvp(parsed[0], parsed) < 0) {
            printf("\nCould not execute command 1..");
            exit(0);
        }
    } else {

        p2 = fork();
  
        if (p2 < 0) {
            printf("\nCould not fork");
            return;
        }

        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsedpipe[0], parsedpipe) < 0) {
                printf("\nCould not execute command 2..");
                exit(0);
            }
        } else {
           
            close(pipefd[0]);
            close(pipefd[1]);
            wait(NULL);
            wait(NULL);
        }
    }
}



int pipeexec(char** args )
{


      char **arr1=malloc(100*sizeof(char *));
      char **arr2=malloc(100*sizeof(char *));
      int k1,k2;
      k1=0;
      k2=0;
      for(int i = 0; i < ii; i++)  
    {  
      char* temp;
        if(i < piip)  
            //arr1[k1++] = args[i];  
         { temp=strdup(args[i]);
          arr1[k1++]=temp;}
         
        if(i>piip){  
           temp=strdup(args[i]);
          arr2[k2++]=temp;  
    }  }
    arr1[k1]=NULL;
    
    arr2[k2]=NULL;
    execArgsPiped(arr1,arr2);
    main();    
    }


int andExecution(char ** args){
    
    char **arr3=malloc(100*sizeof(char *));
    int j=0;
    int f1=0;
    for ( int m=0 ; args[m] != NULL; m++)
    {
        if (strcmp(args[m], "&&") == 0)
        {  arr3[j]=NULL;
           f1= executioner(arr3);
           if(status!=0){
               return 1;
           }
           else{
               m++;
               j=0;

           }
        }
        if(strcmp(args[m], "&&") != 0){
            arr3[j]=args[m];
            j++;

        }
    };
    arr3[j]=NULL;
    executioner(arr3);

     return 1;


}
int redirectcheck(char** args)
{
    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], ">") == 0) {
            return i;
        }
    }

    return -1;
}