#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


void M_Error();


int main()
{
    int fd[2]; //fd[0] for reading, fd[1] for writing.
    int pipe_status = pipe(fd);
    if(pipe_status == -1) M_Error("pipe_status == -1");

    pid_t pid = fork();
    if(pid == -1) M_Error("pid == -1");
    
    if(pid == 0)
    {
        //Child
        close(fd[0]); //Close unused read end.

        int dup2_status = dup2(fd[1], 1); //Changing the file descriptor number from fd[1] to 1 (1 represents STDOUT). Now execlp() will write to the pipe fd[1] instead of STDOUT.
        if(dup2_status == -1) M_Error("dup2_status == -1 (child code)");
        int execlp_status = execlp("ls", "ls", "/", NULL);
        if(execlp_status == -1) M_Error("execlp_status == -1 (child code)");
    }
    else
    {
        //Parent
        close(fd[1]); //Close unused write end.

        int child_wait_status;
        pid_t child_pid = wait(&child_wait_status); //Wait for child process to finish writing to the pipe.
        if(child_wait_status == -1) M_Error("child_wait_status == -1");
        if(pid != child_pid) M_Error("pid != child_pid"); //Happens if multiple child processes are created and they don't match.

        int dup2_status = dup2(fd[0], 0); //Changing the file descriptor number from fd[0] to 0 (0 represents STDIN). Now execlp() will use the input provided by the pipe fd[0] as input instead of STDIN.
        if(dup2_status == -1) M_Error("dup2_status == -1 (parent code)");
        int execlp_status = execlp("wc", "wc", "-l", NULL);
        if(execlp_status == -1) M_Error("execlp_status == -1 (parent code)");
    }
    
    return 0;
}


void M_Error(char* error_message)
{
    perror(error_message);
    exit(1);
}