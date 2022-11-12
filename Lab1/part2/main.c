#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <mqueue.h>
#include <stdbool.h>
#include <ctype.h>


#define FLAGS 0 //The flag 0 blocks the sending process from sending message if the queue is full.
#define MAX_MESSAGE_SIZE_BYTES 1024
#define MAX_NUMBER_OF_MESSAGES 10
#define CURRENT_NUMBER_MESSAGES 0


const char* MQ_NAME = "/m_message_queue";
const char* END_MESSAGE = "p_end";


int Word_Count(char* message);
void M_Error(char* error_message);


int main()
{
    pid_t pid = fork();
    if(pid == -1) M_Error("pid == -1");

    if(pid == 0)
    {
        //Child
        struct mq_attr attr;
        attr.mq_flags = FLAGS;
        attr.mq_msgsize = MAX_MESSAGE_SIZE_BYTES + 1;
        attr.mq_maxmsg = MAX_NUMBER_OF_MESSAGES;
        attr.mq_curmsgs = CURRENT_NUMBER_MESSAGES;

        mqd_t mqd = mq_open(MQ_NAME, O_CREAT | O_WRONLY,  0600, &attr);
        if(mqd == -1) M_Error("mqd == -1 (child code)");

        FILE *file_pointer = fopen("text.txt", "r");
        if(file_pointer == NULL) M_Error("Can't find file <text.txt>. Terminating program.");
        else
        {
            char message[MAX_MESSAGE_SIZE_BYTES] = "";
            char character;
            int send_status = -1;
            int message_byte_counter = 0;
            while((character = fgetc(file_pointer)) != EOF)
            {
                strncat(message, &character, 1);
                message_byte_counter++;

                if(message_byte_counter == MAX_MESSAGE_SIZE_BYTES)
                {
                    send_status = mq_send(mqd, message, MAX_MESSAGE_SIZE_BYTES + 1, 0); // The last argument, 0, is the priority.
                    if(send_status == -1) M_Error("send_status == -1 (child code)");

                    strcpy(message, "");
                    message_byte_counter = 0;
                }
            }

            //Send the last message in the buffer that may be less than MAX_MESSAGE_SIZE_BYTES.
            if(message_byte_counter > 0)
            {
                send_status = mq_send(mqd, message, MAX_MESSAGE_SIZE_BYTES + 1, 0); // The last argument, 0, is the priority.
                if(send_status == -1) M_Error("send_status == -1 (child code)");
            }

            fclose(file_pointer);
        }

        char end_message[MAX_MESSAGE_SIZE_BYTES];
        strcpy(end_message, END_MESSAGE);
        int end_message_send_status = mq_send(mqd, end_message, MAX_MESSAGE_SIZE_BYTES + 1, 0); //The last argument, 0, is the priority.
        if(end_message_send_status == -1) M_Error("end_message_send_status == -1 (child code)");

        int close_status = mq_close(mqd);
        if(close_status == -1) M_Error("close_status == -1 (child code)");
    }
    else
    {
        //Parent
        struct mq_attr attr;
        attr.mq_flags = FLAGS;
        attr.mq_msgsize = MAX_MESSAGE_SIZE_BYTES + 1;
        attr.mq_maxmsg = MAX_NUMBER_OF_MESSAGES;
        attr.mq_curmsgs = CURRENT_NUMBER_MESSAGES;

        mqd_t mqd = mq_open(MQ_NAME, O_CREAT | O_RDONLY, 0600, &attr);
        if(mqd == -1) M_Error("mqd == -1 (parent code)");

        char message[MAX_MESSAGE_SIZE_BYTES];
        strcpy(message, END_MESSAGE);
        if(message == NULL) M_Error("message == NULL (parent code)");

        int number_of_words = 0;
        while(true)
        {
            int receive_status = mq_receive(mqd, message, MAX_MESSAGE_SIZE_BYTES + 1, 0); //The last argument, 0, is the priority.
            if(receive_status == -1) M_Error("receive_status == -1 (parent code)");

            if(strcmp(message, END_MESSAGE) == 0) break;
            else number_of_words += Word_Count(message);
        }
        printf("%d\n", number_of_words);

        int close_status = mq_close(mqd);
        if(close_status == -1) M_Error("close_status == -1 (parent code)");

        int unlink_status = mq_unlink(MQ_NAME);
        if(unlink_status == -1) M_Error("unlink_status == -1 (parent code)");
    }
}


int Word_Count(char* message)
{
    if(strcmp("", message) == 0) return 0;

    int number_of_words = 0;
    bool last_character_was_break_character = true;

    for(int index = 0; message[index] != '\0' && index < MAX_MESSAGE_SIZE_BYTES; index++)
    {
        if(message[index] == ' ' || message[index] == '\t' || message[index] == '\n')
        {
            last_character_was_break_character = true;
            continue;
        }
        else
        {
            if(last_character_was_break_character == true) number_of_words++;
            last_character_was_break_character = false;
        }
    }

    return number_of_words;
}


void M_Error(char* error_message)
{
    perror(error_message);
    exit(1);
}