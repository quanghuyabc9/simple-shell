#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>

#define MAX_LINE 80
#define INPUT_END 1
#define OUTPUT_END 0

int main(void)
{
    char *args[MAX_LINE/2 +1];

    setNULL(args);

    int should_run=1;
    int ret;// a variable which is used for saving return value
    char *history_buffer=NULL;// a pointer is used for save command in history

    while(should_run)
    {
        printf("osh>");
        fflush(stdout);

        char *line= (char*)malloc((MAX_LINE+1)*sizeof(char));

        ret = scanf("%[^\n]", line); // read user's input

        if(ret < 1)
        {
            ret = scanf("%1[\n]", line);
            if(ret < 1)
            {
                printf("STDIN FAILD\n");
                return 1;
            }
            continue;
        }


        char *line_ptr = line;

        while(*line_ptr==' '|| *line_ptr=='\t')// skip space(' ') or tab ('\t')
            line_ptr++;

        //when the user enters exit at the prompt,this program will set should_run to 0 and terminate.
        if(strstr(line_ptr, "exit"))
        {
            should_run=0;
            continue;
        }

        // history feature
        if(line_ptr[0]=='!' && line_ptr[1]=='!')
        {
            if(history_buffer==NULL)
            {
                printf("No commands in history.\n");
                continue;
            }
            else
            {
                printf("Previous Command: %s\n", history_buffer);
                free(line);
                line=(char*)malloc((MAX_LINE+1)*sizeof(char));
                strcpy(line,history_buffer);
                line_ptr=line;
            }
        }
        else
        {
            if(history_buffer!=NULL)
                free(history_buffer);
            history_buffer=(char*)malloc((MAX_LINE+1)*sizeof(char));
            strcpy(history_buffer,line_ptr);
        };

        // The process of shell
        int i=0;

        while(*line_ptr != '\0')
        {
            char *buffer=(char*)malloc((MAX_LINE+1)*sizeof(char));

            args[i]=(char*)malloc((MAX_LINE+1)*sizeof(char));
            ret = sscanf(line_ptr,"%[^ \t]",args[i]);

            if(ret < 1)
            {
                printf("INVALID COMMAND\n");
                return 1;
            }

            line_ptr+=strlen(args[i]);
            sscanf(line_ptr,"%[ \t]",buffer);
            line_ptr+=strlen(buffer); //skip spaces and tabs
            i++;
            free(buffer);
        }

        int waiting_for_the_child = 1;

        if(strlen(args[i-1])==1 && args[i-1][0] == '&')
            waiting_for_the_child = 0;

        int i_modification = 0, o_modification = 0, io_pipe = 0;

        for(int tmp=0; tmp<i; tmp++)
        {
            if(args[tmp][0]=='<')
                i_modification = 1;
            if(args[tmp][0]=='>')
                o_modification = 1;
            if(args[tmp][0]=='|')
                io_pipe = 1;
        }

        pid_t pid = 0;
        pid = fork();

        if(pid<0)
        {
            printf("FORK FAILED\n");
            return 1;
        }
        else if(pid==0)
        {
            //input modification, redirecting input
            if(i_modification)
            {
                if(!waiting_for_the_child)// if it have & at the end of cmd line
                {
                    strcpy(args[i-3],args[i-2]);
                    free(args[i-2]);
                    args[i-2]=NULL;
                    free(args[i-1]);
                    args[i-1]=NULL;
                }
                else
                {
                    strcpy(args[i-2],args[i-1]);
                    free(args[i-1]);
                    args[i-1]=NULL;
                }
                if(execvp(args[0],args))
                {
                    printf("INVALID COMMAND\n");
                    return 1;
                }
            }
            // ouput modification, redirecting output
            else if(o_modification)
            {
                char*filename;

                if(!waiting_for_the_child)// if it have & at the end of cmd line
                {
                    filename=(char*)malloc(strlen(args[i-2])*sizeof(char));
                    strcpy(filename, args[i-2]);
                    free(args[i-3]);
                    args[i-3]=NULL;
                    free(args[i-2]);
                    args[i-2]=NULL;
                    free(args[i-1]);
                    args[i-1]=NULL;
                }
                else
                {
                    filename = (char*)malloc(strlen(args[i-1])*sizeof(char));
                    strcpy(filename, args[i-1]);
                    free(args[i-2]);
                    args[i-2]=NULL;
                    free(args[i-1]);
                    args[i-1]=NULL;
                }

                int file_desc= open(filename, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
                dup2(file_desc,STDOUT_FILENO);
                //close(f);
                if(execvp(args[0],args))
                {
                    printf("INVALID COMMAND\n");
                    return 1;
                }
            }
            //input, ouput with pipe, communication via a pipe
            else if(io_pipe)
            {
                int index;
                // find index of '|'
                for(int i=0; i< MAX_LINE/2 +1; i++ )
                {
                    if(args[i][0]=='|')
                    {
                        index= i;
                        break;
                    }
                }

                char *argsFirstCmd[MAX_LINE/2+1];//first command (command before '|')
                setNULL(argsFirstCmd);
                int ifcmd=0;
                char *argsSecondCmd[MAX_LINE/2+1];// second command (command after '|')
                setNULL(argsSecondCmd);
                int iscmd=0;
                for(int j=0; j< MAX_LINE/2 +1 && args[j] != NULL && args[j][0] !='&'; j++)
                {
                    if(j< index)
                    {
                        argsFirstCmd[ifcmd]= (char*)malloc((MAX_LINE+1)*sizeof(char));
                        strcpy(argsFirstCmd[ifcmd], args[j]);
                        ifcmd++;
                    }
                    else if(j > index)
                    {
                        argsSecondCmd[iscmd]= (char*)malloc((MAX_LINE+1)*sizeof(char));
                        strcpy(argsSecondCmd[iscmd], args[j]);
                        iscmd++;
                    }
                }
                pid_t pid1;
                pid_t pid2;
                int fd[2];

                pipe(fd);
                pid1 = fork();
                //
                if(pid1==0)
                {
                    close(fd[INPUT_END]);
                    dup2(fd[OUTPUT_END], STDIN_FILENO);
                    close(fd[OUTPUT_END]);
                    execvp(argsSecondCmd[0],argsSecondCmd);
                }
                else
                {
                    //
                    pid2=fork();
                    //
                    if(pid2==0)
                    {

                        close(fd[OUTPUT_END]);
                        dup2(fd[INPUT_END], STDOUT_FILENO);
                        close(fd[INPUT_END]);
                        execvp(argsFirstCmd[0], argsFirstCmd);
                    }
                    //
                    close(fd[OUTPUT_END]);
                    close(fd[INPUT_END]);
                    waitpid(-1, NULL, 0);
                    waitpid(-1, NULL, 0);
                }
            }
            //normal command
            else
            {
                if(!waiting_for_the_child) // if it have & at the end of cmd line
                {
                    free(args[i-1]);
                    args[i-1]= NULL;
                }
                if(execvp(args[0],args))
                {
                    printf("INVALID COMMAND\n");
                    return 1;
                }
            }
        }
        else
        {
            if(waiting_for_the_child)
            {
                printf("Waiting for the child!!\n");
                while(wait(NULL)!= pid);
            }
            else
            {
                printf("The child process is running in the background!!\n");
                printf("[1]%d\n",pid);
            }
        }
    }
    return 0;
}


void setNULL(char** args)
{
    for(int i=0; i< MAX_LINE/2 +1; i++)
    {
        args[i]=NULL;
    }
}
